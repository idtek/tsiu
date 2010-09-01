/*****************************************************************************
DISCLAIMER: The algorithms implemented using UDT/CCC in this file may be
modified. These modifications may NOT necessarily reflect the view of
the algorithms' original authors.

Written by: Yunhong Gu <ygu@cs.uic.edu>, last updated on Mar 02, 2005.
*****************************************************************************/

#ifndef WIN32
   #include <sys/time.h>
   #include <time.h>
#endif

#include <cmath>
#include <vector>
#include <algorithm>

#include <window.h>
#include <ccc.h>
#include <udt.h>

using namespace std;


/*****************************************************************************
TCP congestion control
Reference: 
M. Allman, V. Paxson, W. Stevens (consultant), TCP Congestion Control, RFC 
2581, April 1999.

Note:
This base TCP control class can be used to derive new TCP variants, including
those implemented in this file: HighSpeed, Scalable, BiC, Vegas, and FAST.
*****************************************************************************/

class CTCP: public CCC
{
public:
   void init()
   {
      m_bSlowStart = true;
      m_issthresh = 83333;

      m_dPktSndPeriod = 0.0;
      m_dCWndSize = 2.0;

      setACKInterval(2);
      setRTO(1000000);
   }

   virtual void onACK(const int& ack)
   {
      if (ack == m_iLastACK)
      {
         if (3 == ++ m_iDupACKCount)
            DupACKAction();
         else if (m_iDupACKCount > 3)
            m_dCWndSize += 1.0;
         else
            ACKAction();
      }
      else
      {
         if (m_iDupACKCount >= 3)
            m_dCWndSize = m_issthresh;

         m_iLastACK = ack;
         m_iDupACKCount = 1;

         ACKAction();
      }
   }

   virtual void onTimeout()
   {
      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_bSlowStart = true;
      m_dCWndSize = 2.0;
   }

protected:
   virtual void ACKAction()
   {
      if (m_bSlowStart)
      {
         m_dCWndSize += 1.0;

         if (m_dCWndSize >= m_issthresh)
            m_bSlowStart = false;
      }
      else
         m_dCWndSize += 1.0/m_dCWndSize;
   }

   virtual void DupACKAction()
   {
      m_bSlowStart = false;

      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_dCWndSize = m_issthresh + 3;
   }

protected:
   int m_issthresh;
   bool m_bSlowStart;

   int m_iDupACKCount;
   int m_iLastACK;
};


/*****************************************************************************
Scalable TCP congestion control
Reference:
Tom Kelly, Scalable TCP: Improving Performance in Highspeed Wide Area 
Networks, Computer Communication Review, Vol. 33 No. 2 - April 2003
*****************************************************************************/

class CScalableTCP: public CTCP
{
protected:
   virtual void ACKAction()
   {
      if (m_dCWndSize <= 38.0)
         CTCP::ACKAction();
      else
      {
         if (m_bSlowStart)
            m_dCWndSize += 1.0;
         else
            m_dCWndSize += 0.01;
      }

      if (m_dCWndSize > m_iMaxCWndSize)
         m_dCWndSize = m_iMaxCWndSize;
   }

   virtual void DupACKAction()
   {
      if (m_dCWndSize <= 38.0)
         m_dCWndSize *= 0.5;
      else
         m_dCWndSize *= 0.875;

      if (m_dCWndSize < m_iMinCWndSize)
         m_dCWndSize = m_iMinCWndSize;
   }

private:
   static const int m_iMinCWndSize = 16;
   static const int m_iMaxCWndSize = 100000;

   static const int m_iCWndThresh = 38;
};


/*****************************************************************************
HighSpeed TCP congestion control
Reference:
Sally Floyd, HighSpeed TCP for Large Congestion Windows, RFC 3649, 
Experimental, December 2003
*****************************************************************************/

class CHSTCP: public CTCP
{
public:
   virtual void ACKAction()
   {
      m_dCWndSize += a(m_dCWndSize)/m_dCWndSize;
   }

   virtual void DupACKAction()
   {
      m_dCWndSize -= m_dCWndSize*b(m_dCWndSize);
   }

private:
   double a(double w)
   {
      return (w * w * 2.0 * b(w)) / ((2.0 - b(w)) * pow(w, 1.2) * 12.8);
   }

