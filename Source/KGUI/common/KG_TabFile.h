#ifndef _INCLUDE_KG_TABFILE_H_
#define _INCLUDE_KG_TABFILE_H_

#include "kbintab.h"
#include "IKG_TabFile.h"

namespace UI
{
    class KG_TabFile : public IKG_TabFile
    {
    public:
        friend IKG_TabFile* OpenTabFile(
            const char *pcszFilePath,
            const char *pcszFormat,
            KG_FILE_OPEN_MODE emFileOpenMode
        );

    public:
        ULONG Release();

        size_t GetRowCount();

        int Search(
            void *pvKey,
            FUNC_COMPARE fnCompare,
            void *pvRetResult,
            unsigned uLength
        );
        int GetRow(unsigned uIndex, void *pvRetRow, unsigned uLength);
        
    protected:
        int OffsetToPointer(void *pvRow);

    protected:
        KG_TabFile();
        virtual ~KG_TabFile();

    protected:
        KG_FILE_OPEN_MODE m_emFileOpenMode;
        size_t m_uRowCount;
        size_t m_uColumnCount;
        char *m_pszFormat;
        SYSTEM_INFO m_SysInfo;

        HANDLE m_hFileMap;
        char *m_pszText;
        char *m_pszMapViewText;

        KBinTab *m_pBinTab;
        BYTE *m_pbyData;

        DWORD m_dwLastError;
    };
}

#endif // _INCLUDE_KG_TABFILE_H_

