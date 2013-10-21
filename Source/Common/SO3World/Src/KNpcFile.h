//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KNpcFile.h
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-28 15:11:19
//  Comment     :   NPC file 
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KNPCFILE_H_
#define _KNPCFILE_H_

#include <list>
#include "INpcFile.h"

class KNpcFile : public INpcFile
{
public:
    KNpcFile();
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG   STDMETHODCALLTYPE AddRef();
    ULONG   STDMETHODCALLTYPE Release();

public:
    BOOL Push(const KNPC_DATA& crNpcInfo);
    BOOL Save(const char cszName[]);

    BOOL Load(const char cszName[]);
    BOOL Pop(KNPC_DATA* pNpcInfo);

    int  GetCount() { return (int)m_DataTable.size(); }

private:
    typedef std::list<KNPC_DATA> KNPC_DATA_TABLE;
    KNPC_DATA_TABLE m_DataTable;
    ULONG           m_uRefCount;
};

#endif
