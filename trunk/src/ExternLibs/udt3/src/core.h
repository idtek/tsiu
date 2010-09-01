/*****************************************************************************
Copyright � 2001 - 2006, The Board of Trustees of the University of Illinois.
All Rights Reserved.

UDP-based Data Transfer Library (UDT) version 3

Laboratory for Advanced Computing (LAC)
National Center for Data Mining (NCDM)
University of Illinois at Chicago
http://www.lac.uic.edu/

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*****************************************************************************/

/*****************************************************************************
This header file contains the definition of UDT buffer structure and operations.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 03/20/2006
*****************************************************************************/

#ifndef __UDT_CORE_H__
#define __UDT_CORE_H__


#include "udt.h"
#include "common.h"
#include "list.h"
#include "buffer.h"
#include "window.h"
#include "packet.h"
#include "channel.h"
#include "api.h"
#include "ccc.h"


class UDT_API CUDT
{
friend class CUDTSocket;
friend class CUDTUnited;
friend class CCC;

private: // constructor and desctructor
   CUDT();
   CUDT(const CUDT& ancestor);
   const CUDT& operator=(const CUDT&) {return *this;}
   ~CUDT();

public: //API
   static UDTSOCKET socket(int af, int type = SOCK_STREAM, int protocol = 0);
   static int bind(UDTSOCKET u, const sockaddr* name, int namelen);
   static int listen(UDTSOCKET u, int backlog);
   static UDTSOCKET accept(UDTSOCKET u, sockaddr* addr, int* addrlen);
   static int connect(UDTSOCKET u, const sockaddr* name, int namelen);
   static int close(UDTSOCKET u);
   static int getpeername(UDTSOCKET u, sockaddr* name, int* namelen);
   static int getsockname(UDTSOCKET u, sockaddr* name, int* namelen);
   static int getsockopt(UDTSOCKET u, int level, UDTOpt optname, void* optval, int* optlen);
   static int setsockopt(UDTSOCKET u, int level, UDTOpt optname, const void* optval, int optlen);
   static int shutdown(UDTSOCKET u, int how);
   static int send(UDTSOCKET u, const char* buf, int len, int flags = 0, int* handle = NULL, UDT_MEM_ROUTINE routine = NULL);
   static int recv(UDTSOCKET u, char* buf, int len, int flags = 0, int* handle = NULL, UDT_MEM_ROUTINE routine = NULL);
   static int sendmsg(UDTSOCKET u, const char* buf, int len, int ttl = -1, bool inorder = false);
   static int recvmsg(UDTSOCKET u, char* buf, int len);
   static __int64 sendfile(UDTSOCKET u, ifstream& ifs, const __int64& offset, __int64& size, const int& block = 366000);
   static __int64 recvfile(UDTSOCKET u, ofstream& ofs, const __int64& offset, __int64& size, const int& block = 7320000);
   static bool getoverlappedresult(UDTSOCKET u, int handle, int& progress, bool wait = false);
   static int select(int nfds, ud_set* readfds, ud_set* writefds, ud_set* exceptfds, const timeval* timeout);
   static CUDTException& getlasterror();
   static int perfmon(UDTSOCKET u, CPerfMon* perf, bool clear = true);

public: // internal API
   static bool isUSock(UDTSOCKET u);
   static CUDT* getUDTHandle(UDTSOCKET u);

private:
      // Functionality:
      //    initialize a UDT entity and bind to a local address.
      // Parameters:
      //    0) [in] addr: pointer to the local address to be bound to.
      // Returned value:
      //    None.

   void open(const sockaddr* addr = NULL);

      // Functionality:
      //    Start listening to any connection request.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   void listen();

      // Functionality:
      //    Connect to a UDT entity listening at address "peer".
      // Parameters:
      //    0) [in] peer: The address of the listening UDT entity.
      // Returned value:
      //    None.

   void connect(const sockaddr* peer);

