//
// Author: Yunhong Gu, gu@lac.uic.edu
//
// Description: 
//
// Assumption: This code does NOT process sequence number wrap, which will overflow after 2^31 packets.
//             But I assume that you won't run NS for that long time :)
//
// Last Update: 03/20/2006
//

#include <stdlib.h>
#include <math.h>
#include "ip.h"
#include "udt.h"

int hdr_udt::off_udt_;

static class UDTHeaderClass : public PacketHeaderClass 
{
public:
   UDTHeaderClass() : PacketHeaderClass("PacketHeader/UDT", sizeof(hdr_udt)) 
   {
      bind_offset(&hdr_udt::off_udt_);
   }
} class_udthdr;

static class UdtClass : public TclClass 
{
public:
   UdtClass() : TclClass("Agent/UDT") {}
   TclObject* create(int, const char*const*) 
   {
      return (new UdtAgent());
   }
} class_udt;


UdtAgent::UdtAgent(): Agent(PT_UDT),
syn_timer_(this),
ack_timer_(this),
nak_timer_(this),
exp_timer_(this),
snd_timer_(this),
syn_interval_(0.01),
mtu_(1500),
max_flow_window_(100000)
{
   bind("mtu_", &mtu_);
   bind("max_flow_window_", &max_flow_window_);

   snd_loss_list_ = new SndLossList(max_flow_window_, 1 << 29, 1 << 30);
   rcv_loss_list_ = new RcvLossList(max_flow_window_, 1 << 29, 1 << 30);

   flow_window_size_ = 2;
   snd_interval_ = 0.000001;

   ack_interval_ = syn_interval_;
   nak_interval_ = syn_interval_;
   exp_interval_ = 1.01;
   
   nak_count_ = 0;
   dec_count_ = 0;
   snd_last_ack_ = 0;
   local_send_ = 0;
   local_loss_ = 0;
   local_ack_ = 0;
   snd_curr_seqno_ = -1;
   curr_max_seqno_ = 0;
   avg_nak_num_ = 2;
   dec_random_ = 2;

   loss_rate_limit_ = 0.01;
   loss_rate_ = 0;

   rtt_ = 1;
   
   rcv_interval_ = snd_interval_;
   rcv_last_ack_ = 0;
   rcv_last_ack_time_ = Scheduler::instance().clock();
   rcv_last_ack2_ = 0;
   ack_seqno_ = -1;
   rcv_curr_seqno_ = -1;
   local_recv_ = 0;
   last_dec_seq_ = -1;
   last_delay_time_ = Scheduler::instance().clock();
   last_dec_int_ = 1.0;

   slow_start_ = true;
   freeze_ = false;

   ack_timer_.resched(ack_interval_);
   nak_timer_.resched(nak_interval_);
}

UdtAgent::~UdtAgent()
{
}

int UdtAgent::command(int argc, const char*const* argv)
{
   return Agent::command(argc, argv);
}

