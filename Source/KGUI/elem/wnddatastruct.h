/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2005
//	Creator	:   zbq
//	CreateTime:	2005-4-9
------------------------------------------------------------------------------------------
*****************************************************************************************/

#ifndef UI_ELEM_T_WND_DATA_STRUCT_H
#define UI_ELEM_T_WND_DATA_STRUCT_H

namespace UI
{

	//--------------------------------------------------------------------------------------------------------------
	// 提供简单的数组操作
	template <typename T>
	class TBlock
	{
	public:
		TBlock() 
			: m_pData(0)
			, m_nSize(0)
		{
		}

		~TBlock()
		{
			Free();
		}

		TBlock(TBlock const& rhs)
			: m_pData(0)
			, m_nSize(0)
		{
			Alloc(rhs.GetSize());
			for (int nIndex = 0; nIndex < GetSize(); ++nIndex)
				m_pData[nIndex] = rhs[nIndex];
		}

		TBlock const& operator=(TBlock const& rhs)
		{
			if (this != &rhs)
			{
				Alloc(rhs.GetSize());
				for (int nIndex = 0; nIndex < GetSize(); ++nIndex)
					m_pData[nIndex] = rhs[nIndex];
			}
			return *this;
		}

		bool Alloc(int nSize)
		{
			if (nSize > 0)
			{
				Free();

				m_pData = new T[nSize];
				if (m_pData)
				{
					m_nSize = nSize;
					return true;
				}
			}

			return false;
		}

		void Free()
		{
			if (m_pData)
			{
				delete[] m_pData;
				m_pData = NULL;
				m_nSize = 0;
			}
		}

		int GetSize() const
		{
			return m_nSize;
		}

		T* GetAt(int nIndex)
		{
			if (nIndex >= 0 && nIndex < m_nSize)
				return m_pData + nIndex;
			return NULL;
		}

		T const* GetAt(int nIndex) const
		{
			if (nIndex >= 0 && nIndex < m_nSize)
				return m_pData + nIndex;
			return NULL;
		}

		T& operator[](int nIndex)
		{
			return m_pData[nIndex];
		}

		T const& operator[](int nIndex) const
		{
			return m_pData[nIndex];
		}

	private:
		bool operator==(TBlock const& rhs)
		{
			return false;
		}
	private:
		T*	m_pData;
		int	m_nSize;
	};

}
//-----------------------------------------------------------------------------------------------------------------------------

#endif // UI_ELEM_T_WND_DATA_STRUCT_H

