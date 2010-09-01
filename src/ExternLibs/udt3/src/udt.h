/*****************************************************************************
Copyright © 2001 - 2006, The Board of Trustees of the University of Illinois.
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
This is the (only) header file of the UDT API, needed for programming with UDT.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 03/14/2006
*****************************************************************************/

#ifndef _UDT_H_
#define _UDT_H_


#ifndef WIN32
   #include <sys/types.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
#else
   #include <windows.h>
#endif
#include <fstream>
#include <set>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

#ifndef WIN32
   // Explicitly define 32-bit and 64-bit numbers
   #define __int32 int
   #define __int64 long long

   #define UDT_API
#else
   #ifdef UDT_EXPORTS
      #define UDT_API __declspec(dllexport)
   #else
      #define UDT_API __declspec(dllimport)
   #endif
#endif

typedef void (*UDT_MEM_ROUTINE)(char*, int);

typedef int UDTSOCKET;

typedef set<UDTSOCKET> ud_set;
#define UD_CLR(u, uset) ((uset)->erase(u))
#define UD_ISSET(u, uset) ((uset)->find(u) != (uset)->end())
#define UD_SET(u, uset) ((uset)->insert(u))
#define UD_ZERO(uset) ((uset)->clear())

////////////////////////////////////////////////////////////////////////////////

enum UDTOpt
{
   UDT_MSS,             // the Maximum Transfer Unit
   UDT_SNDSYN,          // if sending is blocking
   UDT_RCVSYN,          // if receiving is blocking
   UDT_CC,              // custom congestion control algorithm
   UDT_FC,              // deprecated, for compatibility only
   UDT_SNDBUF,          // maximum buffer in sending queue
   UDT_RCVBUF,          // UDT receiving buffer size
   UDT_LINGER,          // waiting for unsent data when closing
   UDP_SNDBUF,          // UDP sending buffer size
   UDP_RCVBUF,          // UDP receiving buffer size
   UDT_MAXMSG,          // maximum datagram message size
   UDT_MSGTTL,          // time-to-live of a datagram message
   UDT_RENDEZVOUS,      // rendezvous connection mode
   UDT_SNDTIMEO,	// send() timeout
   UDT_RCVTIMEO		// recv() timeout
};

////////////////////////////////////////////////////////////////////////////////

struct UDT_API CPerfMon
{
   // global measurements
   __int64 msTimeStamp;                 // time since the UDT entity is started, in milliseconds
   __int64 pktSentTotal;                // total number of sent data packets, including retransmissions
   __int64 pktRecvTotal;                // total number of received packets
   __int32 pktSndLossTotal;             // total number of lost packets (sender side)
   __int32 pktRcvLossTotal;             // total number of lost packets (receiver side)
   __int32 pktRetransTotal;             // total number of retransmitted packets
   __int32 pktSentACKTotal;             // total number of sent ACK packets
   __int32 pktRecvACKTotal;             // total number of received ACK packets
   __int32 pktSentNAKTotal;             // total number of sent NAK packets
   __int32 pktRecvNAKTotal;             // total number of received NAK packets

   // local measurements
   __int64 pktSent;                     // number of sent data packets, including retransmissions
   __int64 pktRecv;                     // number of received packets
   __int32 pktSndLoss;                  // number of lost packets (sender side)
   __int32 pktRcvLoss;                  // number of lost packets (receiverer side)
   __int32 pktRetrans;                  // number of retransmitted packets
   __int32 pktSentACK;                  // number of sent ACK packets
   __int32 pktRecvACK;                  // number of received ACK packets
   __int32 pktSentNAK;                  // number of sent NAK packets
   __int32 pktRecvNAK;                  // number of received NAK packets
   double mbpsSendRate;                 // sending rate in Mbps
   double mbpsRecvRate;                 // receiving rate in Mbps

