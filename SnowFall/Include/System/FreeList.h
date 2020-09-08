#pragma once
//NOTE:
/*
	Free list with support for 65k Objects and 65k Generation cycles each.
	This could be made easier?
*/

#pragma once
#include <vector>
#include "System/Types.h"
#include "System/Assert.h"

template <class T>
struct FreeListItem
{
	Uint16 m_Generation = 0;
	T m_Object;
};

template <class T>
class FreeList
{
private:
	std::vector<FreeListItem<T>> m_Items;
	std::vector<int> m_FreeList;
	Uint32 m_Count = 0;
	Uint32 m_MaxCount = 0;

public:
	FreeList()
	{
		// Does nothing
	}

	void Initialize(Uint32 size)
	{
		m_Count = 0;
		m_MaxCount = size;
		m_Items.resize(m_MaxCount);
		m_FreeList.resize(m_MaxCount);

		// Reverse fill free list i.e [0] = MaxCount - 1
		for (int i = m_MaxCount - 1; i >= 0; --i)
		{
			m_FreeList[i] = (Uint32)((m_MaxCount - 1) - i);
		}
	}

	T* operator[](const Handle index)
	{
		if (m_Items.empty()) { return nullptr; }
		FreeListItem<T>* item = &m_Items[index.Index]; // O(n)
		return (item->m_Generation == index.Generation) ? &m_Items[index.Index].m_Object : nullptr;
	}

	//Return handle, and pointer to the object
	Handle Allocate()
	{
		if (!m_FreeList.empty() && !m_Items.empty())
		{
			// Get the next free Handle, Or with the Generation
			Handle handle;
			handle.Index = m_FreeList.back();
			handle.Generation = m_Items[handle.Index].m_Generation;

			// Pop and return handle.
			m_FreeList.pop_back();
			m_Count++;
			return handle;
		}
		assert(0 && "Free List memory is full or Uninitialized");
		return -1;
	}

	// This version outs the object immediatly
	Handle Allocate(T*& outPtr)
	{
		if (!m_FreeList.empty())
		{
			// Get the next free Handle, Or with the Generation
			Handle handle;
			handle.Index = m_FreeList.back();
			handle.Generation = m_Items[handle.Index].m_Generation;

			// Pop and return handle.
			m_FreeList.pop_back();
			outPtr = &m_Items[handle.Index].m_Object;
			m_Count++;
			return handle;
		}
		assert(0 && "Free List memory is full or Uninitialized");
		return Handle();
	}

	void Destroy(Handle handle)
	{
		if (m_Items.empty()) { return; }
		FreeListItem<T>* item = &m_Items[handle.Index];
		if (item->m_Generation == handle.Generation)
		{
			// Effectivly destroyed
			item->m_Generation++;
			item->m_Object = T(); // Reset object to defaults?
			m_FreeList.push_back(handle.Index);
			m_Count--;
		}

		// No Object to Destory.
		return;
	}

	void Clear()
	{
		m_Items.clear();
		m_FreeList.clear();
		m_Count = 0;
		m_MaxCount = 0;
	}

	Uint32 Size()
	{
		return m_Count;
	}

	Uint32 MaxSize()
	{
		return m_MaxCount;
	}

	bool IsFull()const
	{
		return m_Count >= m_MaxCount;
	}
};