/* -------------------------------------------------------------------------
//	文件名		：	kipcstream.h
//	创建者		：	wangbin
//	创建时间	：	2007-12-21 11:53:00
//	功能描述	：	进程间通信实现
// -----------------------------------------------------------------------*/
#ifndef __KIPCSTREAM_H__
#define __KIPCSTREAM_H__

#include "kringbuffer.h"
#ifndef WIN32
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "Engine.h"
#endif

// guard管理的server类型
enum KE_GUARDSERVER
{
	emKGUARDSERVER_FIRSTID = 1000,	// 起始ID
	emKGUARDSERVER_GODDESS = 0,
	emKGUARDSERVER_BISHOP,
	emKGUARDSERVER_GAMECENTER,
	emKGUARDSERVER_LOGSERVER,
	emKGUARDSERVER_GAMESERVER01,
	emKGUARDSERVER_GAMESERVER02,
	emKGUARDSERVER_GAMESERVER03,
	emKGUARDSERVER_GAMESERVER04,
	emKGUARDSERVER_GAMESERVER05,
	emKGUARDSERVER_GAMESERVER06,
	emKGUARDSERVER_GAMESERVER07,
	emKGUARDSERVER_GAMESERVER08,
	emKGUARDSERVER_GAMESERVER09,
	emKGUARDSERVER_GAMESERVER10,
	emKGUARDSERVER_GAMESERVER11,
	emKGUARDSERVER_GAMESERVER12,
	emKGUARDSERVER_COUNT
};

class IKPackStream
{
public:
	virtual VOID	Release() = 0;
	virtual BOOL	PutPack(VOID* pData, size_t nLen) = 0;
	virtual CHAR*	GetPack(size_t& nLen) = 0;
	virtual VOID	PopPack() = 0;
};