void UdtAgent::recv(Packet *pkt, Handler*)
{
   hdr_udt* udth = hdr_udt::access(pkt);

   double r;

   if (1 == udth->flag())
   {
      switch (udth->type())
      {
      case 2:
         sendCtrl(6, udth->ackseq());

         if (udth->ack() > snd_last_ack_)
         {
            snd_last_ack_ = udth->ack();
            snd_loss_list_->remove((int)snd_last_ack_);
         }
         else
            break;

         snd_timer_.resched(0);

         if (rtt_ == syn_interval_)
            rtt_ = udth->rtt() / 1000000.0;
         else
            rtt_ = rtt_ * 0.875 + udth->rtt() / 1000000.0 * 0.125;

         if (slow_start_)
            flow_window_size_ = snd_last_ack_;
         else if (udth->lrecv() > 0)
            flow_window_size_ = int(ceil(flow_window_size_ * 0.875 + udth->lrecv() * (rtt_ + syn_interval_) * 0.125));

         if (flow_window_size_ > max_flow_window_)
         {
            slow_start_ = false;

            flow_window_size_ = max_flow_window_;
         }

         bandwidth_ = int(bandwidth_ * 0.875 + udth->bandwidth() * 0.125);

         exp_timer_.resched(exp_interval_);

         rateControl();

         if (snd_interval_ > rtt_)
         {
            snd_interval_ = rtt_;
            snd_timer_.resched(0);
         }

         break;

      case 3:
         slow_start_ = false;

         last_dec_int_ = snd_interval_;

         if ((udth->loss()[0] & 0x7FFFFFFF) > last_dec_seq_)
         {
            freeze_ = true;
            snd_interval_ = snd_interval_ * 1.125;

            avg_nak_num_ = 1 + int(ceil(double(avg_nak_num_) * 0.875 + double(nak_count_) * 0.125));

            dec_random_ = int(rand() * double(avg_nak_num_) / (RAND_MAX + 1.0)) + int(ceil(avg_nak_num_/5.0));

            nak_count_ = 1;

            last_dec_seq_ = snd_curr_seqno_;
         }
         else if (0 == (++ nak_count_ % dec_random_))
         {
            snd_interval_ = snd_interval_ * 1.125;

            last_dec_seq_ = snd_curr_seqno_;
         }

         if (snd_interval_ > rtt_)
            snd_interval_ = rtt_;

         local_loss_ ++;

         for (int i = 0, n = udth->losslen(); i < n; ++ i)
         {
            if ((udth->loss()[i] & 0x80000000) && ((udth->loss()[i] & 0x7FFFFFFF) >= snd_last_ack_))
            {
               snd_loss_list_->insert(udth->loss()[i] & 0x7FFFFFFF, udth->loss()[i + 1]);
               ++ i;
            }
            else if (udth->loss()[i] >= snd_last_ack_)
            {

               snd_loss_list_->insert(udth->loss()[i], udth->loss()[i]);
            }
         }

         exp_timer_.resched(exp_interval_);

         snd_timer_.resched(0);

         break;
     
      case 4:
/*
         if (slow_start_)
            slow_start_ = false;

         last_dec_int_ = snd_interval_;

         snd_interval_ = snd_interval_ * 1.125;

         last_dec_seq_ = snd_curr_seqno_;
         nak_count_ = -16;
         dec_count_ = 1;
*/
         break;

      case 6:
      {
         int ack;
         double rtt = ack_window_.acknowledge(udth->ackseq(), ack);

         if (rtt > 0)
         {
            time_window_.ack2arrival(rtt);

//            if ((time_window_.getdelaytrend()) && (Scheduler::instance().clock() - last_delay_time_ > 2 * rtt_))
//               sendCtrl(4);

            if (rtt_ == syn_interval_)
               rtt_ = rtt;
            else
               rtt_ = rtt_ * 0.875 + rtt * 0.125;

            nak_interval_ = rtt_;
            if (nak_interval_ < syn_interval_)
               nak_interval_ = syn_interval_;

            if (rcv_last_ack2_ < ack)
               rcv_last_ack2_ = ack;
         }

         break;
      }

      default:
         break;
      }

      Packet::free(pkt);
      return;
   }
   

   time_window_.pktarrival();

   if (0 == udth->seqno() % 16)
      time_window_.probe1arrival();
   else if (1 == udth->seqno() % 16)
      time_window_.probe2arrival();

   local_recv_ ++;

   int offset = udth->seqno() - rcv_last_ack_;
 
   if (offset < 0)
   {
      Packet::free(pkt);
      return;
   }

   if (udth->seqno() > rcv_curr_seqno_ + 1)
   {
      int c;

      if (rcv_curr_seqno_ + 1 == udth->seqno() - 1)
         c = 1;
      else
         c = 2;

      int* loss = new int[c];

      if (c == 2)
      {
         loss[0] = (rcv_curr_seqno_ + 1) | 0x80000000;
         loss[1] = udth->seqno() - 1;
      }
      else
         loss[0] = rcv_curr_seqno_ + 1;

      sendCtrl(3, c, loss);

      delete [] loss;
   }

   if (udth->seqno() > rcv_curr_seqno_)
   {
      rcv_curr_seqno_ = udth->seqno();
   }
   else
   {
      rcv_loss_list_->remove(udth->seqno());
   }

   Packet::free(pkt);
   return;
}

void UdtAgent::sendmsg(int nbytes, const char* /*flags*/)
{
   if (curr_max_seqno_ == snd_curr_seqno_ + 1)
      exp_timer_.resched(exp_interval_);

   curr_max_seqno_ += nbytes/1468;

   snd_timer_.resched(0);
}

