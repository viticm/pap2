//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDoodadFile.h
//  Version     :   1.0
//  Creater     :   Zhao Chunfeng
//  Date        :   2008-05-28 15:11:19
//  Comment     :   Dooodad file 
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KDOOODADFILE_H_
#define _KDOOODADFILE_H_

#include <list>
#include "IDoodadFile.h"

class KDoodadFile : public IDoodadFile
{
public:
    KDoodadFile();
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG   STDMETHODCALLTYPE AddRef();
    ULONG   STDMETHODCALLTYPE Release();

public:
    BOOL Push(const KDOODAD_DATA& crDoodadInfo);
    BOOL Save(const char cszName[]);

    BOOL Load(const char cszName[]);
    BOOL Pop(KDOODAD_DATA* pDoodadInfo);

    int  GetCount() { return (int)m_DataTable.size(); }

private:
    typedef std::list<KDOODAD_DATA> KDOODAD_DATA_TABLE;
    KDOODAD_DATA_TABLE  m_DataTable;
    ULONG               m_uRefCount;
};

#endif
