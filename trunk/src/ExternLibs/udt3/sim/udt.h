//
// Author: Yunhong Gu, gu@lac.uic.edu
//
// Descrition: the program is used to simulate UDT on NS-2
//
// Last Update: 03/20/2006
// 


#ifndef __NS_UDT_H__
#define __NS_UDT_H__

#include "agent.h"
#include "packet.h"

const int MAX_LOSS_LEN = 300;


struct hdr_udt 
{
   int flag_;
   int seqno_;
   int type_;
   int losslen_;
   int ackseq_;
   int ack_;
   int recv_;
   int rtt_;
   int bandwidth_;
   int loss_[MAX_LOSS_LEN];

   static int off_udt_;
   inline static int& offset() { return off_udt_; }
   inline static hdr_udt* access(Packet* p) {return (hdr_udt*) p->access(off_udt_);}

   int& flag() {return flag_;}
   int& seqno() {return seqno_;}
   int& type() {return type_;}
   int& losslen() {return losslen_;}
   int& ackseq() {return ackseq_;}
   int& ack() {return ack_;}
   int& lrecv() {return recv_;}
   int& rtt() {return rtt_;}
   int& bandwidth() {return bandwidth_;};
   int* loss() {return loss_;}
};


class UdtAgent;

class SndTimer: public TimerHandler 
{
public:
   SndTimer(UdtAgent *a) : TimerHandler() { a_ = a; }

protected:
   virtual void expire(Event *e);
   UdtAgent *a_;
};

class SynTimer: public TimerHandler
{
public:
   SynTimer(UdtAgent *a) : TimerHandler() { a_ = a; }

protected:
   virtual void expire(Event *e);
   UdtAgent *a_;
};

class AckTimer: public TimerHandler
{
public:
   AckTimer(UdtAgent *a) : TimerHandler() { a_ = a; }

protected:
   virtual void expire(Event *e);
   UdtAgent *a_;
};

class NakTimer: public TimerHandler
{
public:
   NakTimer(UdtAgent *a) : TimerHandler() { a_ = a; }

protected:
   virtual void expire(Event *e);
   UdtAgent *a_;
};

class ExpTimer: public TimerHandler
{
public:
   ExpTimer(UdtAgent *a) : TimerHandler() { a_ = a; }

protected:
   virtual void expire(Event *e);
   UdtAgent *a_;
};



class LossList
{
protected:
   const bool greaterthan(const int& seqno1, const int& seqno2) const;
   const bool lessthan(const int& seqno1, const int& seqno2) const;
   const bool notlessthan(const int& seqno1, const int& seqno2) const;
   const bool notgreaterthan(const int& seqno1, const int& seqno2) const;

   const int getLength(const int& seqno1, const int& seqno2) const;

   const int incSeqNo(const int& seqno) const;
   const int decSeqNo(const int& seqno) const;

protected:
   int seq_no_th_;                  // threshold for comparing seq. no.
   int max_seq_no_;                 // maximum permitted seq. no.
};

////////////////////////////////////////////////////////////////////////////////
class SndLossList: public LossList
{
public:
   SndLossList(const int& size, const int& th, const int& max);
   ~SndLossList();

   int insert(const int& seqno1, const int& seqno2);
   void remove(const int& seqno);
   int getLossLength();
   int getLostSeq();

private:
   int* data1_;			// sequence number starts
   int* data2_;			// seqnence number ends
   int* next_;			// next node in the list

   int head_;			// first node
   int length_;			// loss length
   int size_;			// size of the static array
   int last_insert_pos_;		// position of last insert node
};


////////////////////////////////////////////////////////////////////////////////
class RcvLossList: public LossList
{
public:
   RcvLossList(const int& size, const int& th, const int& max);
   ~RcvLossList();

   void insert(const int& seqno1, const int& seqno2);
   bool remove(const int& seqno);
   int getLossLength() const;
   int getFirstLostSeq() const;
   void getLossArray(int* array, int* len, const int& limit, const double& interval);

private:
   int* data1_;			// sequence number starts
   int* data2_;			// sequence number ends
   double* last_feedback_time_;		// last feedback time of the node
   int* count_;			// report counter
   int* next_;			// next node in the list
   int* prior_;			// prior node in the list;

   int head_;			// first node in the list
   int tail_;			// last node in the list;
   int length_;			// loss length
   int size_;			// size of the static array
};


class AckWindow
{
public:
   AckWindow();
   ~AckWindow();

   void store(const int& seq, const int& ack);
   double acknowledge(const int& seq, int& ack);

private:
   int* ack_seqno_;
   int* ack_;
   double* ts_;

   const int size_;

   int head_;
   int tail_;
};


class TimeWindow
{
public:
   TimeWindow();
   ~TimeWindow();

   int getbandwidth() const;
   int getpktspeed() const;
   bool getdelaytrend() const;

   void pktarrival();
   void ack2arrival(const double& rtt);

   void probe1arrival();
   void probe2arrival();

private:
   const int size_;

   double* pkt_window_;
   int pkt_window_ptr_;

   double* rtt_window_;
   double* pct_window_;
   double* pdt_window_;
   int rtt_window_ptr_;

   double* probe_window_;
   int probe_window_ptr_;

   double last_arr_time_;
   double probe_time_;
   double curr_arr_time_;

   bool first_round_;
};

class UdtAgent: public Agent
{
friend SndTimer;
friend SynTimer;
friend AckTimer;
friend NakTimer;
friend ExpTimer;

public:
   UdtAgent();
   ~UdtAgent();

   int command(int argc, const char*const* argv);

   virtual void recv(Packet*, Handler*);
   virtual void sendmsg(int nbytes, const char *flags = 0);

protected:
   SndTimer snd_timer_;
   SynTimer syn_timer_;
   AckTimer ack_timer_;
   NakTimer nak_timer_;
   ExpTimer exp_timer_;

   double syn_interval_;
   double ack_interval_;
   double nak_interval_;
   double exp_interval_;

   int mtu_;

   int max_flow_window_;
   int flow_window_size_;
   
   SndLossList* snd_loss_list_;
   RcvLossList* rcv_loss_list_;

   double snd_interval_;

   int bandwidth_;

   int nak_count_;
   int dec_count_;
   volatile int snd_last_ack_;
   int local_send_;
   int local_loss_;
   int local_ack_;
   volatile int snd_curr_seqno_;
   int curr_max_seqno_;
   int dec_random_;
   int avg_nak_num_;

   double loss_rate_limit_;
   double loss_rate_;

   AckWindow ack_window_;
   TimeWindow time_window_;

   double rtt_;

   double rcv_interval_;
   int rcv_last_ack_;
   double rcv_last_ack_time_;
   int rcv_last_ack2_;
   int ack_seqno_;
   volatile int rcv_curr_seqno_;
   int local_recv_;
   int last_dec_seq_;
   double last_delay_time_;
   double last_dec_int_;

   bool slow_start_;

   bool freeze_;

   bool firstloss_;

protected:
   void rateControl();
   void flowControl();
   void sendCtrl(int pkttype, int lparam = 0, int* rparam = NULL);
   void sendData();
   void timeOut();
};


#endif
