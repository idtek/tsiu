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
This file contains the implementation of UDT sending and receiving buffer
management modules.

The sending buffer is a linked list of application data to be sent.
The receiving buffer is a logically circular memeory block.
*****************************************************************************/

/*****************************************************************************
written by
   Yunhong Gu [gu@lac.uic.edu], last updated 02/14/2006
*****************************************************************************/

#include <cstring>
#include <cmath>
#include "common.h"
#include "buffer.h"


CSndBuffer::CSndBuffer(const __int32& mss):
m_pBlock(NULL),
m_pLastBlock(NULL),
m_pCurrSendBlk(NULL),
m_pCurrAckBlk(NULL),
m_iCurrBufSize(0),
m_iCurrSendPnt(0),
m_iCurrAckPnt(0),
m_iNextMsgNo(0),
m_iMSS(mss)
{
   #ifndef WIN32
      pthread_mutex_init(&m_BufLock, NULL);
   #else
      m_BufLock = CreateMutex(NULL, false, NULL);
   #endif
}

CSndBuffer::~CSndBuffer()
{
   Block* pb = m_pBlock;

   // Release allocated data structure if there is any
   while (NULL != m_pBlock)
   {
      pb = pb->m_next;

      // process user data according with the routine provided by applications
      if (NULL != m_pBlock->m_pMemRoutine)
         m_pBlock->m_pMemRoutine(m_pBlock->m_pcData, m_pBlock->m_iLength);

      delete m_pBlock;
      m_pBlock = pb;
   }

   #ifndef WIN32
      pthread_mutex_destroy(&m_BufLock);
   #else
      CloseHandle(m_BufLock);
   #endif
}

void CSndBuffer::addBuffer(const char* data, const __int32& len, const __int32& handle, const UDT_MEM_ROUTINE func, const __int32& ttl, const __int32& seqno, const bool& order)
{
   CGuard bufferguard(m_BufLock);

   if (NULL == m_pBlock)
   {
      // Insert a block to the empty list   
  
      m_pBlock = new Block;
      m_pBlock->m_pcData = const_cast<char *>(data);
      m_pBlock->m_iLength = len;
      gettimeofday(&m_pBlock->m_OriginTime, 0);
      m_pBlock->m_iTTL = ttl;
      m_pBlock->m_iMsgNo = m_iNextMsgNo;
      m_pBlock->m_iSeqNo = seqno;
      m_pBlock->m_iInOrder = order;
      m_pBlock->m_iInOrder <<= 29;
      m_pBlock->m_iHandle = handle;
      m_pBlock->m_pMemRoutine = func;
      m_pBlock->m_next = NULL;
      m_pLastBlock = m_pBlock;
      m_pCurrSendBlk = m_pBlock;
      m_iCurrSendPnt = 0;
      m_pCurrAckBlk = m_pBlock;
      m_iCurrAckPnt = 0;
   }
   else
   {
      // Insert a new block to the tail of the list

      __int32 lastseq = m_pLastBlock->m_iSeqNo;
      __int32 offset = m_pLastBlock->m_iLength;

      m_pLastBlock->m_next = new Block;
      m_pLastBlock = m_pLastBlock->m_next;
      m_pLastBlock->m_pcData = const_cast<char *>(data);
      m_pLastBlock->m_iLength = len;
      gettimeofday(&m_pLastBlock->m_OriginTime, 0);
      m_pLastBlock->m_iTTL = ttl;
      m_pLastBlock->m_iMsgNo = m_iNextMsgNo;
      m_pLastBlock->m_iSeqNo = lastseq + (__int32)ceil(double(offset) / m_iMSS);
      m_pLastBlock->m_iInOrder = order;
      m_pLastBlock->m_iInOrder <<= 29;
      m_pLastBlock->m_iHandle = handle;
      m_pLastBlock->m_pMemRoutine = func;
      m_pLastBlock->m_next = NULL;
      if (NULL == m_pCurrSendBlk)
         m_pCurrSendBlk = m_pLastBlock;
   }

   m_iCurrBufSize += len;

   m_iNextMsgNo = CMsgNo::incmsg(m_iNextMsgNo);
}