void UdtAgent::sendCtrl(int pkttype, int lparam, int* rparam)
{
   Packet* p;
   hdr_udt* udth;
   hdr_cmn* ch;

   int ack;

   switch (pkttype)
   {
   case 2:
      if (rcv_loss_list_->getLossLength() == 0)
         ack = rcv_curr_seqno_ + 1;
      else
         ack = rcv_loss_list_->getFirstLostSeq();

      if (ack > rcv_last_ack_)
      {
         rcv_last_ack_ = ack;
      }
      else if (Scheduler::instance().clock() - rcv_last_ack_time_ <= 2 * rtt_)
      {
         ack_timer_.resched(ack_interval_);

         break;
      }

      if (rcv_last_ack_ > rcv_last_ack2_)
      {
         p = allocpkt(40);
         udth = hdr_udt::access(p);

         udth->flag() = 1;
         udth->type() = 2;
         udth->lrecv() = time_window_.getpktspeed();
         udth->bandwidth() = time_window_.getbandwidth();
         udth->rtt() = int(rtt_ * 1000000.0);

         ack_seqno_ ++;
         udth->ackseq() = ack_seqno_;
         udth->ack() = rcv_last_ack_;

         ch = hdr_cmn::access(p);
         ch->size() = 40;
         Agent::send(p, 0);

         ack_window_.store(ack_seqno_, rcv_last_ack_);

         rcv_last_ack_time_ = Scheduler::instance().clock();
      }

      ack_timer_.resched(ack_interval_);

      break;

   case 3:
      if (rparam != NULL)
      {
         p = allocpkt(32 + lparam * 4);
         udth = hdr_udt::access(p);

         udth->flag() = 1;
         udth->type() = 3;
         udth->losslen() = lparam;
         memcpy(udth->loss(), rparam, lparam * 4);

         ch = hdr_cmn::access(p);
         ch->size() = 32 + lparam * 4;
         Agent::send(p, 0);
      }
      else if (rcv_loss_list_->getLossLength() > 0)
      {
         int losslen;
         int* loss = new int[MAX_LOSS_LEN];
         rcv_loss_list_->getLossArray(loss, &losslen, MAX_LOSS_LEN, rtt_);
 
         if (losslen > 0)
         {
            p = allocpkt(32 + losslen);
            udth = hdr_udt::access(p);

            udth->flag() = 1;
            udth->type() = 3;
            udth->losslen() = losslen;
            memcpy(udth->loss(), loss, MAX_LOSS_LEN);

            ch = hdr_cmn::access(p);
            ch->size() = 32 + losslen;
            Agent::send(p, 0);
         }

         delete [] loss;
      }

      nak_timer_.resched(nak_interval_);

      break;

   case 4:
      p = allocpkt(32);
      udth = hdr_udt::access(p);

      udth->flag() = 1;
      udth->type() = 4;

      ch = hdr_cmn::access(p);
      ch->size() = 32;
      Agent::send(p, 0);

      last_delay_time_ = Scheduler::instance().clock();

      break;

   case 6:
      p = allocpkt(32);
      udth = hdr_udt::access(p);

      udth->flag() = 1;
      udth->type() = 6;
      udth->ackseq() = lparam;

      ch = hdr_cmn::access(p);
      ch->size() = 32;
      Agent::send(p, 0);

      break;
   }
}

void UdtAgent::sendData()
{
   bool probe = false;

   if (snd_last_ack_ == curr_max_seqno_)
      snd_timer_.resched(snd_interval_);

   int nextseqno;

   if (snd_loss_list_->getLossLength() > 0)
   {
      nextseqno = snd_loss_list_->getLostSeq();
   }
   else if (snd_curr_seqno_ - snd_last_ack_ < flow_window_size_)
   {
      nextseqno = ++ snd_curr_seqno_;
      if (0 == nextseqno % 16)
         probe = true;
   }
   else
   {
/*
      if (freeze_)
      {
         snd_timer_.resched(syn_interval_ + snd_interval_);
         freeze_ = false;
      }
      else
         snd_timer_.resched(snd_interval_);
*/
      return;
   }

   Packet* p;

   p = allocpkt(mtu_);
   hdr_udt* udth = hdr_udt::access(p);
   udth->flag() = 0;
   udth->seqno() = nextseqno;

   hdr_cmn* ch = hdr_cmn::access(p);
   ch->size() = mtu_;
   Agent::send(p, 0);

   local_send_ ++;

   if (probe)
   {
      snd_timer_.resched(0);
      return;
   }

   if (freeze_)
   {
      snd_timer_.resched(syn_interval_ + snd_interval_);
      freeze_ = false;
   }
   else
      snd_timer_.resched(snd_interval_);
}

void UdtAgent::rateControl()
{
   if (slow_start_)
      return;

   double inc = 0.0;

   if (bandwidth_ < 1.0 / snd_interval_)
      inc = 1.0/mtu_;
   else
   {
      inc = pow(10, ceil(log10((bandwidth_ - 1.0 / snd_interval_) * mtu_ * 8))) * 0.0000015 / mtu_;

      if (inc < 1.0/mtu_)
         inc = 1.0/mtu_;
   }

   snd_interval_ = (snd_interval_ * syn_interval_) / (snd_interval_ * inc + syn_interval_);

   if (snd_interval_ < 0.000001)
      snd_interval_ = 0.000001;
}

void UdtAgent::timeOut()
{
   if (snd_curr_seqno_ >= snd_last_ack_)
   {
      snd_loss_list_->insert(int(snd_last_ack_), int(snd_curr_seqno_));
   }

   exp_interval_ = 1.0; //rtt_ + syn_interval_;

   exp_timer_.resched(exp_interval_);

   snd_timer_.resched(0);
}

/////////////////////////////////////////////////////////////////
void SndTimer::expire(Event*)
{
   a_->sendData();
}

void SynTimer::expire(Event*)
{
   a_->rateControl();
}

