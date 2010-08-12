#ifndef __TUTILITY_LIST__
#define __TUTILITY_LIST__

#include "TCore_Types.h"

namespace TsiU
{
	template<typename T>
	class List
	{
		struct ListNode
		{
			T value;
			ListNode* next;
			ListNode* prev;
		};

	public:
		List();
		~List();
		
		T& operator[]( u32 index );
		const T& operator[]( u32 index ) const;

		void AddHead(const T& item);
		void AddTail(const T& item);

		void Insert(u32 idx, const T& item, Bool isAfter = true);
		void Remove(u32 idx);
		T& Get(u32 idx);
		T& Get(u32 idx) const;
		u32 Size() const;
		void Clear();

		void Dump();

	private:
		ListNode* Find(u32 idx) const
		{
			if(m_Size == 0 || idx >= m_Size)
				return NULL;
			
			if( idx <= m_Size / 2)
			{
				ListNode* pNode = m_Head;
				for(s32 i = 0; i < (s32)idx + 1; ++i)
				{
					pNode = pNode->next;
				}
				return pNode;
			}
			else
			{
				ListNode* pNode = m_Tail;
				for(s32 i = m_Size - 1; i >= (s32)idx; --i)
				{
					pNode = pNode->prev;
				}
				return pNode;
			}
		}

	private:
		u32 m_Size;
		ListNode* m_Head;
		ListNode* m_Tail;
	};

	template<typename T>
	List<T>::List()
	{
		m_Size = 0;
		m_Head = new ListNode;
		D_CHECK(m_Head);
		m_Tail = new ListNode;
		D_CHECK(m_Tail);
		m_Head->prev = NULL;
		m_Head->next = m_Tail;
		m_Tail->prev = m_Head;
		m_Tail->next = NULL;
	}
	template<typename T>
	List<T>::~List()
	{
	}
	template<typename T>
	void List<T>::AddHead(const T& item)
	{
		Insert(0, item, false);
	}
	template<typename T>
	void List<T>::AddTail(const T& item)
	{
		Insert(m_Size - 1, item);
	}
	/*template<typename T>
	ListNode* List<T>::Find(u32 idx)
	{
		
	}*/
	template<typename T>
	T& List<T>::Get(u32 idx)
	{
		ListNode* pNode = Find(idx);
		D_CHECK(pNode);
		return pNode->value;
	}
	template<typename T>
	T& List<T>::Get(u32 idx) const
	{
		ListNode* pNode = Find(idx);
		D_CHECK(pNode);
		return pNode->value;
	}
	template<typename T>
	void List<T>::Insert(u32 idx, const T& item, Bool isAfter)
	{
		ListNode* pNode = Find(idx);
		if(!pNode && m_Size == 0)
		{
			pNode = (isAfter ? m_Head : m_Tail);
		}
		D_CHECK(pNode);
		
		ListNode* iNode = new ListNode;
		D_CHECK(iNode);
		iNode->value = item;

		if(isAfter == false)
		{	
			iNode->prev = pNode->prev;
			iNode->next = pNode;
			pNode->prev->next = iNode;
			pNode->prev = iNode;
		}
		else
		{
			iNode->prev = pNode;
			iNode->next = pNode->next;
			pNode->next->prev = iNode;
			pNode->next = iNode;
		}
		m_Size++;
	}
	template<typename T>
	void List<T>::Remove(u32 idx)
	{
		ListNode* pNode = Find(idx);
		D_CHECK(pNode);
		pNode->prev->next = pNode->next;
		pNode->next->prev = pNode->prev;
		delete pNode;
		m_Size--;
	}
	template<typename T>
	void List<T>::Clear()
	{
		m_Size = 0;
		m_Head->prev = NULL;
		m_Head->next = m_Tail;
		m_Tail->prev = m_Head;
		m_Tail->next = NULL;
	}
	template<typename T>
	u32 List<T>::Size() const
	{
		return m_Size;
	}
	template<typename T> 
	T& List<T>::operator[]( u32 i )
	{ 
		return Get(i); 
	}

	template<typename T> 
	const T& List<T>::operator[]( u32 i ) const
	{ 
		return Get(i); 
	}
	template<typename T>
	void List<T>::Dump()
	{
		D_DebugOut("Size = %d\n", m_Size);
	}
}

#endif