__int32 CSndBuffer::readData(char** data, const __int32& len, __int32& msgno)
{
   CGuard bufferguard(m_BufLock);

   // No data to read
   if (NULL == m_pCurrSendBlk)
      return 0;

   // read data in the current sending block
   if (m_iCurrSendPnt + len < m_pCurrSendBlk->m_iLength)
   {
      *data = m_pCurrSendBlk->m_pcData + m_iCurrSendPnt;

      msgno = m_pCurrSendBlk->m_iMsgNo | m_pCurrSendBlk->m_iInOrder;
      if (0 == m_iCurrSendPnt)
         msgno |= 0x80000000;
      if (m_pCurrSendBlk->m_iLength == m_iCurrSendPnt + len)
         msgno |= 0x40000000;

      m_iCurrSendPnt += len;

      return len;
   }

   // Not enough data to read. 
   // Read an irregular packet and move the current sending block pointer to the next block
   __int32 readlen = m_pCurrSendBlk->m_iLength - m_iCurrSendPnt;
   *data = m_pCurrSendBlk->m_pcData + m_iCurrSendPnt;

   if (0 == m_iCurrSendPnt)
      msgno = m_pCurrSendBlk->m_iMsgNo | 0xC0000000 | m_pCurrSendBlk->m_iInOrder;
   else
      msgno = m_pCurrSendBlk->m_iMsgNo | 0x40000000 | m_pCurrSendBlk->m_iInOrder;

   m_pCurrSendBlk = m_pCurrSendBlk->m_next;
   m_iCurrSendPnt = 0;

   return readlen;
}

__int32 CSndBuffer::readData(char** data, const __int32 offset, const __int32& len, __int32& msgno, __int32& seqno, __int32& msglen)
{
   CGuard bufferguard(m_BufLock);

   Block* p = m_pCurrAckBlk;

   // No data to read
   if (NULL == p)
      return 0;

   // Locate to the data position by the offset
   __int32 loffset = offset + m_iCurrAckPnt;
   while (p->m_iLength <= loffset)
   {
      loffset -= p->m_iLength;
      loffset -= len - ((0 == p->m_iLength % len) ? len : (p->m_iLength % len));
      p = p->m_next;
      if (NULL == p)
         return 0;
   }

   if (p->m_iTTL >= 0)
   {
      timeval currtime;
      gettimeofday(&currtime, 0);

      __int32 e = (currtime.tv_sec - p->m_OriginTime.tv_sec) * 1000000 + currtime.tv_usec - p->m_OriginTime.tv_usec;

      if (e > p->m_iTTL)
      {
         msgno = p->m_iMsgNo;
         seqno = p->m_iSeqNo;
         msglen = p->m_iLength;

         return -1;
      }
   }

   // Read a regular data
   if (loffset + len <= p->m_iLength)
   {
      *data = p->m_pcData + loffset;
      msgno = p->m_iMsgNo | p->m_iInOrder;

      if (0 == loffset)
         msgno |= 0x80000000;
      if (p->m_iLength == loffset + len)
         msgno |= 0x40000000;

      return len;
   }

   // Read an irrugular data at the end of a block
   *data = p->m_pcData + loffset;
   msgno = p->m_iMsgNo | p->m_iInOrder;

   if (0 == loffset)
      msgno |= 0xC0000000;
   else
      msgno |= 0x40000000;

   return p->m_iLength - loffset;
}

void CSndBuffer::ackData(const __int32& len, const __int32& payloadsize)
{
   CGuard bufferguard(m_BufLock);

   m_iCurrAckPnt += len;

   // Remove the block if it is acknowledged
   while (m_iCurrAckPnt >= m_pCurrAckBlk->m_iLength)
   {
      m_iCurrAckPnt -= m_pCurrAckBlk->m_iLength;

      // Update the size error between regular and irregular packets
      if (0 != m_pCurrAckBlk->m_iLength % payloadsize)
         m_iCurrAckPnt -= payloadsize - m_pCurrAckBlk->m_iLength % payloadsize;

      m_iCurrBufSize -= m_pCurrAckBlk->m_iLength;
      m_pCurrAckBlk = m_pCurrAckBlk->m_next;

      // process user data according with the routine provided by applications
      if (NULL != m_pBlock->m_pMemRoutine)
         m_pBlock->m_pMemRoutine(m_pBlock->m_pcData, m_pBlock->m_iLength);

      delete m_pBlock;
      m_pBlock = m_pCurrAckBlk;

      if (NULL == m_pBlock)
         break;
   }
}

__int32 CSndBuffer::getCurrBufSize() const
{
   return m_iCurrBufSize - m_iCurrAckPnt;
}

bool CSndBuffer::getOverlappedResult(const __int32& handle, __int32& progress)
{
   CGuard bufferguard(m_BufLock);

   if (NULL != m_pCurrAckBlk)
   {
      if (handle == m_pCurrAckBlk->m_iHandle)
      {
         progress = m_iCurrAckPnt;
         return false;
      }
      else 
      {
         __int32 end = (m_pLastBlock->m_iHandle >= m_pCurrAckBlk->m_iHandle) ? m_pLastBlock->m_iHandle : m_pLastBlock->m_iHandle + (1 << 30);
         __int32 h = (handle >= m_pCurrAckBlk->m_iHandle) ? handle : handle + (1 << 30);

         if ((h > m_pCurrAckBlk->m_iHandle) && (h <= end))
         {
            progress = 0;
            return false;
         }
      }
   }

   progress = 0;
   return true;
}