void AckTimer::expire(Event*)
{
   a_->sendCtrl(2);
}

void NakTimer::expire(Event*)
{
   a_->sendCtrl(3);
}

void ExpTimer::expire(Event*)
{
   a_->timeOut();
}

////////////////////////////////////////////////////////////////////

// Definition of >, <, >=, and <= with sequence number wrap

inline const bool LossList::greaterthan(const int& seqno1, const int& seqno2) const
{
   if ((seqno1 > seqno2) && (seqno1 - seqno2 < seq_no_th_))
      return true;
 
   if (seqno1 < seqno2 - seq_no_th_)
      return true;

   return false;
}

inline const bool LossList::lessthan(const int& seqno1, const int& seqno2) const
{
   return greaterthan(seqno2, seqno1);
}

inline const bool LossList::notlessthan(const int& seqno1, const int& seqno2) const
{
   if (seqno1 == seqno2)
      return true;

   return greaterthan(seqno1, seqno2);
}

inline const bool LossList::notgreaterthan(const int& seqno1, const int& seqno2) const
{
   if (seqno1 == seqno2)
      return true;

   return lessthan(seqno1, seqno2);
}

// return the distance between two sequence numbers, parameters are pre-checked
inline const int LossList::getLength(const int& seqno1, const int& seqno2) const
{
   if (seqno2 >= seqno1)
      return seqno2 - seqno1 + 1;
   else if (seqno2 < seqno1 - seq_no_th_)
      return seqno2 - seqno1 + max_seq_no_ + 1;
   else
      return 0;
}

//Definition of ++, and -- with sequence number wrap

inline const int LossList::incSeqNo(const int& seqno) const
{
   return (seqno + 1) % max_seq_no_;
}

inline const int LossList::decSeqNo(const int& seqno) const
{
   return (seqno - 1 + max_seq_no_) % max_seq_no_;
}


SndLossList::SndLossList(const int& size, const int& th, const int& max):
size_(size)
{
   seq_no_th_ = th;
   max_seq_no_ = max;

   data1_ = new int [size_];
   data2_ = new int [size_];
   next_ = new int [size_];

   // -1 means there is no data in the node
   for (int i = 0; i < size; ++ i)
   {
      data1_[i] = -1;
      data2_[i] = -1;
   }

   length_ = 0;
   head_ = -1;
   last_insert_pos_ = -1;
}

SndLossList::~SndLossList()
{
   delete [] data1_;
   delete [] data2_;
   delete [] next_;
}

int SndLossList::insert(const int& seqno1, const int& seqno2)
{
   if (0 == length_)
   {
      // insert data into an empty list

      head_ = 0;
      data1_[head_] = seqno1;
      if (seqno2 != seqno1)
         data2_[head_] = seqno2;

      next_[head_] = -1;
      last_insert_pos_ = head_;

      length_ += getLength(seqno1, seqno2);

      return length_;
   }

   // otherwise find the position where the data can be inserted
   int origlen = length_;

   int offset = seqno1 - data1_[head_];

   if (offset < -seq_no_th_)
      offset += max_seq_no_;
   else if (offset > seq_no_th_)
      offset -= max_seq_no_;

   int loc = (head_ + offset + size_) % size_;

   if (offset < 0)
   {
      // Insert data prior to the head pointer

      data1_[loc] = seqno1;
      if (seqno2 != seqno1)
         data2_[loc] = seqno2;

      // new node becomes head
      next_[loc] = head_;
      head_ = loc;
      last_insert_pos_ = loc;

      length_ += getLength(seqno1, seqno2);
   }
   else if (offset > 0)
   {
      if (seqno1 == data1_[loc])
      {
         last_insert_pos_ = loc;

         // first seqno is equivlent, compare the second
         if (-1 == data2_[loc])
         {
            if (seqno2 != seqno1)
            {
               length_ += getLength(seqno1, seqno2) - 1;
               data2_[loc] = seqno2;
            }
         }
         else if (greaterthan(seqno2, data2_[loc]))
         {
            // new seq pair is longer than old pair, e.g., insert [3, 7] to [3, 5], becomes [3, 7]
            length_ += getLength(data2_[loc], seqno2) - 1;
            data2_[loc] = seqno2;
         }
         else
            // Do nothing if it is already there
            return 0;
      }
      else
      {
         // searching the prior node
         int i;
         if ((-1 != last_insert_pos_) && lessthan(data1_[last_insert_pos_], seqno1))
            i = last_insert_pos_;
         else
            i = head_;

         while ((-1 != next_[i]) && lessthan(data1_[next_[i]], seqno1))
            i = next_[i];

         if ((-1 == data2_[i]) || lessthan(data2_[i], seqno1))
         {
            last_insert_pos_ = loc;

            // no overlap, create new node
            data1_[loc] = seqno1;
            if (seqno2 != seqno1)
               data2_[loc] = seqno2;

            next_[loc] = next_[i];
            next_[i] = loc;

            length_ += getLength(seqno1, seqno2);
         }
         else
         {
            last_insert_pos_ = i;

            // overlap, coalesce with prior node, insert(3, 7) to [2, 5], ... becomes [2, 7]
            if (lessthan(data2_[i], seqno2))
            {
               length_ += getLength(data2_[i], seqno2) - 1;
               data2_[i] = seqno2;

               loc = i;
            }
            else
               return 0;
         }
      }
   }
   else
   {
      last_insert_pos_ = head_;

      // insert to head node
      if (seqno2 != seqno1)
      {
         if (-1 == data2_[loc])
         {
            length_ += getLength(seqno1, seqno2) - 1;
            data2_[loc] = seqno2;
         }
         else if (greaterthan(seqno2, data2_[loc]))
         {
            length_ += getLength(data2_[loc], seqno2) - 1;
            data2_[loc] = seqno2;
         }
         else 
            return 0;
      }
      else
         return 0;
   }

   // coalesce with next node. E.g., [3, 7], ..., [6, 9] becomes [3, 9] 
   while ((-1 != next_[loc]) && (-1 != data2_[loc]))
   {
      int i = next_[loc];

      if (notgreaterthan(data1_[i], incSeqNo(data2_[loc])))
      {
         // coalesce if there is overlap
         if (-1 != data2_[i])
         {
            if (greaterthan(data2_[i], data2_[loc]))
            {
               if (notlessthan(data2_[loc], data1_[i]))
                  length_ -= getLength(data1_[i], data2_[loc]);

               data2_[loc] = data2_[i];
            }
            else
               length_ -= getLength(data1_[i], data2_[i]);
         }
         else
         {
            if (data1_[i] == incSeqNo(data2_[loc]))
               data2_[loc] = data1_[i];
            else
               length_ --;
         }

         data1_[i] = -1;
         data2_[i] = -1;
         next_[loc] = next_[i];
      }
      else
         break;
   }

   return length_ - origlen;
}