      // Functionality:
      //    Connect to a UDT entity listening at address "peer", which has sent "hs" request.
      // Parameters:
      //    0) [in] peer: The address of the listening UDT entity.
      //    1) [in/out] hs: The handshake information sent by the peer side (in), negotiated value (out).
      // Returned value:
      //    None.

   void connect(const sockaddr* peer, CHandShake* hs);

      // Functionality:
      //    Close the opened UDT entity.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   void close();
      // Functionality:
      //    Request UDT to send out a data block "data" with size of "len".
      // Parameters:
      //    0) [in] data: The address of the application data to be sent.
      //    1) [in] len: The size of the data block.
      //    2) [in, out] overlapped: A pointer to the returned overlapped IO handle.
      //    3) [in] func: pointer to a function to process the buffer after overlapped IO is completed.
      // Returned value:
      //    Actual size of data sent.

   __int32 send(char* data, const __int32& len,  __int32* overlapped = NULL, const UDT_MEM_ROUTINE func = NULL);

      // Functionality:
      //    Request UDT to receive data to a memory block "data" with size of "len".
      // Parameters:
      //    0) [out] data: data received.
      //    1) [in] len: The desired size of data to be received.
      //    2) [out] overlapped: A pointer to the returned overlapped IO handle.
      //    3) [in] unused.
      // Returned value:
      //    Actual size of data received.

   __int32 recv(char* data, const __int32& len, __int32* overlapped = NULL, const UDT_MEM_ROUTINE func = NULL);

      // Functionality:
      //    send a message of a memory block "data" with size of "len".
      // Parameters:
      //    0) [out] data: data received.
      //    1) [in] len: The desired size of data to be received.
      //    2) [in] ttl: the time-to-live of the message.
      //    3) [in] inorder: if the message should be delivered in order.
      // Returned value:
      //    Actual size of data sent.

   __int32 sendmsg(const char* data, const __int32& len, const __int32& ttl, const bool& inorder);

      // Functionality:
      //    Receive a message to buffer "data".
      // Parameters:
      //    0) [out] data: data received.
      //    1) [in] len: size of the buffer.
      // Returned value:
      //    Actual size of data received.

   __int32 recvmsg(char* data, const __int32& len);

      // Functionality:
      //    query the result of an overlapped IO indicated by "handle".
      // Parameters:
      //    0) [in] handle: the handle that indicates the submitted overlapped IO.
      //    1) [out] progess: how many data left to be sent/receive.
      //    2) [in] wait: wait for the IO finished or not.
      // Returned value:
      //    if the overlapped IO is completed.

   bool getOverlappedResult(const __int32& handle, __int32& progress, const bool& wait = false);

      // Functionality:
      //    Request UDT to send out a file described as "fd", starting from "offset", with size of "size".
      // Parameters:
      //    0) [in] ifs: The input file stream.
      //    1) [in] offset: From where to read and send data;
      //    2) [in] size: How many data to be sent.
      //    3) [in] block: size of block per read from disk
      // Returned value:
      //    Actual size of data sent.

   __int64 sendfile(ifstream& ifs, const __int64& offset, const __int64& size, const __int32& block = 366000);

      // Functionality:
      //    Request UDT to receive data into a file described as "fd", starting from "offset", with expected size of "size".
      // Parameters:
      //    0) [out] ofs: The output file stream.
      //    1) [in] offset: From where to write data;
      //    2) [in] size: How many data to be received.
      //    3) [in] block: size of block per write to disk
      // Returned value:
      //    Actual size of data received.

   __int64 recvfile(ofstream& ofs, const __int64& offset, const __int64& size, const __int32& block = 7320000);

      // Functionality:
      //    Configure UDT options.
      // Parameters:
      //    0) [in] optName: The enum name of a UDT option.
      //    1) [in] optval: The value to be set.
      //    2) [in] optlen: size of "optval".
      // Returned value:
      //    None.

   void setOpt(UDTOpt optName, const void* optval, const __int32& optlen);