void CSndBuffer::releaseBuffer(char* buf, int)
{
   delete [] buf;
}

////////////////////////////////////////////////////////////////////////////////

CRcvBuffer::CRcvBuffer(const __int32& mss):
m_pcData(NULL),
m_iSize(40960000),
m_iStartPos(0),
m_iLastAckPos(0),
m_iMaxOffset(0),
m_pcUserBuf(NULL),
m_iUserBufSize(0),
m_pPendingBlock(NULL),
m_pLastBlock(NULL),
m_iPendingSize(0),
m_pMessageList(NULL),
m_iMSS(mss)
{
   m_pcData = new char [m_iSize];

   #ifndef WIN32
      pthread_mutex_init(&m_MsgLock, NULL);
   #else
      m_MsgLock = CreateMutex(NULL, false, NULL);
   #endif
}

CRcvBuffer::CRcvBuffer(const __int32& mss, const __int32& bufsize):
m_pcData(NULL),
m_iSize(bufsize),
m_iStartPos(0),
m_iLastAckPos(0),
m_iMaxOffset(0),
m_pcUserBuf(NULL),
m_iUserBufSize(0),
m_pPendingBlock(NULL),
m_pLastBlock(NULL),
m_iPendingSize(0),
m_pMessageList(NULL),
m_iMSS(mss)
{
   m_pcData = new char [m_iSize];

   #ifndef WIN32
      pthread_mutex_init(&m_MsgLock, NULL);
   #else
      m_MsgLock = CreateMutex(NULL, false, NULL);
   #endif
}

CRcvBuffer::~CRcvBuffer()
{
   delete [] m_pcData;

   Block* p = m_pPendingBlock;

   while (NULL != p)
   {
     m_pPendingBlock = m_pPendingBlock->m_next;
     delete p;
     p = m_pPendingBlock;
   }

   if (NULL != m_pMessageList)
      delete [] m_pMessageList;

   #ifndef WIN32
      pthread_mutex_destroy(&m_MsgLock);
   #else
      CloseHandle(m_MsgLock);
   #endif
}

bool CRcvBuffer::nextDataPos(char** data, __int32 offset, const __int32& len)
{
   // Search the user data block first
   if (NULL != m_pcUserBuf)
   {
      if (m_iUserBufAck + offset + len <= m_iUserBufSize)
      {
         // find a position in user buffer
         *data = m_pcUserBuf + m_iUserBufAck + offset;
         return true;
      }
      else if (m_iUserBufAck + offset < m_iUserBufSize)
      {
         // Meet the end of the user buffer and there is not enough space for a regular packet
         return false;
      }
      else
         // offset is larger than user buffer size
         offset -= m_iUserBufSize - m_iUserBufAck;
   }

   // Remember the position of the furthest "dirty" data
   __int32 origoff = m_iMaxOffset;
   if (offset + len > m_iMaxOffset)
      m_iMaxOffset = offset + len;

   if (m_iLastAckPos >= m_iStartPos)
      if (m_iLastAckPos + offset + len <= m_iSize)
      {
         *data = m_pcData + m_iLastAckPos + offset;
         return true;
      }
      else if ((m_iLastAckPos + offset > m_iSize) && (offset - (m_iSize - m_iLastAckPos) + len <= m_iStartPos))
      {
         *data = m_pcData + offset - (m_iSize - m_iLastAckPos);
         return true;
      }

   if (m_iLastAckPos + offset + len <= m_iStartPos)
   {
      *data = m_pcData + m_iLastAckPos + offset;
      return true;
   }

   // recover this pointer if no space is found
   m_iMaxOffset = origoff;

   return false;
}

