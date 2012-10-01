#ifndef __TUTILITY_ARRAY__
#define __TUTILITY_ARRAY__

namespace TsiU
{
	template<typename T>
	class Array
	{
	public:
		Array();
		~Array();

		T& operator[]( u32 index );
		const T& operator[]( u32 index ) const;

		const Array<T>& operator=(const Array<T>& rhs);

		void ReSize(u32 newsize);
		void Set(u32 i, const T& item);
		T& Get(u32 i) const;
		void Clear();
		u32 Size() const;

		void PushBack(const T& item);
		void PopBack();
		void RemoveAt(unsigned int idx);

		void Dump();

	private:
		T*		m_Items;
		u32		m_Size;
		u32		m_Capacity;
	};

	template<typename T>
	Array<T>::Array()
	{
		m_Items = NULL;
		m_Size = 0;
		m_Capacity = 0;
	}

	template<typename T>
	Array<T>::~Array()
	{
		if( m_Items )
		{
			delete[] m_Items;
			m_Items = NULL;
		}
	}

	template<typename T> 
	T &Array<T>::operator[]( u32 i )
	{ 
		return Get(i); 
	}

	template<typename T> 
	const T &Array<T>::operator[]( u32 i ) const
	{ 
		return Get(i); 
	}

	template<typename T>
	T& Array<T>::Get(u32 i) const
	{
		D_CHECK( i < m_Size );
		return m_Items[i];
	}

	template<typename T>
	void Array<T>::Set(u32 i, const T& item)
	{
		D_CHECK( i < m_Size );
		m_Items[i] = item;
	}

	template<typename T>
	u32 Array<T>::Size() const
	{
		return m_Size;
	}

	template<typename T>
	void Array<T>::ReSize(u32 newSize)
	{
		D_CHECK( newSize >= 0 );
		if(newSize > m_Capacity)
		{
			u32 newCapacity = 2 * newSize;
			T* newItems = new T[newCapacity];
			D_CHECK(newItems);

			for( u32 i = 0; i < m_Size; ++i )
				newItems[i] = m_Items[i];

			if( m_Items )
				delete[] m_Items;

			m_Items = newItems;
			m_Capacity = newCapacity;
		}
		m_Size = newSize;
	}

	template<typename T>
	void Array<T>::PushBack(const T& item)
	{
		ReSize(m_Size+1);
		m_Items[m_Size - 1] = item;
	}

	template<typename T>
	void Array<T>::PopBack()
	{
		if(m_Size > 0)
			m_Size--;
	}

	template<typename T>
	void Array<T>::Clear()
	{
		if( m_Items )
		{
			delete[] m_Items;
			m_Items = NULL;
		}
		m_Size = 0;
		m_Capacity = 0;
	}

	template<typename T>
	const Array<T>& Array<T>::operator=(const Array<T>& rhs)
	{
		if(this == &rhs)
			return *this;

		Clear();

		//ReSize(rhs.Size());
		for(s32 i = 0; i < rhs.Size(); ++i)
		{
			PushBack(rhs[i]);
		}
		return *this;
	}

	template<typename T>
	void Array<T>::Dump()
	{
		D_DebugOut("Size = %d, Capacity = %d\n", m_Size, m_Capacity);
	}

	template<typename T>
	void Array<T>::RemoveAt(unsigned int idx)
	{
		D_CHECK( idx < m_Size );
		int newSize = m_Size - 1;
		if(newSize == 0)
		{
			Clear();
		}
		else
		{
			int newCapacity = (m_Capacity > newSize * 2) ? m_Capacity / 2 : m_Capacity;
			T* newItems = new T[newCapacity];
			D_CHECK(newItems);
			int newIdx = 0;
			for( u32 i = 0; i < m_Size; ++i )
			{
				if(i == idx)
					continue;
				newItems[newIdx++] = m_Items[i];
			}
			if( m_Items )
				delete[] m_Items;
			m_Items = newItems;
			m_Capacity = newCapacity;
			m_Size = newSize;
		}
	}
}

#endif
