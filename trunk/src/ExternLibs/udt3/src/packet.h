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
This header file contains the definition of UDT packet structure and operations.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 02/14/2006
*****************************************************************************/

#ifndef __UDT_PACKET_H__
#define __UDT_PACKET_H__


#include "common.h"
#include "udt.h"


class CChannel;


class CPacket
{
friend class CChannel;

public:
   __int32& m_iSeqNo;           // alias: sequence number
   __int32& m_iMsgNo;           // alias: message number
   __int32& m_iTimeStamp;       // alias: timestamp
   char*& m_pcData;             // alias: data/control information

   const static __int32 m_iPktHdrSize = 12;

public:
   CPacket();
   ~CPacket();

      // Functionality:
      //    Get the payload or the control information field length.
      // Parameters:
      //    None.
      // Returned value:
      //    the payload or the control information field length.

   __int32 getLength() const;

      // Functionality:
      //    Set the payload or the control information field length.
      // Parameters:
      //    0) [in] len: the payload or the control information field length.
      // Returned value:
      //    None.

   void setLength(const __int32& len);

      // Functionality:
      //    Pack a Control packet.
      // Parameters:
      //    0) [in] pkttype: packet type filed.
      //    1) [in] lparam: pointer to the first data structure, explained by the packet type.
      //    2) [in] rparam: pointer to the second data structure, explained by the packet type.
      //    3) [in] size: size of rparam, in number of bytes;
      // Returned value:
      //    None.

   void pack(const __int32& pkttype, void* lparam = NULL, void* rparam = NULL, const __int32& size = 0);

      // Functionality:
      //    Read the packet vector.
      // Parameters:
      //    None.
      // Returned value:
      //    Pointer to the packet vector.

   iovec* getPacketVector();

      // Functionality:
      //    Read the packet flag.
      // Parameters:
      //    None.
      // Returned value:
      //    packet flag (0 or 1).

   __int32 getFlag() const;

      // Functionality:
      //    Read the packet type.
      // Parameters:
      //    None.
      // Returned value:
      //    packet type filed (000 ~ 111).

   __int32 getType() const;

      // Functionality:
      //    Read the extended packet type.
      // Parameters:
      //    None.
      // Returned value:
      //    extended packet type filed (0x000 ~ 0xFFF).

   __int32 getExtendedType() const;

      // Functionality:
      //    Read the ACK-2 seq. no.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field (bit 16~31).

   __int32 getAckSeqNo() const;

      // Functionality:
      //    Read the message boundary flag bit.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 0~1).

   __int32 getMsgBoundary() const;

      // Functionality:
      //    Read the message inorder delivery flag bit.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 2).

   __int32 getMsgOrderFlag() const;

      // Functionality:
      //    Read the message sequence number.
      // Parameters:
      //    None.
      // Returned value:
      //    packet header field [1] (bit 3~31).

   __int32 getMsgSeq() const;

protected:
   unsigned __int32 m_nHeader[3];       // The 96-bit header field
   iovec m_PacketVector[2];             // The 2-demension vector of UDT packet [header, data]

   __int32 __pad;

   void operator = (const CPacket&) {}
};

////////////////////////////////////////////////////////////////////////////////

struct CHandShake
{
   __int32 m_iVersion;          // UDT version
   __int32 m_iType;             // UDT socket type
   __int32 m_iISN;              // random initial sequence number
   __int32 m_iMSS;              // maximum segment size
   __int32 m_iFlightFlagSize;   // flow control window size
   __int32 m_iReqType;          // connection request type: -1: response, 1: initial request, 0: rendezvous request
};


#endif