bool CRcvBuffer::addData(char** data, __int32 offset, __int32 len)
{
   // Check the user buffer first
   if (NULL != m_pcUserBuf)
   {
      if (m_iUserBufAck + offset + len <= m_iUserBufSize)
      {
         // write data into the user buffer
         memcpy(m_pcUserBuf + m_iUserBufAck + offset, *data, len);
         return true;
      }
      else if (m_iUserBufAck + offset < m_iUserBufSize)
      {
         // write part of the data to the user buffer
         memcpy(m_pcUserBuf + m_iUserBufAck + offset, *data, m_iUserBufSize - (m_iUserBufAck + offset));
         *data += m_iUserBufSize - (m_iUserBufAck + offset);
         len -= m_iUserBufSize - (m_iUserBufAck + offset);
         offset = 0;
      }
      else
         // offset is larger than size of user buffer
         offset -= m_iUserBufSize - m_iUserBufAck;
   }

   // Record this value in case that the method is failed
   __int32 origoff = m_iMaxOffset;
   if (offset + len > m_iMaxOffset)
      m_iMaxOffset = offset + len;

   if (m_iLastAckPos >= m_iStartPos)
      if (m_iLastAckPos + offset + len <= m_iSize)
      {
         memcpy(m_pcData + m_iLastAckPos + offset, *data, len);
         *data = m_pcData + m_iLastAckPos + offset;
         return true;
      }
      else if ((m_iLastAckPos + offset < m_iSize) && (len - (m_iSize - m_iLastAckPos - offset) <= m_iStartPos))
      {
         memcpy(m_pcData + m_iLastAckPos + offset, *data, m_iSize - m_iLastAckPos - offset);
         memcpy(m_pcData, *data + m_iSize - m_iLastAckPos - offset, len - (m_iSize - m_iLastAckPos - offset));
         *data = m_pcData + m_iLastAckPos + offset;
         return true;
      }
      else if ((m_iLastAckPos + offset >= m_iSize) && (offset - (m_iSize - m_iLastAckPos) + len <= m_iStartPos))
      {
         memcpy(m_pcData + offset - (m_iSize - m_iLastAckPos), *data, len);
         *data = m_pcData + offset - (m_iSize - m_iLastAckPos);
         return true;
      }

   if (m_iLastAckPos + offset + len <= m_iStartPos)
   {
      memcpy(m_pcData + m_iLastAckPos + offset, *data, len);
      *data = m_pcData + m_iLastAckPos + offset;
      return true;
   }

   // recover the offset pointer since the write is failed
   m_iMaxOffset = origoff;

   return false;
}

void CRcvBuffer::moveData(__int32 offset, const __int32& len)
{
   // check the user buffer first
   if (NULL != m_pcUserBuf)
   {
      if (m_iUserBufAck + offset + len < m_iUserBufSize)
      {
         // move data in user buffer
         memmove(m_pcUserBuf + m_iUserBufAck + offset, m_pcUserBuf + m_iUserBufAck + offset + len, m_iUserBufSize - (m_iUserBufAck + offset + len));

         // move data from protocol buffer
         if (m_iMaxOffset > 0)
         {
            __int32 reallen = len;
            if (m_iMaxOffset < len)
               reallen = m_iMaxOffset;

            if (m_iSize < m_iLastAckPos + reallen)
            {
               memcpy(m_pcUserBuf + m_iUserBufSize - len, m_pcData + m_iLastAckPos, m_iSize - m_iLastAckPos);
               memcpy(m_pcUserBuf + m_iUserBufSize - len + m_iSize - m_iLastAckPos, m_pcData, m_iLastAckPos + reallen - m_iSize);
            }
            else
               memcpy(m_pcUserBuf + m_iUserBufSize - len, m_pcData + m_iLastAckPos, reallen);
         }

         offset = 0; 
      }
      else if (m_iUserBufAck + offset < m_iUserBufSize)
      {
         if (m_iMaxOffset > m_iUserBufAck + offset + len - m_iUserBufSize)
         {
            __int32 reallen = m_iUserBufSize - (m_iUserBufAck + offset);
            __int32 startpos = m_iLastAckPos + len - reallen;
            if (m_iMaxOffset < len)
               reallen -= len - m_iMaxOffset;

            // Be sure that the m_iSize is at least 1 packet size, whereas len cannot be greater than this value, checked in setOpt().
            if (m_iSize < startpos)
               memcpy(m_pcUserBuf + m_iUserBufAck + offset, m_pcData + startpos - m_iSize, reallen);
            else if (m_iSize < startpos + reallen)
            {
               memcpy(m_pcUserBuf + m_iUserBufAck + offset, m_pcData + startpos, m_iSize - startpos);
               memcpy(m_pcUserBuf + m_iUserBufAck + offset + m_iSize - startpos, m_pcData, startpos + reallen - m_iSize);
            }
            else
               memcpy(m_pcUserBuf + m_iUserBufAck + offset, m_pcData + startpos, reallen);
         }

         offset = 0;
      }
      else
         // offset is larger than size of user buffer
         offset -= m_iUserBufSize - m_iUserBufAck;
   }

   // No data to move
   if (m_iMaxOffset - offset < len)
   {
      m_iMaxOffset = offset;
      return;
   }

   // Move data in protocol buffer.
   if (m_iLastAckPos + m_iMaxOffset <= m_iSize)
      memmove(m_pcData + m_iLastAckPos + offset, m_pcData + m_iLastAckPos + offset + len, m_iMaxOffset - offset - len);
   else if (m_iLastAckPos + offset > m_iSize)
      memmove(m_pcData + (m_iLastAckPos + offset) % m_iSize, m_pcData + (m_iLastAckPos + offset + len) % m_iSize, m_iMaxOffset - offset - len);
   else if (m_iLastAckPos + offset + len <= m_iSize)
   {
      memmove(m_pcData + m_iLastAckPos + offset, m_pcData + m_iLastAckPos + offset + len, m_iSize - m_iLastAckPos - offset - len);
      memmove(m_pcData + m_iSize - len, m_pcData, len);
      memmove(m_pcData, m_pcData + len, m_iLastAckPos + m_iMaxOffset - m_iSize - len);
   }
   else
   {
      memmove(m_pcData + m_iLastAckPos + offset, m_pcData + len - (m_iSize - m_iLastAckPos - offset), m_iSize - m_iLastAckPos - offset);
      memmove(m_pcData, m_pcData + len, m_iLastAckPos + m_iMaxOffset - m_iSize - len);
   }

   // Update the offset pointer
   m_iMaxOffset -= len;
}