   double b(double w)
   {
      return (0.1 - 0.5) * (log(w) - log(38.)) / (log(83000.) - log(38.)) + 0.5;
   }

private:
   static const int m_iHighWnd = 83000;
   static const int m_iLowWnd = 38;
};


/*****************************************************************************
BiC TCP congestion control
Reference:
Lisong Xu, Khaled Harfoush, and Injong Rhee, "Binary Increase Congestion 
Control for Fast Long-Distance Networks", INFOCOM 2004.
*****************************************************************************/

class CBiCTCP: public CTCP
{
public:
   CBiCTCP()
   {
      m_dMaxWin = m_iDefaultMaxWin;
      m_dMinWin = m_dCWndSize;
      m_dTargetWin = (m_dMaxWin + m_dMinWin) / 2;

      m_dSSCWnd = 1.0;
      m_dSSTargetWin = m_dCWndSize + 1.0;
   }

protected:
   virtual void ACKAction()
   {
      if (m_dCWndSize < m_iLowWindow)
      {
         m_dCWndSize += 1/m_dCWndSize;
         return;
      }

      if (!m_bSlowStart)
      {
         if (m_dTargetWin - m_dCWndSize < m_iSMax)
            m_dCWndSize += (m_dTargetWin - m_dCWndSize)/m_dCWndSize;
         else
            m_dCWndSize += m_iSMax/m_dCWndSize;

         if (m_dMaxWin > m_dCWndSize)
         {
            m_dMinWin = m_dCWndSize;
            m_dTargetWin = (m_dMaxWin + m_dMinWin) / 2;
         }
         else
         {
            m_bSlowStart = true;
            m_dSSCWnd = 1.0;
            m_dSSTargetWin = m_dCWndSize + 1.0;
            m_dMaxWin = m_iDefaultMaxWin;
         }
      }
      else
      {
         m_dCWndSize += m_dSSCWnd/m_dCWndSize;
         if(m_dCWndSize >= m_dSSTargetWin)
         {
            m_dSSCWnd *= 2;
            m_dSSTargetWin = m_dCWndSize + m_dSSCWnd;
         }
         if(m_dSSCWnd >= m_iSMax)
            m_bSlowStart = false;
      }        
   }

   virtual void DupACKAction()
   {
      if (m_dCWndSize <= m_iLowWindow)
         m_dCWndSize *= 0.5;
      else
      {
         m_dPreMax = m_dMaxWin;
         m_dMaxWin = m_dCWndSize;
         m_dCWndSize *= 0.875;
         m_dMinWin = m_dCWndSize;

         if (m_dPreMax > m_dMaxWin)
         {
            m_dMaxWin = (m_dMaxWin + m_dMinWin) / 2;
            m_dTargetWin = (m_dMaxWin + m_dMinWin) / 2;
         }
      }
   }

private:
   static const int m_iLowWindow = 38;
   static const int m_iSMax = 32;
   static const int m_iSMin = 1;
   static const int m_iDefaultMaxWin = 1 << 29;

   double m_dMaxWin;
   double m_dMinWin;
   double m_dPreMax;
   double m_dTargetWin;
   double m_dSSCWnd;
   double m_dSSTargetWin;
};

/*****************************************************************************
TCP Westwood
reference:
http://www.cs.ucla.edu/NRL/hpi/tcpw/
*****************************************************************************/

class CWestwood: public CTCP
{
public:
   CWestwood(): m_dBWE(1), m_dLastBWE(1), m_dBWESample(1), m_dLastBWESample(1)
   {
      gettimeofday(&m_LastACKTime, 0);
   }

   virtual void onACK(const int& ack)
   {
      timeval currtime;
      gettimeofday(&currtime, 0);

      m_dBWESample = double(ack - m_iLastACK) / double((currtime.tv_sec - m_LastACKTime.tv_sec) * 1000.0 + (currtime.tv_usec - m_LastACKTime.tv_usec) / 1000.0);
      m_dBWE = 19.0/21.0 * m_dLastBWE + 1.0/21.0 * (m_dBWESample + m_dLastBWESample);

      m_LastACKTime = currtime;
      m_dLastBWE = m_dBWE;
      m_dLastBWESample = m_dBWESample;

      if (ack == m_iLastACK)
      {
         if (3 == ++ m_iDupACKCount)
         {
            m_bSlowStart = false;

            m_issthresh = int(ceil(getPerfInfo()->msRTT * m_dBWE));
            if (m_issthresh < 2)
            m_issthresh = 2;

            m_dCWndSize = m_issthresh + 3;
         }
         else if (m_iDupACKCount > 3)
            m_dCWndSize += 1.0;
         else
            ACKAction();
      }
      else
      {
         if (m_iDupACKCount >= 3)
            m_dCWndSize = m_issthresh;

         m_iLastACK = ack;
         m_iDupACKCount = 1;

         ACKAction();
      }
   }

