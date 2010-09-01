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
This header file contains the definition of structures related to UDT API.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 02/14/2006
*****************************************************************************/

#ifndef __UDT_API_H__
#define __UDT_API_H__


#include <map>
#include "udt.h"
#include "packet.h"


class CUDT;


struct CUDTSocket
{
   CUDTSocket();
   ~CUDTSocket();

   enum UDTSTATUS {INIT = 1, OPENED, LISTENING, CONNECTED, CLOSED};
   UDTSTATUS m_Status;                  // current socket state

   timeval m_TimeStamp;                 // time when the socket is closed

   __int32 m_iIPversion;                // IP version
   sockaddr* m_pSelfAddr;               // pointer to the local address of the socket
   sockaddr* m_pPeerAddr;               // pointer to the peer address of the socket

   UDTSOCKET m_Socket;                  // socket ID
   UDTSOCKET m_ListenSocket;            // ID of the listener socket; 0 means this is an independent socket

   CUDT* m_pUDT;                        // pointer to the UDT entity

   set<UDTSOCKET>* m_pQueuedSockets;    // set of connections waiting for accept()
   set<UDTSOCKET>* m_pAcceptSockets;    // set of accept()ed connections

   pthread_cond_t m_AcceptCond;         // used to block "accept" call
   pthread_mutex_t m_AcceptLock;        // mutex associated to m_AcceptCond

   unsigned __int32 m_uiBackLog;        // maximum number of connections in queue
};

////////////////////////////////////////////////////////////////////////////////

class CUDTUnited
{
public:
   CUDTUnited();
   ~CUDTUnited();

public:

      // Functionality:
      //    Create a new UDT socket.
      // Parameters:
      //    0) [in] af: IP version, IPv4 (AF_INET) or IPv6 (AF_INET6).
      //    1) [in] type: socket type, SOCK_STREAM or SOCK_DGRAM
      // Returned value:
      //    The new UDT socket ID, or INVALID_SOCK.
   UDTSOCKET newSocket(const __int32& af, const __int32& type);

      // Functionality:
      //    Create a new UDT connection.
      // Parameters:
      //    0) [in] listen: the listening UDT socket;
      //    1) [in] peer: peer address.
      //    2) [in/out] hs: handshake information from peer side (in), negotiated value (out);
      // Returned value:
      //    If the new connection is successfully created: 1 success, 0 already exist, -1 error.

   int newConnection(const UDTSOCKET listen, const sockaddr* peer, CHandShake* hs);

      // Functionality:
      //    look up the UDT entity according to its ID.
      // Parameters:
      //    0) [in] u: the UDT socket ID.
      // Returned value:
      //    Pointer to the UDT entity.

   CUDT* lookup(const UDTSOCKET u);

      // socket APIs

   __int32 bind(const UDTSOCKET u, const sockaddr* name, const __int32& namelen);
   __int32 listen(const UDTSOCKET u, const __int32& backlog);
   UDTSOCKET accept(const UDTSOCKET listen, sockaddr* addr, __int32* addrlen);
   __int32 connect(const UDTSOCKET u, const sockaddr* name, const __int32& namelen);
   __int32 close(const UDTSOCKET u);
   __int32 getpeername(const UDTSOCKET u, sockaddr* name, __int32* namelen);
   __int32 getsockname(const UDTSOCKET u, sockaddr* name, __int32* namelen);
   __int32 select(ud_set* readfds, ud_set* writefds, ud_set* exceptfds, const timeval* timeout);

      // Functionality:
      //    record the UDT exception.
      // Parameters:
      //    0) [in] e: pointer to a UDT exception instance.
      // Returned value:
      //    None.

   void setError(CUDTException* e);

      // Functionality:
      //    look up the most recent UDT exception.
      // Parameters:
      //    None.
      // Returned value:
      //    pointer to a UDT exception instance.

   CUDTException* getError();

private:
   map<UDTSOCKET, CUDTSocket*> m_Sockets;       // stores all the socket structures

   pthread_mutex_t m_ControlLock;               // used to synchronize UDT API

   pthread_mutex_t m_IDLock;                    // used to synchronize ID generation
   UDTSOCKET m_SocketID;                        // seed to generate a new unique socket ID

private:
   pthread_key_t m_TLSError;                    // thread local error record (last error)
   static void TLSDestroy(void* e) {delete (CUDTException*)e;}

private:
   CUDTSocket* locate(const UDTSOCKET u);
   CUDTSocket* locate(const UDTSOCKET u, const sockaddr* peer);
   void checkBrokenSockets();
   void removeSocket(const UDTSOCKET u);
};


#endif