bool CRcvBuffer::readBuffer(char* data, const __int32& len)
{
   if (m_iStartPos + len <= m_iLastAckPos)
   {
      // Simplest situation, read "len" data from start position
      memcpy(data, m_pcData + m_iStartPos, len);
      m_iStartPos += len;
      return true;
   }
   else if (m_iLastAckPos < m_iStartPos)
   {
      if (m_iStartPos + len < m_iSize)
      {
         // Data is not cover the ohysical boundary of the buffer
         memcpy(data, m_pcData + m_iStartPos, len);
         m_iStartPos += len;
         return true;
      }
      if (len - (m_iSize - m_iStartPos) <= m_iLastAckPos)
      {
         // data length exceeds the physical boundary, read twice
         memcpy(data, m_pcData + m_iStartPos, m_iSize - m_iStartPos);
         memcpy(data + m_iSize - m_iStartPos, m_pcData, len - (m_iSize - m_iStartPos));
         m_iStartPos = len - (m_iSize - m_iStartPos);
         return true;
      }
   }

   // Not enough data to read
   return false;
}

__int32 CRcvBuffer::ackData(const __int32& len)
{
   __int32 ret = 0;

   if (NULL != m_pcUserBuf)
      if (m_iUserBufAck + len < m_iUserBufSize)
      {
         // update user buffer ACK pointer
         m_iUserBufAck += len;
         return 0;
      }
      else
      {
         // user buffer is fulfilled
         // update protocol ACK pointer
         m_iLastAckPos += m_iUserBufAck + len - m_iUserBufSize;
         m_iMaxOffset -= m_iUserBufAck + len - m_iUserBufSize;

         // the overlapped IO is completed, a pending buffer should be activated
         m_pcUserBuf = NULL;
         m_iUserBufSize = 0;
         if (NULL != m_pPendingBlock)
         {
            registerUserBuf(m_pPendingBlock->m_pcData, m_pPendingBlock->m_iLength, m_pPendingBlock->m_iHandle, m_pPendingBlock->m_pMemRoutine);
            m_iPendingSize -= m_pPendingBlock->m_iLength;
            m_pPendingBlock = m_pPendingBlock->m_next;
            if (NULL == m_pPendingBlock)
               m_pLastBlock = NULL;
         }

         // returned value is 1 means user buffer is fulfilled
         ret = 1;
      }
   else
   {
      // there is no user buffer
      m_iLastAckPos += len;
      m_iMaxOffset -= len;
   }

   m_iLastAckPos %= m_iSize;

   return ret;
}