   virtual void onTimeout()
   {
      m_issthresh = int(ceil(getPerfInfo()->msRTT * m_dBWE));
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_bSlowStart = true;
      m_dCWndSize = 2.0;
   }

private:
   double m_dBWE, m_dLastBWE;
   double m_dBWESample, m_dLastBWESample;
   timeval m_LastACKTime;
};


/*****************************************************************************
TCP Vegas
Reference:
L. Brakmo, S. O'Malley, and L. Peterson. TCP Vegas: New techniques for 
congestion detection and avoidance. In Proceedings of the SIGCOMM '94 
Symposium (Aug. 1994) pages 24-35. 

Note:
This class can be used to derive new delay based approaches, e.g., FAST.
*****************************************************************************/

class CVegas: public CTCP
{
public:
   CVegas()
   {
      m_iSSRound = 1;
      m_iRTT = 1000000;
      m_iBaseRTT = 1000000;
      gettimeofday(&m_LastCCTime, 0);
      m_iPktSent = 0;
      m_pAckWindow = new CACKWindow(100000);
   }

   ~CVegas()
   {
      delete m_pAckWindow; 
   }

   virtual void onACK(const int& seq)
   {
      double expected, actual, diff; //kbps

      int rtt = m_pAckWindow->acknowledge(seq, const_cast<int&>(seq));
      if (rtt > 0)
         m_iRTT = (m_iRTT * 15 + rtt) >> 4;

      timeval currtime;
      gettimeofday(&currtime, 0);

      if ((currtime.tv_sec - m_LastCCTime.tv_sec) * 1000000 + (currtime.tv_usec - m_LastCCTime.tv_usec) < m_iRTT)
         return;

      expected = m_dCWndSize * 1000.0 / m_iBaseRTT;
      actual = m_iPktSent / ((currtime.tv_sec - m_LastCCTime.tv_sec) * 1000.0 + (currtime.tv_usec - m_LastCCTime.tv_usec) / 1000.0);
      diff = expected - actual;

      if (m_bSlowStart)
      {
         if (diff < gamma)
            m_bSlowStart = false;

         if (m_iSSRound & 1)
            m_dCWndSize *= 2;

         m_iSSRound ++;
      }
      else
      {
         if (diff < alpha)
            m_dCWndSize += 1.0;
         else if (diff > beta)
            m_dCWndSize -= 1.0;
      }

      gettimeofday(&m_LastCCTime, 0);
      m_iPktSent = 0;
      if (m_iBaseRTT > m_iRTT)
         m_iBaseRTT = m_iRTT;
   }

   virtual void onPktSent(const CPacket* pkt)
   {
      m_pAckWindow->store(pkt->m_iSeqNo, pkt->m_iSeqNo);
      m_iPktSent ++;
   }

   virtual void onTimeout()
   {
   }

protected:
   int m_iSSRound;

   int m_iRTT;
   int m_iBaseRTT;
   timeval m_LastCCTime;

   int m_iPktSent;

   static const int alpha = 30; //kbps
   static const int beta = 60;  //kbps
   static const int gamma = 30; //kbps

   CACKWindow* m_pAckWindow;
};


/*****************************************************************************
FAST TCP
Reference:
1. C. Jin, D. X. Wei and S. H. Low, "FAST TCP: motivation, architecture, 
   algorithms, performance", IEEE Infocom, March 2004
2. C. Jin, D. X. Wei and S. H. Low, FAST TCP for High-Speed Long-Distance 
   Networks, Internet Draft, draft-jwl-tcp-fast-01.txt,
   http://netlab.caltech.edu/pub/papers/draft-jwl-tcp-fast-01.txt

Note:
   Precision of RTT measurement may make great difference in the throughput
*****************************************************************************/

class CFAST: public CVegas
{
public:
   CFAST()
   {
      m_dOldWin = m_dCWndSize;
      m_iNumACK = 100000;
   }