class KIpcStream : public IKPackStream
{
public:
	KIpcStream() : m_pSendBuffer(NULL), m_pRecvBuffer(NULL), m_pszSendBuffer(NULL), m_pszRecvBuffer(NULL)
	{
#ifdef WIN32
		m_hRecvHandle = NULL;
		m_hSendHandle = NULL;
#else
		m_nRecvHandle = -1;
		m_nSendHandle = -1;
#endif
	}
	virtual ~KIpcStream()
	{
		if (m_pSendBuffer)
		{
			delete m_pSendBuffer;
			m_pSendBuffer = NULL;
		}
		if (m_pRecvBuffer)
		{
			delete m_pRecvBuffer;
			m_pRecvBuffer = NULL;
		}
	}
	virtual VOID Release()
	{
		delete this;
	};
	BOOL Initialize(INT nKey, size_t nLen, BOOL bReverse = FALSE)
	{
#ifdef WIN32
		ASSERT(nKey > 0 && nLen > 0);
		if (m_pSendBuffer || m_pRecvBuffer)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		CHAR szSendKey[64];
		CHAR szRecvKey[64];
		if (bReverse)
		{
			_snprintf(szSendKey, sizeof(szSendKey) - 1, "%d_r", nKey);
			_snprintf(szRecvKey, sizeof(szRecvKey) - 1, "%d_w", nKey);
		}
		else
		{
			_snprintf(szSendKey, sizeof(szSendKey) - 1, "%d_w", nKey);
			_snprintf(szRecvKey, sizeof(szRecvKey) - 1, "%d_r", nKey);
		}
		szSendKey[sizeof(szSendKey) - 1] = '\0';
		szRecvKey[sizeof(szRecvKey) - 1] = '\0';
		size_t nHeadLen = (sizeof(KRINGBUFF_STAT) + 0xf) & ~0xf;
		m_pszSendBuffer = CreateShareMemory(szSendKey, nHeadLen + nLen, &m_hSendHandle);
		m_pszRecvBuffer = CreateShareMemory(szRecvKey, nHeadLen + nLen, &m_hRecvHandle);
#else
		ASSERT(nKey > 0);
		if (nKey <= 0)
		{
			ASSERT(FALSE);
			return FALSE;
		}
		key_t nSendKey = nKey * 10;
		key_t nRecvKey = nSendKey + 1;
		if (bReverse)
		{
			key_t nTmp = nSendKey;
			nSendKey = nRecvKey;
			nRecvKey = nTmp;
		}
		size_t nHeadLen = (sizeof(KRINGBUFF_STAT) + 0xf) & ~0xf;
		m_pszSendBuffer = CreateShareMemory(nSendKey, nHeadLen + nLen, &m_nSendHandle);
		m_pszRecvBuffer = CreateShareMemory(nRecvKey, nHeadLen + nLen, &m_nRecvHandle);
#endif
		if (!m_pszSendBuffer || !m_pszRecvBuffer)
			return FALSE;
		m_pSendBuffer = new KRingBuffer(&m_pszSendBuffer[nHeadLen], nLen, (KRINGBUFF_STAT*)m_pszSendBuffer);
		m_pRecvBuffer = new KRingBuffer(&m_pszRecvBuffer[nHeadLen], nLen, (KRINGBUFF_STAT*)m_pszRecvBuffer);
		// 为了防止错误的数据积留，清空要读取的数据
		m_pRecvBuffer->Clear();
		return TRUE;
	}
	VOID UnInitialize()
	{
		if (m_pSendBuffer)
		{
#ifdef WIN32
			CloseHandle(m_hSendHandle);
			m_hSendHandle = NULL;
#else
			shmdt(m_pszSendBuffer);
			m_nSendHandle = -1;
#endif
			delete m_pSendBuffer;
			m_pSendBuffer = NULL;
			m_pszSendBuffer = NULL; 
		}
		if (m_pRecvBuffer)
		{
#ifdef WIN32
			CloseHandle(m_hRecvHandle);
			m_hRecvHandle = NULL;
#else
			shmdt(m_pszRecvBuffer);
			m_nRecvHandle = -1;
#endif
			delete m_pRecvBuffer;
			m_pRecvBuffer = NULL;
			m_pszRecvBuffer = NULL;
		}
	}
	virtual BOOL PutPack(VOID* pData, size_t nLen)
	{
		if (!m_pSendBuffer)
			return FALSE;
		size_t nFreeSize = m_pSendBuffer->GetFreeSize();
		if (nFreeSize < nLen + sizeof(size_t))
			return FALSE;
		m_pSendBuffer->PutData(&nLen, sizeof(size_t));
		m_pSendBuffer->PutData(pData, nLen);
		return TRUE;
	}
	virtual CHAR* GetPack(size_t& nLen)
	{
		size_t nPackLen = 0;
		if (!m_pRecvBuffer || !m_pRecvBuffer->GetData((CHAR*)&nPackLen, sizeof(size_t)))
			return FALSE;
		ASSERT(nPackLen > 0);
		CHAR* pData = m_pRecvBuffer->GetData(nPackLen, sizeof(size_t));
		if (!pData)
			return FALSE;
		nLen = nPackLen;
		return pData;
	}
	virtual VOID PopPack()
	{
		if (!m_pRecvBuffer)
			return;
		size_t nPackLen = 0;
		if (!m_pRecvBuffer->GetData((CHAR*)&nPackLen, sizeof(size_t)))
		{
			ASSERT(FALSE);
			return;
		}
		m_pRecvBuffer->PopData(nPackLen + sizeof(size_t));
	};
private:
#ifdef WIN32
	CHAR* CreateShareMemory(LPCSTR pszKey, size_t nLen, HANDLE* pHandle)
	{
		ASSERT(pszKey && nLen > 0 && pHandle);
		HANDLE hHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, pszKey);
		if (!hHandle) 
		{
			hHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)nLen, pszKey);
			if (!hHandle) 
			{
				// DEBUG
				printf("Faield to create ShareMemory:%s, Size:%uK\n", pszKey, nLen / 1024);
				return NULL;
			}
		} 
		*pHandle = hHandle;
		LPVOID lpMapAddress = MapViewOfFile(hHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		return (CHAR*)lpMapAddress;
	}
#else
	CHAR* CreateShareMemory(key_t nKey, size_t nLen, int* pnHandle)
	{
#define SHM_MOD (SHM_R | SHM_W | SHM_R >> 3 | SHM_R >> 6)
		int nIndex = 0;
		int nHandle = shmget(nKey, nLen, SHM_MOD | IPC_CREAT | IPC_EXCL);
		if (nHandle < 0 && errno == EEXIST)
		{
			nHandle = shmget(nKey, nLen, SHM_MOD);
		}
		if (nHandle < 0)
		{
			printf("Failed to create share memory with key[%u]: %s\n", nKey, strerror(errno));
			return NULL;
		}
		*pnHandle = nHandle;
		return (CHAR*)shmat(nHandle, NULL, 0);
	}
#endif	
private:
	KRingBuffer* m_pSendBuffer;	// 发送缓冲区
	KRingBuffer* m_pRecvBuffer;	// 接收缓冲区
	CHAR*		 m_pszSendBuffer;
	CHAR*		 m_pszRecvBuffer;
#ifdef WIN32
	HANDLE		 m_hRecvHandle;
	HANDLE		 m_hSendHandle;
#else
	int			 m_nRecvHandle;
	int			 m_nSendHandle;
#endif
};

#endif //__KIPCSTREAM_H__
