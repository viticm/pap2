/* -------------------------------------------------------------------------
//	文件名		：	kringbuffer.h
//	创建者		：	wangbin
//	创建时间	：	2007-12-21 11:43:00
//	功能描述	：	环形缓冲区实现
// -----------------------------------------------------------------------*/
#ifndef __KRINGBUFFER_H__
#define __KRINGBUFFER_H__

#include "Ksdef.h"

#pragma pack(push, 1)
struct KRINGBUFF_STAT
{
	size_t	nDataHead;	// 数据起始偏移位置
	size_t	nDataTail;	// 数据结束偏移位置
};
#pragma pack(pop)

class KRingBuffer
{
public:
	struct KBUFFER
	{
		CHAR*	pBuf;
		size_t	nLen;
	};
	KRingBuffer(CHAR* pBuf, size_t nLen, KRINGBUFF_STAT* pStat = NULL) : m_pBuf(pBuf), m_nLen(nLen), m_pStat(pStat)
	{
		ASSERT(pBuf && nLen > 1);
		ASSERT(pStat == NULL || (pStat->nDataHead < nLen && pStat->nDataTail < nLen));
		memset(&m_sStat, 0, sizeof(m_sStat));
		if (m_pStat == NULL)
			m_pStat = &m_sStat;
	}
	~KRingBuffer()
	{
	}
	BOOL PutData(VOID* pData, size_t nLen)
	{
		ASSERT(pData && nLen > 0);
		KBUFFER aBuff[2];
		GetFreeChip(m_pStat->nDataHead, m_pStat->nDataTail, aBuff);
		if (aBuff[0].nLen + aBuff[1].nLen < nLen)
			return FALSE;
		if (aBuff[0].nLen >= nLen)
		{
			memcpy(aBuff[0].pBuf, pData, nLen);
		}
		else
		{
			size_t nChipLen = aBuff[0].nLen;
			memcpy(aBuff[0].pBuf, pData, nChipLen);
			memcpy(aBuff[1].pBuf, (CHAR*)pData + nChipLen, nLen - nChipLen);
		}
		m_pStat->nDataTail = (m_pStat->nDataTail + nLen) % m_nLen;
		return TRUE;
	}
	BOOL GetData(VOID* pData, size_t nLen, size_t nOffset = 0)
	{
		ASSERT(pData && nLen > 0);
		KBUFFER aBuff[2];
		GetDataChip(m_pStat->nDataHead, m_pStat->nDataTail, aBuff);
		if (aBuff[0].nLen + aBuff[1].nLen < nOffset + nLen)
			return FALSE;
		if (aBuff[0].nLen >= nLen + nOffset)
		{
			memcpy(pData, &aBuff[0].pBuf[nOffset], nLen);
		}
		else if (aBuff[0].nLen <= nOffset)
		{
			memcpy(pData, &aBuff[1].pBuf[nOffset - aBuff[0].nLen], nLen);
		}
		else
		{
			size_t nChipLen = aBuff[0].nLen - nOffset;
			memcpy(pData, &aBuff[0].pBuf[nOffset], nChipLen);
			memcpy((CHAR*)pData + nChipLen, aBuff[1].pBuf, nLen - nChipLen);
		}
		return TRUE;
	}
	CHAR* GetData(size_t nLen, size_t nOffset = 0)
	{
		//ASSERT(nLen > 0);
		if (nLen <= 0)
			printf("ASSERT !!!!!GetData = %d", nLen);
		if (nLen <= 0)
			return NULL;
		KBUFFER aBuff[2];
		GetDataChip(m_pStat->nDataHead, m_pStat->nDataTail, aBuff);
		if (aBuff[0].nLen + aBuff[1].nLen < nLen + nOffset)
			return NULL;
		if (aBuff[0].nLen >= nLen + nOffset)
		{
			return &aBuff[0].pBuf[nOffset];
		}
		else if (aBuff[0].nLen <= nOffset)
		{
			return &aBuff[1].pBuf[nOffset - aBuff[0].nLen];
		}
		else
		{
			if (nLen > sizeof(m_szBuf))
				return NULL;
			size_t nChipLen = aBuff[0].nLen - nOffset;
			memcpy(m_szBuf, &aBuff[0].pBuf[nOffset], nChipLen);
			memcpy(m_szBuf + nChipLen, aBuff[1].pBuf, nLen - nChipLen);
			return m_szBuf;
		}
	};
	BOOL PopData(size_t nLen)
	{
		ASSERT(nLen > 0);
		if (GetDataSize(m_pStat->nDataHead, m_pStat->nDataTail) < nLen)
			return FALSE;
		m_pStat->nDataHead = (m_pStat->nDataHead + nLen) % m_nLen;
		return TRUE;
	};
	BOOL IsFull() CONST
	{
		return IsFull(m_pStat->nDataHead, m_pStat->nDataTail);
	}
	size_t GetFreeSize()
	{
		return GetFreeSize(m_pStat->nDataHead, m_pStat->nDataTail);
	}
	VOID Clear()
	{
		m_pStat->nDataHead = m_pStat->nDataTail;
	}
private:
	inline VOID GetFreeChip(size_t nHead, size_t nTail, KBUFFER aBuff[2]) CONST
	{
		if (IsFull(nHead, nTail))
		{
			aBuff[0].pBuf = NULL;
			aBuff[0].nLen = 0;
			aBuff[1].pBuf = NULL;
			aBuff[1].nLen = 0;
		}
		else if (nHead == 0)
		{
			aBuff[0].pBuf = m_pBuf + nTail;
			aBuff[0].nLen = m_nLen - 1 - nTail;
			aBuff[1].pBuf = NULL;
			aBuff[1].nLen = 0;
		}
		else if (nHead <= nTail)
		{
			aBuff[0].pBuf = m_pBuf + nTail;
			aBuff[0].nLen = m_nLen - nTail;
			aBuff[1].pBuf = nHead != 1 ? m_pBuf : NULL;
			aBuff[1].nLen = nHead - 1;
		}
		else //if (nHead > nTail)
		{
			aBuff[0].pBuf = m_pBuf + nTail;
			aBuff[0].nLen = nHead - nTail - 1;
			aBuff[1].pBuf = NULL;
			aBuff[1].nLen = 0;
		}
	};
	inline VOID GetDataChip(size_t nHead, size_t nTail, KBUFFER aBuff[2]) CONST
	{
		if (nHead < nTail)
		{
			aBuff[0].pBuf = m_pBuf + nHead;
			aBuff[0].nLen = nTail - nHead;
			aBuff[1].pBuf = NULL;
			aBuff[1].nLen = 0;
		}
		else if (nHead > nTail)
		{
			aBuff[0].pBuf = m_pBuf + nHead;
			aBuff[0].nLen = m_nLen - nHead;
			aBuff[1].pBuf = nTail != 0 ? m_pBuf : NULL;
			aBuff[1].nLen = nTail;
		}
		else
		{
			aBuff[0].pBuf = NULL;
			aBuff[0].nLen = 0;
			aBuff[1].pBuf = NULL;
			aBuff[1].nLen = 0;
		}
	}
	inline BOOL IsFull(size_t nHead, size_t nTail) CONST
	{
		if (nHead == nTail + 1)
			return TRUE;
		if (nHead == 0 && nTail == m_nLen - 1)
			return TRUE;
		return FALSE;
	}
	inline BOOL IsEmpty(size_t nHead, size_t nTail) CONST
	{
		return nHead == nTail;
	}
	inline size_t GetFreeSize(size_t nHead, size_t nTail) CONST
	{
		if (nTail >= nHead)
			return m_nLen - 1 - (nTail - nHead);
		else
			return nHead - nTail - 1;
	}
	inline size_t GetDataSize(size_t nHead, size_t nTail) CONST
	{
		if (nTail >= nHead)
			return nTail - nHead;
		else
			return nTail + m_nLen - nHead;
	}
private:
	KRINGBUFF_STAT	m_sStat;
	KRINGBUFF_STAT*	m_pStat;
	CHAR*			m_pBuf;
	size_t			m_nLen;
	CHAR			m_szBuf[64 * 1024];
};

#endif //__KRINGBUFFER_H__