__int32 CRcvBuffer::registerUserBuf(char* buf, const __int32& len, const __int32& handle, const UDT_MEM_ROUTINE func)
{
   if (NULL != m_pcUserBuf)
   {
      // there is ongoing recv, new buffer is put into pending list.

      Block *nb = new Block;
      nb->m_pcData = buf;
      nb->m_iLength = len;
      nb->m_iHandle = handle;
      nb->m_pMemRoutine = func;
      nb->m_next = NULL;

      if (NULL == m_pPendingBlock)
         m_pLastBlock = m_pPendingBlock = nb;
      else
         m_pLastBlock->m_next = nb;

      m_iPendingSize += len;

      return 0;
   }

   m_iUserBufAck = 0;
   m_iUserBufSize = len;
   m_pcUserBuf = buf;
   m_iHandle = handle;

   // find the furthest "dirty" data that need to be copied
   __int32 currwritepos = (m_iLastAckPos + m_iMaxOffset) % m_iSize;

   // copy data from protocol buffer into user buffer
   if (m_iStartPos <= currwritepos)
      if (currwritepos - m_iStartPos <= len)
      {
         memcpy(m_pcUserBuf, m_pcData + m_iStartPos, currwritepos - m_iStartPos);
         m_iMaxOffset = 0;
      }
      else
      {
         memcpy(m_pcUserBuf, m_pcData + m_iStartPos, len);
         m_iMaxOffset -= len;
      }
   else
      if (m_iSize - (m_iStartPos - currwritepos) <= len)
      {
         memcpy(m_pcUserBuf, m_pcData + m_iStartPos, m_iSize - m_iStartPos);
         memcpy(m_pcUserBuf + m_iSize - m_iStartPos, m_pcData, currwritepos);
         m_iMaxOffset = 0;
      }
      else
      {
         if (m_iSize - m_iStartPos <= len)
         {
            memcpy(m_pcUserBuf, m_pcData + m_iStartPos, m_iSize - m_iStartPos);
            memcpy(m_pcUserBuf + m_iSize - m_iStartPos, m_pcData, len - (m_iSize - m_iStartPos));
         }
         else
            memcpy(m_pcUserBuf, m_pcData + m_iStartPos, len);
         m_iMaxOffset -= len;
      }

   // Update the user buffer pointer
   if (m_iStartPos <= m_iLastAckPos)
      m_iUserBufAck += m_iLastAckPos - m_iStartPos;
   else
      m_iUserBufAck += m_iSize - m_iStartPos + m_iLastAckPos;

   // update the protocol buffer pointer
   m_iStartPos = (m_iStartPos + len) % m_iSize;
   m_iLastAckPos = m_iStartPos;

   return m_iUserBufAck;
}

void CRcvBuffer::removeUserBuf()
{
   m_pcUserBuf = NULL;
   m_iUserBufAck = 0;
}

__int32 CRcvBuffer::getAvailBufSize() const
{
   __int32 bs = m_iSize;

   bs -= m_iLastAckPos - m_iStartPos;

   if (m_iLastAckPos < m_iStartPos)
      bs -= m_iSize;

   if (NULL != m_pcUserBuf)
      bs += m_iUserBufSize - m_iUserBufAck;

   return bs;
}

__int32 CRcvBuffer::getRcvDataSize() const
{
   return (m_iLastAckPos - m_iStartPos + m_iSize) % m_iSize;
}

bool CRcvBuffer::getOverlappedResult(const __int32& handle, __int32& progress)
{
   if ((NULL != m_pcUserBuf) && (handle == m_iHandle))
   {
      progress = m_iUserBufAck;
      return false;
   }

   progress = 0;

   if (NULL != m_pPendingBlock)
   {
      __int32 end = (m_pLastBlock->m_iHandle >= m_pPendingBlock->m_iHandle) ? m_pLastBlock->m_iHandle : m_pLastBlock->m_iHandle + (1 << 30);
      __int32 h = (handle >= m_pPendingBlock->m_iHandle) ? handle : handle + (1 << 30);

      if ((h >= m_pPendingBlock->m_iHandle) && (h <= end))
         return false;
   }

   return true;
}

__int32 CRcvBuffer::getPendingQueueSize() const
{
   return m_iPendingSize + m_iUserBufSize;
}

void CRcvBuffer::initMsgList()
{
   // the message list should contain the most possible number of messages: when each packet is a message
   m_iMsgInfoSize = m_iSize / m_iMSS + 1;

   m_pMessageList = new MsgInfo[m_iMsgInfoSize];

   m_iPtrFirstMsg = -1;
   m_iPtrRecentACK = -1;
   m_iLastMsgNo = 0;
   m_iValidMsgCount = 0;

   for (int i = 0; i < m_iMsgInfoSize; ++ i)
   {
      m_pMessageList[i].m_pcData = NULL;
      m_pMessageList[i].m_iMsgNo = -1;
      m_pMessageList[i].m_iStartSeq = -1;
      m_pMessageList[i].m_iEndSeq = -1;
      m_pMessageList[i].m_iSizeDiff = 0;
      m_pMessageList[i].m_bValid = false;
      m_pMessageList[i].m_bDropped = false;
      m_pMessageList[i].m_bInOrder = false;
      m_pMessageList[i].m_iMsgNo = -1;
   }
}