   virtual void onACK(const int& ack)
   {
      if (ack == m_iLastACK)
      {
         if (3 == ++ m_iDupACKCount)
         {
            m_dCWndSize *= 0.875;
            return;
         }
      }
      else
      {
         if (m_iDupACKCount >= 3)
         {
//            m_dCWndSize = m_issthresh;
//            return;
         }

         m_iLastACK = ack;
         m_iDupACKCount = 1;
      }

      if (0 == (++ m_iACKCount % m_iNumACK))
         m_dCWndSize += m_iIncDec;

      int rtt = m_pAckWindow->acknowledge(ack, const_cast<int&>(ack));
      if (rtt > 0)
         m_iRTT = (m_iRTT * 7 + rtt) >> 3;

      timeval currtime;
      gettimeofday(&currtime, 0);

      if ((currtime.tv_sec - m_LastCCTime.tv_sec) * 1000000 + (currtime.tv_usec - m_LastCCTime.tv_usec) < 2 * m_iRTT)
         return;

      m_dNewWin = 0.5 * (m_dOldWin + (double(m_iBaseRTT) / m_iRTT) * m_dCWndSize + alpha);
      if (m_dNewWin > 2.0 * m_dCWndSize)
        m_dNewWin = 2.0 * m_dCWndSize;

      m_iNumACK = int(ceil(fabs(m_dCWndSize / (m_dNewWin - m_dCWndSize)) / 2.0));
      if (m_dNewWin > m_dCWndSize)
         m_iIncDec = 1;
      else
         m_iIncDec = -1;

      m_dOldWin = m_dCWndSize;

      gettimeofday(&m_LastCCTime, 0);
      m_iPktSent = 0;
      if (m_iBaseRTT > m_iRTT)
         m_iBaseRTT = m_iRTT;
   }

private:
   static const int alpha = 200;

   double m_dOldWin;
   double m_dNewWin;

   int m_iNumACK;
   int m_iIncDec;
   int m_iACKCount;
};


/*****************************************************************************
Reliable UDP Blast

Note:
The class demostrates the simplest control mechanism. The sending rate can
be set at any time by using setRate().
*****************************************************************************/

class CUDPBlast: public CCC
{
public:
   CUDPBlast()
   {
      m_dPktSndPeriod = 1000000; 
      m_dCWndSize = 83333.0; 
   }

public:
   void setRate(int mbps)
   {
      m_dPktSndPeriod = (m_iSMSS * 8.0) / mbps;
   }

protected:
  static const int m_iSMSS = 1500;
};


/*****************************************************************************
Group Transport Protocol
Reference:
Ryan X. Wu, and Andrew Chien, "GTP: Group Transport Protocol for 
Lambda-Grids", in Proceedings of the 4th IEEE/ACM International Symposium on 
Cluster Computing and the Grid (CCGrid), April 2004

Note:
This is a demotration showing how to use UDT/CCC to implement group-based
control mechanisms, such GTP and CM.
*****************************************************************************/

struct gtpcomp;

class CGTP: public CCC
{
friend struct gtpcomp;

public:
   virtual void init()
   {
      m_dRequestRate = 1;

      m_llLastRecvPkt = 0;
      gettimeofday(&m_LastGCTime, 0);

      m_GTPSet.insert(this);
      rateAlloc();
   }

   virtual void close()
   {
      m_GTPSet.erase(this);
      rateAlloc();
   }

   virtual void onPktReceived()
   {
      timeval currtime;
      gettimeofday(&currtime, 0);

      int interval = (currtime.tv_sec - m_LastGCTime.tv_sec) * 1000000 + currtime.tv_usec - m_LastGCTime.tv_usec;

      if (interval < 2 * m_iRTT)
         return;

      const UDT::TRACEINFO* info = getPerfInfo();
      
      double realrate, lossrate = 0;
      realrate = (info->pktRecvTotal - m_llLastRecvPkt) * 1500 * 8.0 / interval;
      if (info->pktRecvTotal != m_llLastRecvPkt)
         lossrate = double(info->pktRcvLossTotal - m_iLastRcvLoss) / (info->pktRecvTotal - m_llLastRecvPkt);

      if (0 == lossrate)
         m_dRequestRate *= 1.02;
      else if (lossrate * 0.5 < 0.125)
         m_dRequestRate *= (1 - lossrate * 0.5); 
      else
         m_dRequestRate *= 0.875;

      if (m_dRequestRate > m_dTargetRate)
         m_dRequestRate = m_dTargetRate;

      requestRate(int(m_dRequestRate));

      m_llLastRecvPkt = info->pktRecvTotal;
      m_iLastRcvLoss = info->pktRcvLossTotal;
      m_LastGCTime = currtime;
      m_iRTT = int(info->msRTT * 1000);
   }