      // Functionality:
      //    Read UDT options.
      // Parameters:
      //    0) [in] optName: The enum name of a UDT option.
      //    1) [in] optval: The value to be returned.
      //    2) [out] optlen: size of "optval".
      // Returned value:
      //    None.

   void getOpt(UDTOpt optName, void* optval, __int32& optlen);

      // Functionality:
      //    read the performance data since last sample() call.
      // Parameters:
      //    0) [in, out] perf: pointer to a CPerfMon structure to record the performance data.
      //    1) [in] clear: flag to decide if the local performance trace should be cleared.
      // Returned value:
      //    None.

   void sample(CPerfMon* perf, bool clear = true);

private:
   static CUDTUnited s_UDTUnited;               // UDT global management base

public:
#ifndef WIN32
   const static UDTSOCKET INVALID_SOCK = -1;    // invalid socket descriptor
   const static int ERROR = -1;                 // socket api error returned value
#else
   const static int INVALID_SOCK = -1;
   #undef ERROR
   const static int ERROR = -1;
#endif

private:
   UDTSOCKET m_SocketID;                        // UDT socket number
   __int32 m_iSockType;                         // Type of the UDT connection (SOCK_STREAM or SOCK_DGRAM)

private: // Version
   const __int32 m_iVersion;                    // UDT version, for compatibility use

private: // Threads, data channel, and timing facility
#ifndef WIN32
   bool m_bSndThrStart;                         // lazy snd thread creation
#endif
   pthread_t m_SndThread;                       // Sending thread
   pthread_t m_RcvThread;                       // Receiving thread
   CChannel* m_pChannel;                        // UDP channel
   CTimer* m_pTimer;                            // Timing facility
   unsigned __int64 m_ullCPUFrequency;          // CPU clock frequency, used for Timer

private: // Timing intervals
   const __int32 m_iSYNInterval;                // Periodical Rate Control Interval, 10 microseconds
   const __int32 m_iSelfClockInterval;          // ACK interval for self-clocking

private: // Packet size and sequence number attributes
   __int32 m_iPktSize;                          // Maximum/regular packet size, in bytes
   __int32 m_iPayloadSize;                      // Maximum/regular payload size, in bytes

private: // Options
   __int32 m_iMSS;                              // Maximum Segment Size
   bool m_bSynSending;                          // Sending syncronization mode
   bool m_bSynRecving;                          // Receiving syncronization mode
   __int32 m_iFlightFlagSize;                   // Maximum number of packets in flight from the peer side
   __int32 m_iSndQueueLimit;                    // Maximum length of the sending buffer queue
   __int32 m_iUDTBufSize;                       // UDT buffer size (for receiving)
   linger m_Linger;                             // Linger information on close
   __int32 m_iUDPSndBufSize;                    // UDP sending buffer size
   __int32 m_iUDPRcvBufSize;                    // UDP receiving buffer size
   __int32 m_iMaxMsg;                           // Maximum message size of datagram UDT connection
   __int32 m_iMsgTTL;                           // Time-to-live of a datagram message, in microseconds
   __int32 m_iIPversion;                        // IP version
   bool m_bRendezvous;                          // Rendezvous connection mode
   __int32 m_iSndTimeOut;                       // sending timeout in milliseconds
   __int32 m_iRcvTimeOut;                       // receiving timeout in milliseconds

   const __int32 m_iProbeInterval;              // Number of regular packets between two probing packet pairs
   const __int32 m_iQuickStartPkts;             // Number of packets to be sent as a quick start

private: // CCC
   CCCVirtualFactory* m_pCCFactory;             // Factory class to create a specific CC instance
   CCC* m_pCC;                                  // custom congestion control class

private: // Status
   volatile bool m_bListening;                  // If the UDT entit is listening to connection
   volatile bool m_bConnected;                  // Whether the connection is on or off
   volatile bool m_bClosing;                    // If the UDT entity is closing
   volatile bool m_bShutdown;                   // If the peer side has shutdown the connection
   volatile bool m_bBroken;                     // If the connection has been broken
   bool m_bOpened;                              // If the UDT entity has been opened
   bool m_bSndSlowStart;                        // If UDT is during slow start phase (snd side flag)
   bool m_bRcvSlowStart;                        // If UDT is during slow start phase (rcv side flag)
   bool m_bFreeze;                              // freeze the data sending
   __int32 m_iEXPCount;                         // Expiration counter
   __int32 m_iBandwidth;                        // Estimated bandwidth

private: // connection setup
   pthread_t m_ListenThread;