void CRcvBuffer::checkMsg(const __int32& type, const __int32& msgno, const __int32& seqno, const char* ptr, const bool& inorder, const __int32& diff)
{
   CGuard msgguard(m_MsgLock);

   __int32 pos;

   if (-1 == m_iPtrFirstMsg)
   {
      pos = m_iPtrFirstMsg = 0;
      m_iPtrRecentACK = -1;
   }
   else
   {
      pos = m_iPtrFirstMsg + CMsgNo::msgoff(m_pMessageList[m_iPtrFirstMsg].m_iMsgNo, msgno);

      if (pos >= m_iMsgInfoSize)
         pos -= m_iMsgInfoSize;
      else if (pos < 0)
      {
         pos += m_iMsgInfoSize;
         m_iPtrFirstMsg = pos;
      }
   }

   MsgInfo* p = m_pMessageList + pos;

   p->m_iMsgNo = msgno;

   switch (type)
   {
   case 3:
      // single packet message
      p->m_pcData = (char*)ptr;
      p->m_iStartSeq = p->m_iEndSeq = seqno;
      p->m_bInOrder = inorder;
      p->m_iSizeDiff = diff;

      break;

   case 2:
      // first packet of the message
      p->m_pcData = (char*)ptr;
      p->m_iStartSeq = seqno;
      p->m_bInOrder = inorder;

      break;

   case 1:
      // last packet of the message
      p->m_iEndSeq = seqno;
      p->m_iSizeDiff = diff;

      break;
   }

   // update the largest msg no so far
   if (CMsgNo::msgcmp(m_iLastMsgNo, msgno) < 0)
      m_iLastMsgNo = msgno;
}

bool CRcvBuffer::ackMsg(const __int32& ack, const CRcvLossList* rll)
{
   CGuard msgguard(m_MsgLock);

   // no message exist, return
   if (-1 == m_iPtrFirstMsg)
   {
      // also means no message is valid

      m_iStartPos = m_iLastAckPos;

      return false;
   }

   __int32 ptr;
   __int32 len;

   if (-1 == m_iPtrRecentACK)
   {
      // all messages are new, check from the start
      ptr = m_iPtrFirstMsg;
      len = CMsgNo::msglen(m_pMessageList[ptr].m_iMsgNo, m_iLastMsgNo);
   }
   else
   {
      // check from the last ACK point
      ptr = m_iPtrRecentACK + 1;

      if (ptr == m_iMsgInfoSize)
         ptr = 0;

      len = CMsgNo::msglen(m_pMessageList[ptr].m_iMsgNo, m_iLastMsgNo);
   }

   for (__int32 i = 0; i < len; ++ i)
   {
      if ((m_pMessageList[ptr].m_iStartSeq != -1) &&
          (m_pMessageList[ptr].m_iEndSeq != -1) &&
          (!m_pMessageList[ptr].m_bDropped) &&
          (!(rll->find(m_pMessageList[ptr].m_iStartSeq, m_pMessageList[ptr].m_iEndSeq))) &&
          (!m_pMessageList[ptr].m_bInOrder || CSeqNo::seqcmp(m_pMessageList[ptr].m_iEndSeq, ack) <= 0))
      {
         m_pMessageList[ptr].m_bValid = true;
         m_pMessageList[ptr].m_iLength = CSeqNo::seqlen(m_pMessageList[ptr].m_iStartSeq, m_pMessageList[ptr].m_iEndSeq) * m_iMSS - m_pMessageList[ptr].m_iSizeDiff;

         ++ m_iValidMsgCount;
      }

      if ((m_pMessageList[ptr].m_iEndSeq != -1) && (CSeqNo::seqcmp(m_pMessageList[ptr].m_iEndSeq, ack) <= 0))
         m_iPtrRecentACK = ptr;

      ++ ptr;

      if (ptr == m_iMsgInfoSize)
         ptr = 0;
   }

   return (m_iValidMsgCount > 0);
}

void CRcvBuffer::dropMsg(const __int32& msgno)
{
   CGuard msgguard(m_MsgLock);

   // no message exist, return
   if (-1 == m_iPtrFirstMsg)
      return;

   __int32 ptr = m_iPtrFirstMsg + CMsgNo::msglen(m_pMessageList[m_iPtrFirstMsg].m_iMsgNo, msgno);
   if (ptr >= m_iMsgInfoSize)
      ptr -= m_iMsgInfoSize;

   m_pMessageList[ptr].m_iMsgNo = msgno;
   m_pMessageList[ptr].m_bDropped = true;

   // update the largest msg no so far
   if (CMsgNo::msgcmp(m_iLastMsgNo, msgno) < 0)
      m_iLastMsgNo = msgno;
}