void SndLossList::remove(const int& seqno)
{
   if (0 == length_)
      return;

   // Remove all from the head pointer to a node with a larger seq. no. or the list is empty

   int offset = seqno - data1_[head_];

   if (offset < -seq_no_th_)
      offset += max_seq_no_;
   else if (offset > seq_no_th_)
      offset -= max_seq_no_;

   int loc = (head_ + offset + size_) % size_;

   if (0 == offset)
   {
      // It is the head. Remove the head and point to the next node
      loc = (loc + 1) % size_;

      if (-1 == data2_[head_])
         loc = next_[head_];
      else
      {
         data1_[loc] = incSeqNo(seqno);
         if (greaterthan(data2_[head_], incSeqNo(seqno)))
            data2_[loc] = data2_[head_];

         data2_[head_] = -1;

         next_[loc] = next_[head_];
      }

      data1_[head_] = -1;

      if (last_insert_pos_ == head_)
         last_insert_pos_ = -1;

      head_ = loc;

      length_ --;
   }
   else if (offset > 0)
   {
      int h = head_;

      if (seqno == data1_[loc])
      {
         // target node is not empty, remove part/all of the seqno in the node.
         int temp = loc;
         loc = (loc + 1) % size_;         

         if (-1 == data2_[temp])
            head_ = next_[temp];
         else
         {
            // remove part, e.g., [3, 7] becomes [], [4, 7] after remove(3)
            data1_[loc] = incSeqNo(seqno);
            if (greaterthan(data2_[temp], incSeqNo(seqno)))
               data2_[loc] = data2_[temp];
            head_ = loc;
            next_[loc] = next_[temp];
            next_[temp] = loc;
            data2_[temp] = -1;
         }
      }
      else
      {
         // targe node is empty, check prior node
         int i = head_;
         while ((-1 != next_[i]) && lessthan(data1_[next_[i]], seqno))
            i = next_[i];

         loc = (loc + 1) % size_;

         if (-1 == data2_[i])
            head_ = next_[i];
         else if (greaterthan(data2_[i], seqno))
         {
            // remove part seqno in the prior node
            data1_[loc] = incSeqNo(seqno);
            if (greaterthan(data2_[i], incSeqNo(seqno)))
               data2_[loc] = data2_[i];

            data2_[i] = seqno;

            next_[loc] = next_[i];
            next_[i] = loc;

            head_ = loc;
         }
         else
            head_ = next_[i];
      }

      // Remove all nodes prior to the new head
      while (h != head_)
      {
         if (data2_[h] != -1)
         {
            length_ -= getLength(data1_[h], data2_[h]);
            data2_[h] = -1;
         }
         else
            length_ --;

         data1_[h] = -1;

         if (last_insert_pos_ == h)
            last_insert_pos_ = -1;

         h = next_[h];
      }
   }
}

