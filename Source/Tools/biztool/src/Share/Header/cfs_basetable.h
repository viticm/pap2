//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-12 18:19
//      File_base        : cfs_basetable
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : 
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__BASETABLE___H____
#define _CFS__BASETABLE___H____

//////////////////////////////////////////////////////////////////////////
// Include region
#include "cfs_db_result.h"


//////////////////////////////////////////////////////////////////////////
// Macro define region
#define MAX_CFSDBTABLENAME_SIZE			32
#define MAX_CFSDBTABLECOLNAME_SIZE		32
#define MAX_CFSDBTABLECHARSETNAME_SZIE	32
#define MAX_CFSDBTABLEKEYNAME_SZIE		32


//////////////////////////////////////////////////////////////////////////
// class declare region
class CFS_BASEDB;


//////////////////////////////////////////////////////////////////////////
// Struct define region
typedef struct tagCFSDBTABLECOLINFO
{
	char szName[MAX_CFSDBTABLECOLNAME_SIZE+1];
	char szDefaultCharSet[MAX_CFSDBTABLECHARSETNAME_SZIE+1];
	enum_field_types enColDataType;
	int nDataSize;
	unsigned int unColAttr;
	enum_field_key_types enColKeyType;
	char szColKeyName[MAX_CFSDBTABLEKEYNAME_SZIE+1];
	byte bConstantData;
}CFSDBTABLECOLINFO, *PCFSDBTABLECOLINFO;

typedef struct tagCFSDBTABLECOLITEM
{
	char szName[MAX_CFSDBTABLECOLNAME_SIZE+1];
	byte bUnsigned;
	enum_field_types enColDataType;
	void *pColData;
	int nColDataSize;
}CFSDBTABLECOLITEM, *PCFSDBTABLECOLITEM;

typedef struct tagCFSDBTABLECOLTITLE
{
	char szName[MAX_CFSDBTABLECOLNAME_SIZE+1];
}CFSDBTABLECOLTITLE, *PCFSDBTABLECOLTITLE;

//////////////////////////////////////////////////////////////////////////
// CFS_BASETABLE class define region

class CFS_BASETABLE
{
public:
	CFS_BASETABLE(const char *pcTableName);
	virtual ~CFS_BASETABLE();

	C_BOOL OpenTable(CFS_BASEDB *pDB, C_BOOL bCreateIfNoExist = C_TRUE);
	void CloseTable();
	C_BOOL IsOpened();

	C_BOOL AddLine(const CFSDBTABLECOLITEM *pcColItemList, 
		int nColItemCount);
	C_BOOL DeleteLines(const char *pcKeyTitle, 
		const char *pcKeyValue);					// Only support ascii string key
	C_BOOL DeleteLines(const char *pcKeyTitle, 
		unsigned int unKeyValue);					// Only support integer key
	C_BOOL UpdateCols(const char *pcKeyTitle, const char *pcKeyValue, 
		const CFSDBTABLECOLITEM *pcColItemList, 
		int nColItemCount);							// Only support ascii string key
	C_BOOL FindCols(const char *pcKeyTitle, 
		const char *pcKeyValue, 
		CFS_DBRESULT *pclsResult, 
		const CFSDBTABLECOLTITLE *pcColTitleList = NULL/* NULL means return all col data */, 
		int nColTitleCount = 0, 
		C_BOOL bStoreRetToClient = C_FALSE);		// Only support ascii string key

	const char *GetTableName();

	int ConstructAddLineSQL(const CFSDBTABLECOLITEM *pcColItemList, 
		int nColItemCount, char *pSQLBuffer);
	int ConstructUpdateColsSQL(const char *pcKeyTitle, 
		const char *pcKeyValue, 
		const CFSDBTABLECOLITEM *pcColItemList, 
		int nColItemCount, char *pSQLBuffer);		// Only support ascii string key

	// pure virtual functions
	virtual int GetTableColNum() = 0;
	virtual const CFSDBTABLECOLINFO *GetTableColInfoList(int *pnColNum = NULL) = 0;

protected:
	char *m_pTempSQLBuff;
	CFS_BASEDB *m_pDB;
	char m_szTableName[MAX_CFSDBTABLENAME_SIZE+1];

	void _FillColItem(CFSDBTABLECOLITEM *pColItem, 
		int nColIndex, void *pColData, int nColDataSize);

private:
	C_BOOL m_bOpened;

	int __ConstructRightConditionValue(const CFSDBTABLECOLITEM *pColItem, char *pValueBuffer);
};

// Public inline
inline
C_BOOL CFS_BASETABLE::IsOpened()
{
	return m_bOpened;
}

inline 
const char *CFS_BASETABLE::GetTableName()
{
	return m_szTableName;
}

// Protected inline
inline 
void CFS_BASETABLE::_FillColItem(CFSDBTABLECOLITEM *pColItem, 
								 int nColIndex, 
								 void *pColData, 
								 int nColDataSize)
{
	const CFSDBTABLECOLINFO *pcColInfo = &(GetTableColInfoList()[nColIndex]);
	strcpy(pColItem->szName, pcColInfo->szName);
	pColItem->enColDataType = pcColInfo->enColDataType;
	pColItem->bUnsigned = ((pcColInfo->unColAttr & enCFS_DBCOL_ATTR_UNSIGNED) > 0);
	pColItem->pColData = pColData;
	pColItem->nColDataSize = nColDataSize;
}

// Private inline

#endif // _CFS__BASETABLE___H____
