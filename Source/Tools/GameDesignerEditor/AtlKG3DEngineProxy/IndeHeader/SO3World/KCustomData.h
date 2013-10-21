////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KCustomData.h
//  Version     : 1.0
//  Creator     : zhaochunfeng
//  Comment     : 对一块内存做任意存取操作的支持
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KCUSTOMDATA_H_
#define _KCUSTOMDATA_H_

#include "Global.h"

template <int nSize>
class KCustomData
{
public:
    KCustomData()
    {
        memset(m_byByffer, 0, sizeof(m_byByffer));
    }

    BOOL Load(BYTE* pbyData, size_t uDataLen)
    {
        BOOL bResult = false;

        assert(pbyData);

        KGLOG_PROCESS_ERROR(uDataLen <= (size_t)nSize);
        memcpy(m_byByffer, pbyData, uDataLen);

        bResult = true;
Exit0:
        return bResult;
    }

    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize)
    {
        BOOL bResult = false;

        assert(puUsedSize);
        assert(pbyBuffer);

        KGLOG_PROCESS_ERROR(uBufferSize >= (size_t)nSize);
        memcpy(pbyBuffer, m_byByffer, nSize);

        *puUsedSize = (size_t)nSize;

        bResult = true;
Exit0:
        return bResult;
    }

    template <class T> BOOL SetValue(int nOffset, T Value)
    {
        if (nOffset >= 0 && nOffset <= nSize - (int)sizeof(T))
        {
            *(T*)(m_byByffer + nOffset) = Value;
            return true;
        }
        return false;
    }

    template <class T> BOOL GetValue(int nOffset, T* pValue)
    {
        if (nOffset >= 0 && nOffset <= nSize - (int)sizeof(T))
        {
            *pValue = *(T*)(m_byByffer + nOffset);
            return true;
        }
        return false;
    }

    // nBitIndex是整个Buffer从第一个字节算起,从左往右数的bit数,计数从0开始
    // nBitIndex的合法值区间 [0, nSize * 8)
    BOOL SetBit(int nBitIndex, BOOL bValue)
    {
        BOOL    bResult    = false;
        int     nByteIndex = nBitIndex / CHAR_BIT;
        int     nByteBit   = CHAR_BIT - 1 - nBitIndex % CHAR_BIT;

        if (nByteIndex >= 0 && nByteIndex < nSize && nByteBit >= 0)
        {
            BYTE byValue = m_byByffer[nByteIndex];

            if (bValue)
            {
                byValue |= (0x1 << nByteBit);
            }
            else
            {
                byValue &= ~(0x1 << nByteBit);
            }

            m_byByffer[nByteIndex] = byValue;

            bResult = true;
        }
        return bResult;
    }

    BOOL GetBit(int nBitIndex, BOOL* pbValue)
    {
        BOOL    bResult    = false;
        int     nByteIndex = nBitIndex / CHAR_BIT;
        int     nByteBit   = CHAR_BIT - 1 - nBitIndex % CHAR_BIT;

        if (nByteIndex >= 0 && nByteIndex < nSize && nByteBit >= 0)
        {
            BYTE byValue = m_byByffer[nByteIndex];

            *pbValue = (BOOL)((byValue >> nByteBit) & 0x1);

            bResult = true;
        }
        return bResult;
    }

    BOOL SetString(int nOffset, const char cszString[])
    {
        int nStrlen = (int)strlen(cszString);

        if (nOffset >= 0 && nOffset <= nSize - nStrlen - 1)
        {
            memcpy(m_byByffer + nOffset, cszString, nStrlen + 1);
            return true;
        }
        return false;
    }

    const char* GetString(int nOffset)
    {
        if (nOffset >= 0 && nOffset < nSize)
        {
            for (int i = nOffset; i < nSize; i++)
            {
                if (m_byByffer[i] == '\0')
                {
                    return (const char*)(m_byByffer + nOffset);
                }
            }
        }
        return NULL;
    }

private:
    BYTE m_byByffer[nSize];
};

#endif
