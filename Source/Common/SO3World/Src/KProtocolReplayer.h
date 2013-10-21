////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KProtocolReplayer.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng,XiaYong
//  Create Date : 2008-7-28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KPROTOCOLREPLAYER_H_ 
#define _KPROTOCOLREPLAYER_H_ 

#define KREPLAY_CACHE_SIZE  (1024 * 1024)

struct IProtocolReplayer : IUnknown
{
    virtual BOOL Pop(void* pvBuffer, size_t uBufferSize) = 0;
};

class KProtocolReplayer : public IProtocolReplayer
{
public:
    KProtocolReplayer(const char cszDirName[]) : 
      m_ulRefCount(1), m_nFileIndex(0), m_nRecordFile(-1),
      m_uCacheCurPos(0), m_uCacheEndPos(0)
    {
        memcpy(m_szDirName, cszDirName, sizeof(m_szDirName));
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG   STDMETHODCALLTYPE AddRef(void);
    ULONG   STDMETHODCALLTYPE Release(void);

    int Pop(void* pvBuffer, size_t uBufferSize);

private:
    BOOL Cache();
    BOOL OpenFileByIndex();

private:
    BYTE            m_byCache[KREPLAY_CACHE_SIZE];
    size_t          m_uCacheCurPos;
    size_t          m_uCacheEndPos;
    int             m_nFileIndex;
    char            m_szDirName[64];
    int             m_nRecordFile;
    ULONG           m_ulRefCount;
};
#endif