int SndLossList::getLossLength()
{
   return length_;
}

int SndLossList::getLostSeq()
{
   if (0 == length_)
     return -1;

   if (last_insert_pos_ == head_)
      last_insert_pos_ = -1;

   // return the first loss seq. no.
   int seqno = data1_[head_];

   // head moves to the next node
   if (-1 == data2_[head_])
   {
      //[3, -1] becomes [], and head moves to next node in the list
      data1_[head_] = -1;
      head_ = next_[head_];
   }
   else
   {
      // shift to next node, e.g., [3, 7] becomes [], [4, 7]
      int loc = (head_ + 1) % size_;

      data1_[loc] = incSeqNo(seqno);
      if (greaterthan(data2_[head_], incSeqNo(seqno)))
         data2_[loc] = data2_[head_];

      data1_[head_] = -1;
      data2_[head_] = -1;

      next_[loc] = next_[head_];
      head_ = loc;
   }

   length_ --;

   return seqno;
}


//
RcvLossList::RcvLossList(const int& size, const int& th, const int& max):
size_(size)
{
   seq_no_th_ = th;
   max_seq_no_ = max;

   data1_ = new int [size_];
   data2_ = new int [size_];
   last_feedback_time_ = new double [size_];
   count_ = new int [size_];
   next_ = new int [size_];
   prior_ = new int [size_];

   // -1 means there is no data in the node
   for (int i = 0; i < size; ++ i)
   {
      data1_[i] = -1;
      data2_[i] = -1;
   }

   length_ = 0;
   head_ = -1;
   tail_ = -1;
}

RcvLossList::~RcvLossList()
{
   delete [] data1_;
   delete [] data2_;
   delete [] last_feedback_time_;
   delete [] count_;
   delete [] next_;
   delete [] prior_;
}

void RcvLossList::insert(const int& seqno1, const int& seqno2)
{
   // Data to be inserted must be larger than all those in the list
   // guaranteed by the UDT receiver

   if (0 == length_)
   {
      // insert data into an empty list
      head_ = 0;
      tail_ = 0;
      data1_[head_] = seqno1;
      if (seqno2 != seqno1)
         data2_[head_] = seqno2;

      last_feedback_time_[head_] = Scheduler::instance().clock();
      count_[head_] = 2;

      next_[head_] = -1;
      prior_[head_] = -1;
      length_ += getLength(seqno1, seqno2);

      return;
   }

   // otherwise searching for the position where the node should be

   int offset = seqno1 - data1_[head_];

   if (offset < -seq_no_th_)
      offset += max_seq_no_;

   int loc = (head_ + offset) % size_;

   if ((-1 != data2_[tail_]) && (incSeqNo(data2_[tail_]) == seqno1))
   {
      // coalesce with prior node, e.g., [2, 5], [6, 7] becomes [2, 7]
      loc = tail_;
      data2_[loc] = seqno2;
   }
   else
   {
      // create new node
      data1_[loc] = seqno1;

      if (seqno2 != seqno1)
         data2_[loc] = seqno2;

      next_[tail_] = loc;
      prior_[loc] = tail_;
      next_[loc] = -1;
      tail_ = loc;
   }

   // Initilize time stamp
   last_feedback_time_[loc] = Scheduler::instance().clock();
   count_[loc] = 2;

   length_ += getLength(seqno1, seqno2);
}