   virtual void processCustomPkt(CPacket* pkt)
   {
      if (m_iGTPPktType != pkt->getExtendedType())
         return;

      m_dPktSndPeriod = (1500 * 8.0) / *(int *)(pkt->m_pcData);
   }

public:
   void setBandwidth(const double& mbps)
   {
      m_dBandwidth = mbps;
   }

private:
   void rateAlloc();

   void requestRate(int mbps)
   {
      CPacket pkt;
      pkt.pack(0x111, const_cast<void*>((void*)&m_iGTPPktType), &mbps, sizeof(int));

      sendCustomMsg(pkt);
   }

private:
   double m_dTargetRate;
   double m_dBandwidth;

   double m_dRequestRate;

   timeval m_LastGCTime;
   __int64 m_llLastRecvPkt;
   int m_iLastRcvLoss;
   int m_iRTT;

private:
   static set<CGTP*> m_GTPSet;
   static const int m_iGTPPktType = 0xFFF;
};

set<CGTP*> CGTP::m_GTPSet;
struct gtpcomp
{
  bool operator()(const CGTP* g1, const CGTP* g2) const
  {
    return g1->m_dBandwidth < g2->m_dBandwidth;
  }
};

void CGTP::rateAlloc()
{
   if (0 == m_GTPSet.size())
      return;

   vector<CGTP*> GTPVec;
   copy(m_GTPSet.begin(), m_GTPSet.end(), GTPVec.begin());
   sort(GTPVec.begin(), GTPVec.end(), gtpcomp());

   int N = GTPVec.size();
   int n = 0;
   vector<CGTP*>::iterator i = GTPVec.begin();
   double availbw = (*(i + N - 1))->m_dBandwidth;
   double fairshare = availbw / N;

   while ((n < N) && ((*i)->m_dBandwidth < fairshare))
   {
      (*i)->m_dTargetRate = (*i)->m_dBandwidth;
      availbw -= (*i)->m_dTargetRate;
      fairshare = availbw / (N - n);

      ++ n;
      ++ i;
   }

   for (; i != GTPVec.end(); ++ i)
      (*i)->m_dTargetRate = fairshare;
}


/*****************************************************************************
Protocol using reliable control channel

Note:
The feedback method using sendCustomMsg() as shown in CGTP sends data
using unreliable channel. If some protocol nees reliable channel to 
transfer control message, a seperate TCP connection can be sarted.
The CReliableChannel class below can be used to derive such protocols.
*****************************************************************************/

class CReliableChannel: public CCC
{
public:
   int startTCPServer(sockaddr* addr)
   {
      if (-1 == (m_TCPSocket = socket(AF_INET, SOCK_STREAM, 0)))
         return -1;

      if (-1 == (bind(m_TCPSocket, addr, sizeof(sockaddr_in))))
         return -1;

      if (-1 == (listen(m_TCPSocket, 10)))
         return -1;

      if (-1 == (m_TCPSocket = accept(m_TCPSocket, NULL, NULL)))
         return -1;

      #ifndef WIN32
         pthread_create(&m_TCPThread, NULL, TCPProcessing, this);
      #endif

      return 0;
   }

   int startTCPClient(sockaddr* addr)
   {
      if (-1 == (m_TCPSocket = socket(AF_INET, SOCK_STREAM, 0)))
         return -1;

      if (-1 == (connect(m_TCPSocket, addr, sizeof(sockaddr_in))))
         return -1;

      #ifndef WIN32
         pthread_create(&m_TCPThread, NULL, TCPProcessing, this);
      #endif

      return 0;
   }

   int sendReliableMsg(const char* data, const int& size)
   {
      return send(m_TCPSocket, data, size, 0);
   }


protected:
   virtual void processRealiableMsg()
   {
      char data[1500];

      while (true)
      {
         recv(m_TCPSocket, data, 1500, 0);
         //process data
      }
   }

protected:
   int m_TCPSocket;

   pthread_t m_TCPThread;

private:
   static void* TCPProcessing(void* self)
   {
      ((CReliableChannel*)self)->processRealiableMsg();
      return NULL;
   }
};

