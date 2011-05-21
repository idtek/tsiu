#ifndef __SD_MEMPOOL_H__
#define __SD_MEMPOOL_H__

//--------------------------------------------------------------------------
#include "SDGlobalDef.h"
#include "Semaphore.h"
#include <deque>
using namespace std;

//--------------------------------------------------------------------------
#define RECV_BUFF			1010

//--------------------------------------------------------------------------
enum
{
	MsgType_Vector = 0,
	MsgType_Float = 1,
	MsgType_Int = 2,
	MsgType_String =3,

	MsgType_Reset = 9,	
	MsgType_SlowMotion = 10,
	MsgType_Command = 11,
	MsgType_DetailsSwitcher = 12
};

struct UDP_PACK
{
	u32  ulFilter;
	u32	 ulType;
	Char zName[32];
	Bool bIsHidden;
	union
	{
		struct
		{
			f32 x,y,z;
		}_vValue;
		Char _zValue[32];
		s32 _iValue;
		f32 _fValue;	
	}unValue;
};

//--------------------------------------------------------------------------
//class MemPool
//{
//public:
//	MemPool();
//	virtual ~MemPool();
//
//	int			GetUDPData(UDP_PACK *buf, int cnt = 1);	
//	void		InsertUDPData(UDP_PACK up);
//	void		CleanBuff();
//
//	inline int	GetSize() { return size; };
//	inline int	GetLoss() { return loss; };
//	inline int	GetMaxSize() { return maxsize; };
//
//	inline void	SetMaxSize(int nmax) 
//	{ 
//		maxsize = nmax; 
//	};
//
//private:
//	deque<UDP_PACK> mempool;
//	Semaphore s;
//	int size;
//	int loss;
//	int maxsize;
//};
//
////--------------------------------------------------------------------------
//inline MemPool::MemPool()
//{
//	size = 0;
//	loss = 0;
//	maxsize = 100000;
//};
//
//inline MemPool::~MemPool()
//{
//	mempool.clear();
//};
//
//inline int MemPool::GetUDPData(UDP_PACK *buf, int cnt /* = 1 */)
//{
//	if(!buf)
//		return 0;
//	if( size < cnt)
//		cnt = size;
//	s.P();
//	deque<UDP_PACK>::iterator itstart,itend;
//	itstart = itend = mempool.begin();
//	for(int i = 0; i < cnt; ++i)
//	{
//		memcpy(buf + i, &(*itend), sizeof(UDP_PACK));
//		itend++;
//	}
//	mempool.erase(itstart, itend);
//	size -= cnt;
//	s.V();
//	return cnt;
//};
//
//inline void MemPool::InsertUDPData(UDP_PACK up)
//{
//	s.P();
//	if( size < maxsize )
//	{
//		mempool.push_back(up);
//		size++;
//	}
//	else
//	{
//		loss++;
//	}
//	s.V();
//};
//
//inline void MemPool::CleanBuff()
//{
//	s.P();
//	size = 0;
//	loss = 0;
//	mempool.clear();
//	s.V();
//}

#endif
