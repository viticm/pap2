#ifndef FILE_K_BIN_TAB_H
#define FILE_K_BIN_TAB_H

// From leoman wKdb

// binary table format
// Hdr
// row 0: field format type 
// row >= 1 : values
// text

// Binary table file

// TODO: pack the same string in text field

class KBinTab
{
public:
	enum 
	{
		FIELD_FORMAT_NULL = 0,
		FIELD_FORMAT_INT = 'i',
		FIELD_FORMAT_FLOAT = 'f',
		FIELD_FORMAT_STRING	= 's',
		FIELD_FORMAT_SLASHES_STRING = 'S',	// NOTE: 二进制格式表格不需要有转义符
        FIELD_FORMAT_PATH = 'p',
	};

public:
	KBinTab();
	~KBinTab();

    BOOL Load(LPCSTR szFilePath, LPCSTR szFormat);
	BOOL Save(LPCSTR szFilePath);
	void* Data(size_t& nRow);

	static size_t TripSlashes(char* szBuffer);

private:
	KBinTab(KBinTab const& rhs);
	KBinTab& operator=(KBinTab const& rhs);

	BOOL LoadBinTab(LPCSTR szFilePath);
    BOOL LoadTxtTab(LPCSTR szFilePath, LPCSTR szFormat);

	BOOL SaveBinTab(LPCSTR szFilePath);
	BOOL SaveTxtTab(LPCSTR szFilePath);

	BOOL LoadTxtTab(char* szBuffer, size_t nSize, LPCSTR szFormat);

	BOOL LoadTxtRow(char* szBuffer, size_t nSize);
	size_t GetTxtRowLength(char* szBuffer, size_t nSize, size_t& nDelimiter);

	BOOL LoadTxtCell(char* szBuffer, size_t nSize, size_t nColumn);
	size_t GetTxtCellLength(char* szBuffer, size_t nSize, size_t& nDelimiter);

    int CountColumn(char szBuffer[], size_t uSize, int *pnRetColumn);
    int CountRow(char szBuffer[], size_t uSize, int *pnRetRow);

    BOOL ToStringOffset();
    BOOL ToStringPointer();

private:
	BYTE* m_pData;
};

#endif // FILE_K_BIN_TAB_H