////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KProtocolRecorder.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng,XiaYong
//  Create Date : 2008-7-25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KPROTOCOLRECORDER_H_ 
#define _KPROTOCOLRECORDER_H_ 
struct IProtocolRecorder : IUnknown
{
    virtual BOOL Push(void* pvData, size_t uDataLen) = 0;
};

class KProtocolRecorder : public IProtocolRecorder
{
public:
    KProtocolRecorder(const char szDirName[]) : m_nFileIndex(0), m_ulRefCount(1), m_nRecordFile(-1), m_uFileSize(0)
    {
        memcpy(m_szDirName, szDirName, sizeof(m_szDirName));
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG   STDMETHODCALLTYPE AddRef(void);
    ULONG   STDMETHODCALLTYPE Release(void);

    BOOL Push(void* pvData, size_t uDataLen);

private:
    BOOL OpenFileByIndex();

private:
    char    m_szDirName[64];
    int     m_nFileIndex;
    int     m_nRecordFile;
    size_t  m_uFileSize;
    ULONG   m_ulRefCount;
};
#endif


