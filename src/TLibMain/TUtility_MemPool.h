#ifndef __TUTILITY_MEMPOOL__
#define __TUTILITY_MEMPOOL__

#include <deque>
#include "TCore_Mutex.h"

namespace TsiU
{
	template<typename T>
	class MemPool
	{
	public:
		MemPool();
		virtual ~MemPool();

		int			GetUDPData(T *buf, int cnt = 1);	
		void		InsertUDPData(const T& up);
		void		CleanBuff();

		inline int	GetSize() { return size; };
		inline int	GetLoss() { return loss; };
		inline int	GetMaxSize() { return maxsize; };

		inline void	SetMaxSize(int nmax) 
		{ 
			maxsize = nmax; 
		};

	private:
		std::deque<T> mempool;
		Mutex s;
		int size;
		int loss;
		int maxsize;
	};

	template<typename T>
	MemPool<T>::MemPool()
	{
		size = 0;
		loss = 0;
		maxsize = 100000;
	};

	template<typename T>
	MemPool<T>::~MemPool()
	{
		mempool.clear();
	};

	template<typename T>
	int MemPool<T>::GetUDPData(T *buf, int cnt /* = 1 */)
	{
		if(!buf)
			return 0;
		if( size < cnt)
			cnt = size;
		s.Lock();
		std::deque<T>::iterator itstart,itend;
		itstart = itend = mempool.begin();
		for(int i = 0; i < cnt; ++i)
		{
			memcpy(buf + i, &(*itend), sizeof(T));
			itend++;
		}
		mempool.erase(itstart, itend);
		size -= cnt;
		s.UnLock();
		return cnt;
	};

	template<typename T>
	void MemPool<T>::InsertUDPData(const T& up)
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

	template<typename T>
	void MemPool<T>::CleanBuff()
	{
		s.Lock();
		size = 0;
		loss = 0;
		mempool.clear();
		s.UnLock();
	}
}

#endif