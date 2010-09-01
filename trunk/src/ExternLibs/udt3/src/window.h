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
This header file contains the definition of Window structures used in UDT.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 02/14/2006
*****************************************************************************/

#ifndef __UDT_WINDOW_H__
#define __UDT_WINDOW_H__


#ifndef WIN32
   #include <sys/time.h>
   #include <time.h>
#endif
#include "udt.h"


class CACKWindow
{
public:
   CACKWindow();
   CACKWindow(const __int32& size);
   ~CACKWindow();

      // Functionality:
      //    Write an ACK record into the window.
      // Parameters:
      //    0) [in] seq: ACK seq. no.
      //    1) [in] ack: DATA ACK no.
      // Returned value:
      //    None.

   void store(const __int32& seq, const __int32& ack);

      // Functionality:
      //    Search the ACK-2 "seq" in the window, find out the DATA "ack" and caluclate RTT .
      // Parameters:
      //    0) [in] seq: ACK-2 seq. no.
      //    1) [out] ack: the DATA ACK no. that matches the ACK-2 no.
      // Returned value:
      //    RTT.

   __int32 acknowledge(const __int32& seq, __int32& ack);

private:
   __int32* m_piACKSeqNo;       // Seq. No. for the ACK packet
   __int32* m_piACK;            // Data Seq. No. carried by the ACK packet
   timeval* m_pTimeStamp;       // The timestamp when the ACK was sent

   __int32 m_iSize;             // Size of the ACK history window
   __int32 m_iHead;             // Pointer to the lastest ACK record
   __int32 m_iTail;             // Pointer to the oldest ACK record
};

////////////////////////////////////////////////////////////////////////////////

class CPktTimeWindow
{
public:
   CPktTimeWindow();
   CPktTimeWindow(const __int32& s1, const __int32& s2, const __int32& s3);
   ~CPktTimeWindow();

      // Functionality:
      //    read the minimum packet sending interval.
      // Parameters:
      //    None.
      // Returned value:
      //    minimum packet sending interval (microseconds).

   __int32 getMinPktSndInt() const;

      // Functionality:
      //    Calculate the packes arrival speed.
      // Parameters:
      //    None.
      // Returned value:
      //    Packet arrival speed (packets per second).

   __int32 getPktRcvSpeed() const;

      // Functionality:
      //    Check if the rtt is increasing or not.
      // Parameters:
      //    None.
      // Returned value:
      //    true is RTT is increasing, otherwise false.

   bool getDelayTrend() const;

      // Functionality:
      //    Estimate the bandwidth.
      // Parameters:
      //    None.
      // Returned value:
      //    Estimated bandwidth (packets per second).

   __int32 getBandwidth() const;

      // Functionality:
      //    Record time information of a packet sending.
      // Parameters:
      //    0) currtime: time stamp of the packet sending.
      // Returned value:
      //    None.

   void onPktSent(const timeval& currtime);

      // Functionality:
      //    Record time information of an arrived packet.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   void onPktArrival();

      // Functionality:
      //    Record the recent RTT.
      // Parameters:
      //    0) [in] rtt: the mose recent RTT from ACK-2.
      // Returned value:
      //    None.

   void ack2Arrival(const __int32& rtt);

      // Functionality:
      //    Record the arrival time of the first probing packet.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   void probe1Arrival();

      // Functionality:
      //    Record the arrival time of the second probing packet and the interval between packet pairs.
      // Parameters:
      //    None.
      // Returned value:
      //    None.

   void probe2Arrival();

private:
   __int32 m_iAWSize;           // size of the packet arrival history window
   __int32* m_piPktWindow;      // packet information window
   __int32 m_iPktWindowPtr;     // position pointer of the packet info. window.

   __int32 m_iRWSize;           // size of RTT history window size
   __int32* m_piRTTWindow;      // RTT history window
   __int32* m_piPCTWindow;      // PCT (pairwise comparison test) history window
   __int32* m_piPDTWindow;      // PDT (pairwise difference test) history window
   __int32 m_iRTTWindowPtr;     // position pointer to the 3 windows above

   __int32 m_iPWSize;           // size of probe history window size
   __int32* m_piProbeWindow;    // record inter-packet time for probing packet pairs
   __int32 m_iProbeWindowPtr;   // position pointer to the probing window

   timeval m_LastSentTime;      // last packet sending time
   __int32 m_iMinPktSndInt;     // Minimum packet sending interval

   timeval m_LastArrTime;       // last packet arrival time
   timeval m_CurrArrTime;       // current packet arrival time
   timeval m_ProbeTime;         // arrival time of the first probing packet
};


#endif
