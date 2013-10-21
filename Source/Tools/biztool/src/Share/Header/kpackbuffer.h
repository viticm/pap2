/* -------------------------------------------------------------------------
//	文件名		：	kpackbuffer.h
//	创建者		：	wangbin
//	创建时间	：	2008-1-11 15:44:00
//	功能描述	：	数据包实现
// -----------------------------------------------------------------------*/
#ifndef __KPACKBUFFER_H__
#define __KPACKBUFFER_H__

#include "kringbuffer.h"

class KPackBuffer
{
public:
	KPackBuffer(CHAR* pBuf, size_t nLen, KRINGBUFF_STAT* pStat = NULL) : m_cBuffer(pBuf, nLen, pStat)
	{
	}
	~KPackBuffer()
	{
	}
	BOOL PutPack(VOID* pData, size_t nLen)
	{
		size_t nSize = m_cBuffer.GetFreeSize();
		if (nLen + sizeof(WORD) > nSize)
			return FALSE;
		WORD wSize = nLen;
		m_cBuffer.PutData(&wSize, sizeof(WORD));
		m_cBuffer.PutData(pData, nLen);
		return TRUE;
	}
	CHAR* GetPack(size_t& nLen)
	{
		WORD wSize = 0;
		if (!m_cBuffer.GetData((CHAR*)&wSize, sizeof(wSize)))
			return NULL;
		if (wSize == 0)
		{
			ASSERT(FALSE);
			return NULL;
		}
		CHAR* pData = m_cBuffer.GetData(wSize, sizeof(WORD));
		if (!pData)
			return NULL;
		nLen = wSize;
		return pData;
	}
	VOID PopPack()
	{
		WORD wSize = 0;
		if (m_cBuffer.GetData((CHAR*)&wSize, sizeof(wSize)))
			m_cBuffer.PopData(wSize + sizeof(wSize));
	};
private:
	KRingBuffer m_cBuffer;
};

#endif //__KPACKBUFFER_H__
