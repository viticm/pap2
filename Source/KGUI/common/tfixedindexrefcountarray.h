#ifndef COMMON_T_FIXED_INDEX_REF_COUNT_ARRAY_H
#define COMMON_T_FIXED_INDEX_REF_COUNT_ARRAY_H


template<typename Value, typename RefCount = short, typename Container = std::vector<Value> >
class TFixedIndexRefCountArray
{
public:
	static int const INVALID_INDEX = -1;

private:
	typedef std::vector<RefCount>	RefCountVector;

public:
	typedef typename Value value_type;
	typedef value_type* pointer;
	typedef value_type& reference;
	typedef value_type const* const_pointer;
	typedef value_type const& const_reference;

public:
	TFixedIndexRefCountArray();
	virtual ~TFixedIndexRefCountArray();

	void Reserve(int nCount);
	int Append(Value const& rhs);
	int Find(Value const& rhs) const;
	bool Remove(int nIndex);
	void Clear();

	bool AddRefCount(int nIndex);
	bool DecRefCount(int nIndex);
	int	GetRefCount(int nIndex) const;

	int	GetUsedCount() const;
	int GetTotalCount() const;

	Value const* GetAt(int nIndex) const;
	Value* GetAt(int nIndex);

private:
	Container		m_aValue;
	RefCountVector	m_aRefCount;
	int				m_nUsed;
};


//------------------------------------------------------------------------------------------------------------
template<typename Value, typename RefCount, typename Container>
TFixedIndexRefCountArray<Value, RefCount, Container>::TFixedIndexRefCountArray()
: m_nUsed(0)
{
}


template<typename Value, typename RefCount, typename Container>
TFixedIndexRefCountArray<Value, RefCount, Container>::~TFixedIndexRefCountArray()
{
	Clear();
}


template<typename Value, typename RefCount, typename Container>
void TFixedIndexRefCountArray<Value, RefCount, Container>::Reserve(int nCount)
{
	m_aValue.reserve(nCount);
}


template<typename Value, typename RefCount, typename Container>
int TFixedIndexRefCountArray<Value, RefCount, Container>::Append(Value const& rhs)
{
	int nIndex = Find(rhs);
	if (nIndex != INVALID_INDEX)
	{
		++m_aRefCount[nIndex];
		return nIndex;
	}

	for (size_t nIndex = 0; nIndex < m_aRefCount.size(); ++nIndex)
	{
		if (m_aRefCount[nIndex] == 0)
		{
			m_aValue[nIndex] = rhs;
			++m_aRefCount[nIndex];
			++m_nUsed;
			return static_cast<int>(nIndex);
		}
	}

	m_aRefCount.push_back(1);
	m_aValue.push_back(rhs);
	++m_nUsed;
	ASSERT(m_aRefCount.size() == m_aValue.size());
	return static_cast<int>(m_aRefCount.size() - 1);
}


template<typename Value, typename RefCount, typename Container>
int TFixedIndexRefCountArray<Value, RefCount, Container>::Find(Value const& rhs) const
{
	for (size_t nIndex = 0; nIndex < m_aRefCount.size(); ++nIndex)
	{
		if (m_aRefCount[nIndex] != 0 && m_aValue[nIndex] == rhs)
		{
			return static_cast<int>(nIndex);
		}
	}

	return INVALID_INDEX;
}


template<typename Value, typename RefCount, typename Container>
bool TFixedIndexRefCountArray<Value, RefCount, Container>::Remove(int nIndex)
{
	if (nIndex >= 0 && nIndex < GetTotalCount())
	{
		if (--m_aRefCount[nIndex] == 0)
		{
			m_aValue[nIndex] = Value();
			--m_nUsed;
			return true;
		}
	}
	return false;
}

template<typename Value, typename RefCount, typename Container>
void TFixedIndexRefCountArray<Value, RefCount, Container>::Clear()
{
	if (m_nUsed > 0)
	{
		int const nCount = GetTotalCount();
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			if (m_aRefCount[nIndex] != 0)
			{
				m_aValue[nIndex] = Value();
				--m_nUsed;
			}
		}
	}
	m_aValue.clear();
	m_aRefCount.clear();
}


template<typename Value, typename RefCount, typename Container>
bool TFixedIndexRefCountArray<Value, RefCount, Container>::AddRefCount(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetTotalCount())
		return false;

	++m_aRefCount[nIndex];
	return true;
}


template<typename Value, typename RefCount, typename Container>
bool TFixedIndexRefCountArray<Value, RefCount, Container>::DecRefCount(int nIndex)
{
	if (nIndex < 0 || nIndex >= GetTotalCount())
		return false;

    if (m_aRefCount[nIndex] == 0)
        return false;

	--m_aRefCount[nIndex];
	return true;
}


template<typename Value, typename RefCount, typename Container>
int TFixedIndexRefCountArray<Value, RefCount, Container>::GetRefCount(int nIndex) const
{
	if (nIndex < 0 || nIndex >= GetTotalCount())
		return 0;

	return m_aRefCount[nIndex];
}


template<typename Value, typename RefCount, typename Container>
inline int TFixedIndexRefCountArray<Value, RefCount, Container>::GetUsedCount() const
{
	return m_nUsed;
}


template<typename Value, typename RefCount, typename Container>
inline int TFixedIndexRefCountArray<Value, RefCount, Container>::GetTotalCount() const
{
	return static_cast<int>(m_aValue.size());
}


template<typename Value, typename RefCount, typename Container>
Value const* TFixedIndexRefCountArray<Value, RefCount, Container>::GetAt(int nIndex) const
{
	if (nIndex >= 0 && nIndex < GetTotalCount() && m_aRefCount[nIndex] != 0)
		return &m_aValue[nIndex];
	return NULL;
}


template<typename Value, typename RefCount, typename Container>
Value* TFixedIndexRefCountArray<Value, RefCount, Container>::GetAt(int nIndex)
{
	if (nIndex >= 0 && nIndex < GetTotalCount() && m_aRefCount[nIndex] != 0)
		return &m_aValue[nIndex];
	return NULL;
}


#endif // COMMON_T_FIXED_INDEX_REF_COUNT_ARRAY_H

