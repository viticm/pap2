////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DLoopQueue.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-7-4 17:23:37
//  Comment     : 
/*
循环队列，空间是固定的，并环形使用。不符合stl标准（要符合不是一般的难）
为了区别stl容器，所以全部方法都是大写的
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DLOOPQUEUE_H_
#define _INCLUDE_KG3DLOOPQUEUE_H_

////////////////////////////////////////////////////////////////////////////////
/************************************************************************/
// 用于处理渐变表的三个元素的队列的问题
/************************************************************************/
namespace KG3D_CUSTOM_HELPERS
{
	template<class _STRUCT, unsigned int _nQueueSize>
	class TKG3DLoopQueue
	{
	public:
		enum
		{
			INTERP_BLEND_QUEUE_MAX_SIZE = _nQueueSize,///不可能为0，否则下面的数组就会出错
		};
	private:
		_STRUCT m_StructArray[INTERP_BLEND_QUEUE_MAX_SIZE];
		int m_nFront;
		int m_nSize;
	public:
		TKG3DLoopQueue()
			:m_nFront(0)
			,m_nSize(0)
		{

		}
		void Clear()
		{
			m_nFront = 0;
			m_nSize = 0;
		}
		inline bool Empty()
		{
			if (m_nSize > 0)
			{
				return false;
			}
			return true;
		}
		inline int Size()
		{
			return m_nSize;
		}
		_STRUCT* Front()
		{
			if (m_nSize > 0)
			{
				return &(m_StructArray[GetRealElementIndex(0)]);
			}
			return NULL;
		}
		_STRUCT* GetTail()
		{
			if (m_nSize > 0)
			{
				return GetStruct(m_nSize-1);
			}
			return NULL;
		}
		inline _STRUCT* GetStruct(int nNum)
		{
			if(nNum >= 0 && nNum < m_nSize)
			{
				return &(m_StructArray[GetRealElementIndex(nNum)]);
			}
			return NULL;
		}
		bool InsertNextInterp(const _STRUCT& Struct)	//这个函数是特殊的，可以考虑移动到KG3DEnvEffDWController
		{
			if (m_nSize >= 0 && m_nSize < INTERP_BLEND_QUEUE_MAX_SIZE)
			{
				return Push(Struct);
			}
			else if (m_nSize == INTERP_BLEND_QUEUE_MAX_SIZE)
			{
				_STRUCT* pTail = GetTail();
				if (pTail)
				{
					*pTail = Struct;
					return true;
				}
			}
			return false;	
		}
		void Pop()
		{
			if (m_nSize <= 0)
			{
				return;
			}
			m_nFront = (++m_nFront)%INTERP_BLEND_QUEUE_MAX_SIZE;
			m_nSize--;
		}
	private:
		inline bool Push(const _STRUCT& Struct)
		{
			if (m_nSize >= INTERP_BLEND_QUEUE_MAX_SIZE)
			{
				return false;
			}
			m_nSize+=1;
			_ASSERTE(m_nSize <= INTERP_BLEND_QUEUE_MAX_SIZE);
			m_StructArray[GetRealElementIndex(m_nSize-1)] = Struct;
			return true;
		}
		inline int GetRealElementIndex(int nNum)
		{
			_ASSERTE(nNum >= 0 && nNum < INTERP_BLEND_QUEUE_MAX_SIZE);
			return (m_nFront + nNum)%INTERP_BLEND_QUEUE_MAX_SIZE;
		}
	};
};

#endif //_INCLUDE_KG3DLOOPQUEUE_H_