   // instant measurements
   double usPktSndPeriod;               // packet sending period, in microseconds
   __int32 pktFlowWindow;               // flow window size, in number of packets
   __int32 pktCongestionWindow;         // congestion window size, in number of packets
   __int32 pktFlightSize;               // number of packets on flight
   double msRTT;                        // RTT, in milliseconds
   double mbpsBandwidth;                // estimated bandwidth, in Mbps
   __int32 byteAvailSndBuf;             // available UDT sender buffer size
   __int32 byteAvailRcvBuf;             // available UDT receiver buffer size
};

////////////////////////////////////////////////////////////////////////////////

class UDT_API CUDTException
{
public:
   CUDTException(__int32 major = 0, __int32 minor = 0, __int32 err = -1);
   CUDTException(const CUDTException& e);
   virtual ~CUDTException();

      // Functionality:
      //    Get the description of the exception.
      // Parameters:
      //    None.
      // Returned value:
      //    Text message for the exception description.

   virtual const char* getErrorMessage();

      // Functionality:
      //    Get the system errno for the exception.
      // Parameters:
      //    None.
      // Returned value:
      //    errno.

   virtual const __int32 getErrorCode() const;

private:
   __int32 m_iMajor;    // major exception categories

// 0: correct condition
// 1: network setup exception
// 2: network connection broken
// 3: memory exception
// 4: file exception
// 5: method not supported
// 6+: undefined error

   __int32 m_iMinor;    // for specific error reasons

   __int32 m_iErrno;    // errno returned by the system if there is any

   char m_pcMsg[1024];  // text error message
};

////////////////////////////////////////////////////////////////////////////////

namespace UDT
{
typedef CUDTException ERRORINFO;
typedef UDTOpt SOCKOPT;
typedef CPerfMon TRACEINFO;
typedef ud_set UDSET;

UDT_API extern const UDTSOCKET INVALID_SOCK;
#undef ERROR
UDT_API extern const int ERROR;

UDT_API UDTSOCKET socket(int af, int type, int protocol);

UDT_API int bind(UDTSOCKET u, const struct sockaddr* name, int namelen);

UDT_API int listen(UDTSOCKET u, int backlog);

UDT_API UDTSOCKET accept(UDTSOCKET u, struct sockaddr* addr, int* addrlen);

UDT_API int connect(UDTSOCKET u, const struct sockaddr* name, int namelen);

UDT_API int close(UDTSOCKET u);

UDT_API int getpeername(UDTSOCKET u, struct sockaddr* name, int* namelen);

UDT_API int getsockname(UDTSOCKET u, struct sockaddr* name, int* namelen);

UDT_API int getsockopt(UDTSOCKET u, int level, SOCKOPT optname, void* optval, int* optlen);

UDT_API int setsockopt(UDTSOCKET u, int level, SOCKOPT optname, const void* optval, int optlen);

UDT_API int shutdown(UDTSOCKET u, int how);

UDT_API int send(UDTSOCKET u, const char* buf, int len, int flags = 0, int* handle = NULL, UDT_MEM_ROUTINE routine = NULL);

UDT_API int recv(UDTSOCKET u, char* buf, int len, int flags = 0, int* handle = NULL, UDT_MEM_ROUTINE routine = NULL);

UDT_API int sendmsg(UDTSOCKET u, const char* buf, int len, int ttl = -1, bool inorder = false);

UDT_API int recvmsg(UDTSOCKET u, char* buf, int len);

UDT_API __int64 sendfile(UDTSOCKET u, ifstream& ifs, const __int64& offset, __int64& size, const int& block = 366000);

UDT_API __int64 recvfile(UDTSOCKET u, ofstream& ofs, const __int64& offset, __int64& size, const int& block = 7320000);

UDT_API bool getoverlappedresult(UDTSOCKET u, int handle, int& progress, bool wait = false);

UDT_API int select(int nfds, UDSET* readfds, UDSET* writefds, UDSET* exceptfds, const struct timeval* timeout);

UDT_API ERRORINFO getlasterror();

UDT_API int perfmon(UDTSOCKET u, TRACEINFO* perf, bool clear = true);
}


#endif