   #ifndef WIN32
      static void* listenHandler(void* listener);
   #else
      static DWORD WINAPI listenHandler(LPVOID listener);
   #endif

private: // Sending related data
   CSndBuffer* m_pSndBuffer;                    // Sender buffer
   CSndLossList* m_pSndLossList;                // Sender loss list
   CPktTimeWindow* m_pSndTimeWindow;            // Packet sending time window

   volatile unsigned __int64 m_ullInterval;     // Inter-packet time, in CPU clock cycles
   unsigned __int64 m_ullLastDecRate;           // inter-packet time when last decrease occurs
   unsigned __int64 m_ullTimeDiff;              // aggregate difference in inter-packet time

   __int32 m_iFlowWindowSize;                   // Flow control window size
   __int32 m_iMaxFlowWindowSize;                // Maximum flow window size = flight flag size of the peer side
   volatile double m_dCongestionWindow;         // congestion window size

   __int32 m_iNAKCount;                         // NAK counter
   __int32 m_iDecRandom;                        // random threshold on decrease by number of loss events
   __int32 m_iAvgNAKNum;                        // average number of NAKs per congestion

   timeval m_LastSYNTime;                       // the timestamp when last rate control occured
   bool m_bLoss;                                // if there is any loss during last RC period

   volatile __int32 m_iSndLastAck;              // Last ACK received
   __int32 m_iSndLastDataAck;                   // The real last ACK that updates the sender buffer and loss list
   __int32 m_iSndCurrSeqNo;                     // The largest sequence number that has been sent
   __int32 m_iLastDecSeq;                       // Sequence number sent last decrease occurs

   __int32 m_iISN;                              // Initial Sequence Number

private: // Receiving related data
   CRcvBuffer* m_pRcvBuffer;                    // Receiver buffer
   CRcvLossList* m_pRcvLossList;                // Receiver loss list
   CIrregularPktList* m_pIrrPktList;            // Irregular sized packet list
   CACKWindow* m_pACKWindow;                    // ACK history window
   CPktTimeWindow* m_pRcvTimeWindow;            // Packet arrival time window

   __int32 m_iRTT;                              // RTT
   __int32 m_iRTTVar;                           // RTT variance

   __int32 m_iRcvLastAck;                       // Last sent ACK
   unsigned __int64 m_ullLastAckTime;           // Timestamp of last ACK
   __int32 m_iRcvLastAckAck;                    // Last sent ACK that has been acknowledged
   __int32 m_iAckSeqNo;                         // Last ACK sequence number
   __int32 m_iRcvCurrSeqNo;                     // Largest received sequence number
   __int32 m_iNextExpect;                       // Sequence number of next speculated packet to receive

   volatile bool m_bReadBuf;                    // Application has called "recv" but has not finished
   volatile char* m_pcTempData;                 // Pointer to the buffer that application want to put received data into
   volatile __int32 m_iTempLen;                 // Size of the "m_pcTempData"
   volatile UDT_MEM_ROUTINE m_iTempRoutine;     // pointer ot a routine function to process "m_pcTempData"

   __int32 m_iUserBufBorder;                    // Sequence number of last packet that will fulfill a user buffer

   unsigned __int64 m_ullLastWarningTime;       // Last time that a warning message is sent

   __int32 m_iPeerISN;                          // Initial Sequence Number of the peer side

