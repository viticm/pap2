#ifndef _INCLUDE_IKG_TABFILE_H_
#define _INCLUDE_IKG_TABFILE_H_

namespace UI
{
    enum KG_FILE_OPEN_MODE
    {
        KG_FILE_OPEN_MODE_BEGIN = 0,

        KG_FILE_OPEN_NORMAL = KG_FILE_OPEN_MODE_BEGIN,
        KG_FILE_OPEN_FILE_MAPPING,

        KG_FILE_OPEN_MODE_COUNT
    };

    class IKG_TabFile
    {
    public:
        typedef int (*FUNC_COMPARE)(const void* pvLValue, const void* pvRValue);

    public:
        virtual ULONG Release() = 0;

        virtual size_t GetRowCount() = 0;

        virtual int Search(
            void *pvKey,
            FUNC_COMPARE fnCompare,
            void *pvRetResult,
            unsigned uLength
        ) = 0;

        virtual int GetRow(unsigned uIndex, void *pvRetRow, unsigned uLength) = 0;
    };

    IKG_TabFile* OpenTabFile(
        const char *pcszFilePath,
        const char *pcszFormat,
        KG_FILE_OPEN_MODE emFileOpenMode
    );
}
#endif // _INCLUDE_IKG_TABFILE_H_