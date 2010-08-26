#ifndef __VM_GLOBALDEF_H__
#define __VM_GLOBALDEF_H__

#include "TsiU.h"

using namespace TsiU;

#if PLATFORM_TYPE == PLATFORM_WIN32
#include "TWin32_Header.h"
#endif

extern Engine*						g_poEngine;

//-------------------------------------------------------------------------
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
#include <deque>

class MemPool
{
public:
	MemPool();
	virtual ~MemPool();

	int			GetUDPData(UDP_PACK *buf, int cnt = 1);	
	void		InsertUDPData(UDP_PACK up);
	void		CleanBuff();

	inline int	GetSize() { return size; };
	inline int	GetLoss() { return loss; };
	inline int	GetMaxSize() { return maxsize; };

	inline void	SetMaxSize(int nmax) 
	{ 
		maxsize = nmax; 
	};

private:
	std::deque<UDP_PACK> mempool;
	Mutex s;
	int size;
	int loss;
	int maxsize;
};

//--------------------------------------------------------------------------
inline MemPool::MemPool()
{
	size = 0;
	loss = 0;
	maxsize = 100000;
};

inline MemPool::~MemPool()
{
	mempool.clear();
};

inline int MemPool::GetUDPData(UDP_PACK *buf, int cnt /* = 1 */)
{
	if(!buf)
		return 0;
	if( size < cnt)
		cnt = size;
	s.Lock();
	std::deque<UDP_PACK>::iterator itstart,itend;
	itstart = itend = mempool.begin();
	for(int i = 0; i < cnt; ++i)
	{
		memcpy(buf + i, &(*itend), sizeof(UDP_PACK));
		itend++;
	}
	mempool.erase(itstart, itend);
	size -= cnt;
	s.UnLock();
	return cnt;
};

inline void MemPool::InsertUDPData(UDP_PACK up)
{
	s.Lock();
	if( size < maxsize )
	{
		mempool.push_back(up);
		size++;
	}
	else
	{
		loss++;
	}
	s.UnLock();
};

inline void MemPool::CleanBuff()
{
	s.Lock();
	size = 0;
	loss = 0;
	mempool.clear();
	s.UnLock();
}
//----------------------------------------------------------------
class MyEngine : public Engine
{
public:
	MyEngine(u32 _uiWidth, u32 _uiHeight, const Char* _strTitle, Bool _bIsWindow);

	virtual void DoInit();
	virtual void DoUnInit();

private:
	Socket*		m_pRecvSocket;
	Socket*		m_pSendSocket;
	Thread*		m_pRecvThread;
	MemPool*	m_pUDPPackBuffer;
};

#endif