__int32 CRcvBuffer::readMsg(char* data, const __int32& len)
{
   CGuard msgguard(m_MsgLock);

   // no message exist, return
   if (-1 == m_iPtrFirstMsg)
      return 0;

   __int32 ptr = m_iPtrFirstMsg;

   // searching first valid message
   while (m_pMessageList[ptr].m_iMsgNo != m_iLastMsgNo)
   {
      if (m_pMessageList[ptr].m_bValid)
         break;

      ++ ptr;
      if (ptr == m_iMsgInfoSize)
         ptr = 0;
   }

   __int32 size = 0;

   if (m_pMessageList[ptr].m_bValid)
   {
      if ((m_pMessageList[ptr].m_bInOrder) || (CSeqNo::seqcmp(m_pMessageList[ptr].m_iEndSeq, m_pMessageList[m_iPtrRecentACK].m_iEndSeq) <= 0))
      {
         m_iStartPos = m_pMessageList[ptr].m_pcData + CSeqNo::seqlen(m_pMessageList[ptr].m_iStartSeq, m_pMessageList[ptr].m_iEndSeq) * m_iMSS - m_pcData;
         if (m_iStartPos > m_iSize)
            m_iStartPos -= m_iSize;
      }
      else
      {
         if (NULL != m_pMessageList[m_iPtrRecentACK].m_pcData)
            m_iStartPos = m_pMessageList[m_iPtrRecentACK].m_pcData - m_pcData;
      }

      size = (len > m_pMessageList[ptr].m_iLength) ? m_pMessageList[ptr].m_iLength : len;

      if (m_pMessageList[ptr].m_pcData - m_pcData + size < m_iSize)
      {
         memcpy(data, m_pMessageList[ptr].m_pcData, size);
      }
      else
      {
         __int32 partial = m_pMessageList[ptr].m_pcData - m_pcData + size - m_iSize;

         memcpy(data, m_pMessageList[ptr].m_pcData, size - partial);
         memcpy(data + size - partial, m_pcData, partial);
      }

      m_pMessageList[ptr].m_bValid = false;

      -- m_iValidMsgCount;
   }

   // all messages prior to the first valid message before the recent ACK point are permanently invalid
   if (CMsgNo::msgcmp(m_pMessageList[ptr].m_iMsgNo, m_pMessageList[m_iPtrRecentACK].m_iMsgNo) <= 0)
   {
      while (ptr != m_iPtrRecentACK)
      {
         ptr ++;
         if (ptr == m_iMsgInfoSize)
            ptr = 0;

         if (m_pMessageList[ptr].m_bValid)
            break;
      }
   }
   else 
      ptr = m_iPtrRecentACK;

   // release the invalid message items
   while (m_iPtrFirstMsg != ptr)
   {
      m_pMessageList[m_iPtrFirstMsg].m_pcData = NULL;
      m_pMessageList[m_iPtrFirstMsg].m_iMsgNo = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iStartSeq = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iEndSeq = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iLength = -1;
      m_pMessageList[m_iPtrFirstMsg].m_bValid = false;
      m_pMessageList[m_iPtrFirstMsg].m_bDropped = false;
      m_pMessageList[m_iPtrFirstMsg].m_bInOrder = false;

      ++ m_iPtrFirstMsg;
      if (m_iPtrFirstMsg == m_iMsgInfoSize)
         m_iPtrFirstMsg = 0;
   }

   // all messages are invalid, re-init the message list
   if ((m_pMessageList[m_iPtrFirstMsg].m_iMsgNo == m_iLastMsgNo) && !(m_pMessageList[m_iPtrFirstMsg].m_bValid))
   {
      m_pMessageList[m_iPtrFirstMsg].m_pcData = NULL;
      m_pMessageList[m_iPtrFirstMsg].m_iMsgNo = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iStartSeq = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iEndSeq = -1;
      m_pMessageList[m_iPtrFirstMsg].m_iLength = -1;
      m_pMessageList[m_iPtrFirstMsg].m_bValid = false;
      m_pMessageList[m_iPtrFirstMsg].m_bDropped = false;
      m_pMessageList[m_iPtrFirstMsg].m_bInOrder = false;

      m_iPtrFirstMsg = -1;
      m_iPtrRecentACK = -1;

      m_iStartPos = m_iLastAckPos;
   }

   return size;
}

__int32 CRcvBuffer::getValidMsgCount()
{
   CGuard msgguard(m_MsgLock);

   return m_iValidMsgCount;
}