   __int32 m_iFlowControlWindow;                // flow control window size to be advertised

private: // Overlapped IO related
   __int32 m_iSndHandle;                        // seed used to generate an overlapped sending handle
   __int32 m_iRcvHandle;                        // seed used to generate an overlapped receiving handle

private: // synchronization: mutexes and conditions
   pthread_mutex_t m_ConnectionLock;            // used to synchronize connection operation

   pthread_cond_t m_SendDataCond;               // used to block sending when there is no data
   pthread_mutex_t m_SendDataLock;              // lock associated to m_SendDataCond

   pthread_cond_t m_SendBlockCond;              // used to block "send" call
   pthread_mutex_t m_SendBlockLock;             // lock associated to m_SendBlockCond

   pthread_mutex_t m_AckLock;                   // used to protected sender's loss list when processing ACK

   pthread_cond_t m_WindowCond;                 // used to block sending when flow window is exceeded
   pthread_mutex_t m_WindowLock;                // lock associated to m_WindowLock

   pthread_cond_t m_RecvDataCond;               // used to block "recv" when there is no data
   pthread_mutex_t m_RecvDataLock;              // lock associated to m_RecvDataCond

   pthread_cond_t m_OverlappedRecvCond;         // used to block "recv" when overlapped receving is in progress
   pthread_mutex_t m_OverlappedRecvLock;        // lock associated to m_OverlappedRecvCond

   pthread_mutex_t m_HandleLock;                // used to generate unique send/recv handle

   pthread_mutex_t m_SendLock;                  // used to synchronize "send" call
   pthread_mutex_t m_RecvLock;                  // used to synchronize "recv" call

   void initSynch();
   void destroySynch();
   void releaseSynch();

private: // Thread handlers
   #ifndef WIN32
      static void* sndHandler(void* sender);
      static void* rcvHandler(void* recver);
   #else
      static DWORD WINAPI sndHandler(LPVOID sender);
      static DWORD WINAPI rcvHandler(LPVOID recver);
   #endif

private: // congestion control
   void rateControl();
   void flowControl(const __int32& recvrate);
private: // Generation and processing of control packet
   void sendCtrl(const __int32& pkttype, void* lparam = NULL, void* rparam = NULL, const __int32& size = 0);
   void processCtrl(CPacket& ctrlpkt);

private: // Trace
   timeval m_StartTime;                         // timestamp when the UDT entity is started
   __int64 m_llSentTotal;                       // total number of sent data packets, including retransmissions
   __int64 m_llRecvTotal;                       // total number of received packets
   __int32 m_iSndLossTotal;                     // total number of lost packets (sender side)
   __int32 m_iRcvLossTotal;                     // total number of lost packets (receiver side)
   __int32 m_iRetransTotal;                     // total number of retransmitted packets
   __int32 m_iSentACKTotal;                     // total number of sent ACK packets
   __int32 m_iRecvACKTotal;                     // total number of received ACK packets
   __int32 m_iSentNAKTotal;                     // total number of sent NAK packets
   __int32 m_iRecvNAKTotal;                     // total number of received NAK packets

   timeval m_LastSampleTime;                    // last performance sample time
   __int64 m_llTraceSent;                       // number of pakctes sent in the last trace interval
   __int64 m_llTraceRecv;                       // number of pakctes received in the last trace interval
   __int32 m_iTraceSndLoss;                     // number of lost packets in the last trace interval (sender side)
   __int32 m_iTraceRcvLoss;                     // number of lost packets in the last trace interval (receiver side)
   __int32 m_iTraceRetrans;                     // number of retransmitted packets in the last trace interval
   __int32 m_iSentACK;                          // number of ACKs sent in the last trace interval
   __int32 m_iRecvACK;                          // number of ACKs received in the last trace interval
   __int32 m_iSentNAK;                          // number of NAKs sent in the last trace interval
   __int32 m_iRecvNAK;                          // number of NAKs received in the last trace interval

private: // internal data
   char* m_pcTmpBuf;
};


#endif
