#ifndef COMMON_T_BLOCK_H
#define COMMON_T_BLOCK_H


#include "./ksmallobjectpool.h"

#define ENABLE_BLOCK_POOL

class KBlockPool
{
public:
	KBlockPool();
	~KBlockPool();

	static bool Init();
	static void Exit();
	static KBlockPool& GetSelf();

	void* Alloc(size_t nSize);
	void Free(void* pData, size_t nSize);
	void Purge();

private:
	KSmallObjectPool	m_Pool32;
	KSmallObjectPool	m_Pool64;
	KSmallObjectPool	m_Pool128;
	KSmallObjectPool	m_Pool256;

	static KBlockPool* ms_pSelf;
};


//-------------------------------------------------------------------------------------------------------
template <typename T>
class TBlock
{
public:
	TBlock();
	TBlock(TBlock const& rhs);
	~TBlock();

	TBlock const& operator=(TBlock const& rhs);

	bool Alloc(int nSize);
	void Free();

	int GetSize() const;

	T* GetAt(int nIndex);
	T const* GetAt(int nIndex) const;

	T& operator[](int nIndex);
	T const& operator[](int nIndex) const;

private:
	bool operator==(TBlock const& rhs);

private:
	T*	m_pData;
	int	m_nSize;
};


//----------------------------------------------------------------------------------------------------
template <typename T>
TBlock<T>::TBlock() 
: m_pData(NULL)
, m_nSize(0)
{
}


template <typename T>
TBlock<T>::~TBlock()
{
	Free();
}


template <typename T>
TBlock<T>::TBlock(TBlock const& rhs)
: m_pData(NULL)
, m_nSize(0)
{
	Alloc(rhs.GetSize());
	for (int nIndex = 0; nIndex < GetSize(); ++nIndex)
	{
		m_pData[nIndex] = rhs[nIndex];
	}
}


template <typename T>
TBlock<T> const& TBlock<T>::operator=(TBlock const& rhs)
{
	if (this != &rhs)
	{
		Alloc(rhs.GetSize());
        for (int nIndex = 0; nIndex < GetSize(); ++nIndex)
        {
            m_pData[nIndex] = rhs[nIndex];
        }
	}
	return *this;
}


template <typename T>
bool TBlock<T>::Alloc(int nSize)
{
	Free();

	if (nSize > 0)
	{
#ifdef ENABLE_BLOCK_POOL
		m_pData = reinterpret_cast<T*>(KBlockPool::GetSelf().Alloc(sizeof(T) * nSize));
		if (m_pData)
		{
			m_nSize = nSize;

			for (int nIndex = 0; nIndex < m_nSize; ++nIndex)
				new(m_pData + nIndex)T;

			return true;
		}
#else // ENABLE_BLOCK_POOL
		m_pData = new T[nSize];
		if (m_pData)
		{
			m_nSize = nSize;
			return true;
		}
#endif // ENABLE_BLOCK_POOL
	}

	return false;
}


template <typename T>
void TBlock<T>::Free()
{
	if (m_pData)
	{
#ifdef ENABLE_BLOCK_POOL
		for (int nIndex = m_nSize - 1; nIndex >= 0; --nIndex)
			m_pData[nIndex].~T();

		KBlockPool::GetSelf().Free(m_pData, sizeof(T) * m_nSize);
#else // ENABLE_BLOCK_POOL
		delete[] m_pData;
#endif // ENABLE_BLOCK_POOL

		m_pData = NULL;
		m_nSize = 0;
	}
}


template <typename T>
int TBlock<T>::GetSize() const
{
	return m_nSize;
}


template <typename T>
T* TBlock<T>::GetAt(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nSize)
		return m_pData + nIndex;
	return NULL;
}


template <typename T>
T const* TBlock<T>::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < m_nSize)
		return m_pData + nIndex;
	return NULL;
}


template <typename T>
T& TBlock<T>::operator[](int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}


template <typename T>
T const& TBlock<T>::operator[](int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}


template <typename T>
bool TBlock<T>::operator==(TBlock const& rhs)
{
	return false;
}

#endif // COMMON_T_BLOCK_H