bool RcvLossList::remove(const int& seqno)
{
   if (0 == length_)
      return false; 

   // locate the position of "seqno" in the list
   int offset = seqno - data1_[head_];

   if (offset < -seq_no_th_)
      offset += max_seq_no_;

   if (offset < 0)
      return false;

   int loc = (head_ + offset) % size_;

   if (seqno == data1_[loc])
   {
      // This is a seq. no. that starts the loss sequence

      if (-1 == data2_[loc])
      {
         // there is only 1 loss in the sequence, delete it from the node
         if (head_ == loc)
         {
            head_ = next_[head_];
            if (-1 != head_)
               prior_[head_] = -1;
         }
         else
         {
            next_[prior_[loc]] = next_[loc];
            if (-1 != next_[loc])
               prior_[next_[loc]] = prior_[loc];
            else
               tail_ = prior_[loc];
         }

         data1_[loc] = -1;
      }
      else
      {
         // there are more than 1 loss in the sequence
         // move the node to the next and update the starter as the next loss inSeqNo(seqno)

         // find next node
         int i = (loc + 1) % size_;

         // remove the "seqno" and change the starter as next seq. no.
         data1_[i] = incSeqNo(data1_[loc]);

         // process the sequence end
         if (greaterthan(data2_[loc], incSeqNo(data1_[loc])))
            data2_[i] = data2_[loc];

         // replicate the time stamp and report counter
         last_feedback_time_[i] = last_feedback_time_[loc];
         count_[i] = count_[loc];

         // remove the current node
         data1_[loc] = -1;
         data2_[loc] = -1;
 
         // update list pointer
         next_[i] = next_[loc];
         prior_[i] = prior_[loc];

         if (head_ == loc)
            head_ = i;
         else
            next_[prior_[i]] = i;

         if (tail_ == loc)
            tail_ = i;
         else
            prior_[next_[i]] = i;
      }

      length_ --;

      return true;
   }

   // There is no loss sequence in the current position
   // the "seqno" may be contained in a previous node

   // searching previous node
   int i = (loc - 1 + size_) % size_;
   while (-1 == data1_[i])
      i = (i - 1 + size_) % size_;

   // not contained in this node, return
   if ((-1 == data2_[i]) || greaterthan(seqno, data2_[i]))
       return false;

   if (seqno == data2_[i])
   {
      // it is the sequence end

      if (seqno == incSeqNo(data1_[i]))
         data2_[i] = -1;
      else
         data2_[i] = decSeqNo(seqno);
   }
   else
   {
      // split the sequence

      // construct the second sequence from incSeqNo(seqno) to the original sequence end
      // located at "loc + 1"
      loc = (loc + 1) % size_;

      data1_[loc] = incSeqNo(seqno);
      if (greaterthan(data2_[i], incSeqNo(seqno)))
         data2_[loc] = data2_[i];

      // the first (original) sequence is between the original sequence start to decSeqNo(seqno)
      if (seqno == incSeqNo(data1_[i]))
         data2_[i] = -1;
      else
         data2_[i] = decSeqNo(seqno);

      // replicate the time stamp and report counter
      last_feedback_time_[loc] = last_feedback_time_[i];
      count_[loc] = count_[i];

      // update the list pointer
      next_[loc] = next_[i];
      next_[i] = loc;
      prior_[loc] = i;

      if (tail_ == i)
         tail_ = loc;
      else
         prior_[next_[loc]] = loc;
   }

   length_ --;

   return true;
}

int RcvLossList::getLossLength() const
{
   return length_;
}

int RcvLossList::getFirstLostSeq() const
{
   if (0 == length_)
      return -1;

   return data1_[head_];
}

void RcvLossList::getLossArray(int* array, int* len, const int& limit, const double& threshold)
{
   double currtime = Scheduler::instance().clock();

   int i  = head_;

   len = 0;

   while ((*len < limit - 1) && (-1 != i))
   {
      if (currtime - last_feedback_time_[i] > count_[i] * threshold)
      {
         array[*len] = data1_[i];
         if (-1 != data2_[i])
         {
            // there are more than 1 loss in the sequence
            array[*len] |= 0x80000000;
            ++ *len;
            array[*len] = data2_[i];
         }

         ++ *len;

         // update the timestamp
         last_feedback_time_[i] = Scheduler::instance().clock();
         // update how many times this loss has been fed back, the "k" in UDT paper
         ++ count_[i];
      }

      i = next_[i];
   }
}





////////////////////////////////////////////////////////////////////////////
//
AckWindow::AckWindow():
size_(1024),
head_(0),
tail_(0)
{
   ack_seqno_ = new int[size_];
   ack_ = new int[size_];
   ts_ = new double[size_];

   ack_seqno_[0] = -1;
}

AckWindow::~AckWindow()
{
   delete [] ack_seqno_;
   delete [] ack_;
   delete [] ts_;
}

void AckWindow::store(const int& seq, const int& ack)
{
   head_ = (head_ + 1) % size_;
   ack_seqno_[head_] = seq;
   ack_[head_] = ack;
   *(ts_ + head_) = Scheduler::instance().clock();

   // overwrite the oldest ACK since it is not likely to be acknowledged
   if (head_ == tail_)
      tail_ = (tail_ + 1) % size_;
}

double AckWindow::acknowledge(const int& seq, int& ack)
{
   if (head_ >= tail_)
   {
      // Head has not exceeded the physical boundary of the window
      for (int i = tail_; i <= head_; i ++)
         // looking for indentical ACK Seq. No.
         if (seq == ack_seqno_[i])
         {
            // return the Data ACK it carried
            ack = ack_[i];

            // calculate RTT
            double rtt = Scheduler::instance().clock() - ts_[i];
            if (i == head_)
               tail_ = head_ = 0;
            else
               tail_ = (i + 1) % size_;

            return rtt;
         }

      // Bad input, the ACK node has been overwritten
      return -1;
   }

   // head has exceeded the physical window boundary, so it is behind to tail
   for (int i = tail_; i <= head_ + size_; i ++)
      // looking for indentical ACK seq. no.
      if (seq == ack_seqno_[i % size_])
      {
         // return Data ACK
         i %= size_;
         ack = ack_[i];

         // calculate RTT
         double currtime = Scheduler::instance().clock();
         double rtt = currtime - ts_[i];
         if (i == head_)
            tail_ = head_ = 0;
         else
            tail_ = (i + 1) % size_;

         return rtt;
      }

   // bad input, the ACK node has been overwritten
   return -1;
}

//
TimeWindow::TimeWindow():
size_(16)
{
   pkt_window_ = new double[size_];
   rtt_window_ = new double[size_];
   pct_window_ = new double[size_];
   pdt_window_ = new double[size_];
   probe_window_ = new double[size_];

   pkt_window_ptr_ = 0;
   rtt_window_ptr_ = 0;
   probe_window_ptr_ = 0;

   first_round_ = true;

   for (int i = 0; i < size_; ++ i)
   {
      pkt_window_[i] = 1.0;
      rtt_window_[i] = pct_window_[i] = pdt_window_[i] = 0.0;
      probe_window_[i] = 1000.0;
   }

   last_arr_time_ = Scheduler::instance().clock();
}

TimeWindow::~TimeWindow()
{
   delete [] pkt_window_;
   delete [] rtt_window_;
   delete [] pct_window_;
   delete [] pdt_window_;
}

int TimeWindow::getbandwidth() const
{
   double temp;
   for (int i = 0; i < ((size_ >> 1) + 1); ++ i)
      for (int j = i; j < size_; ++ j)
         if (probe_window_[i] > probe_window_[j])
         {
            temp = probe_window_[i];
            probe_window_[i] = probe_window_[j];
            probe_window_[j] = temp;
         }

   if (0 == probe_window_[size_ >> 1])
      return 0;

   return int(ceil(1.0 / probe_window_[size_ >> 1]));
}

int TimeWindow::getpktspeed() const
{
   if ((first_round_) && (pkt_window_ptr_ > 0))
   {
      if ((pkt_window_ptr_ > 1) && (pkt_window_[pkt_window_ptr_ - 1] < 2 * pkt_window_[pkt_window_ptr_ - 2]))
         return (int)ceil(1.0 / pkt_window_[pkt_window_ptr_ - 1]);

      return 0;
   }

   double temp;
   for (int i = 0; i < ((size_ >> 1) + 1); ++ i)
      for (int j = i; j < size_; ++ j)
         if (pkt_window_[i] > pkt_window_[j])
         {
            temp = pkt_window_[i];
            pkt_window_[i] = pkt_window_[j];
            pkt_window_[j] = temp;
         }

   double median = pkt_window_[size_ >> 1];
   int count = 0;
   double sum = 0.0;

   for (int i = 0; i < size_; ++ i)
      if ((pkt_window_[i] < (median * 2)) && (pkt_window_[i] > (median / 2)))
      {
         ++ count;
         sum += pkt_window_[i];
      }

   if (count > (size_ >> 1))
      return (int)ceil(1.0 / (sum / count));
   else
      return 0;
}

bool TimeWindow::getdelaytrend() const
{
   double pct = 0.0;
   double pdt = 0.0;

   for (int i = 0; i < size_; ++i)
      if (i != rtt_window_ptr_)
      {
         pct += pct_window_[i];
         pdt += pdt_window_[i];
      }

   pct /= size_ - 1.0;
   if (0.0 != pdt)
      pdt = (rtt_window_[(rtt_window_ptr_ - 1 + size_) % size_] - rtt_window_[rtt_window_ptr_]) / pdt;

   return ((pct > 0.66) && (pdt > 0.45)) || ((pct > 0.54) && (pdt > 0.55));
}

void TimeWindow::pktarrival()
{
   curr_arr_time_ = Scheduler::instance().clock();

   pkt_window_[pkt_window_ptr_] = curr_arr_time_ - last_arr_time_;

   pkt_window_ptr_ = (pkt_window_ptr_ + 1) % size_;
 
   if (0 == pkt_window_ptr_)
      first_round_ = false;

   last_arr_time_ = curr_arr_time_;
}

void TimeWindow::probe1arrival()
{
   probe_time_ = Scheduler::instance().clock();
}

void TimeWindow::probe2arrival()
{
   probe_window_[probe_window_ptr_] = Scheduler::instance().clock() - probe_time_;;

   probe_window_ptr_ = (probe_window_ptr_ + 1) % size_;

   last_arr_time_ = Scheduler::instance().clock();
}

void TimeWindow::ack2arrival(const double& rtt)
{
   rtt_window_[rtt_window_ptr_] = rtt;
   pct_window_[rtt_window_ptr_] = (rtt > rtt_window_[(rtt_window_ptr_ - 1 + size_) % size_]) ? 1 : 0;
   pdt_window_[rtt_window_ptr_] = fabs(rtt - rtt_window_[(rtt_window_ptr_ - 1 + size_) % size_]);

   rtt_window_ptr_ = (rtt_window_ptr_ + 1) % size_;
}
