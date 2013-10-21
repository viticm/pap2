

#ifndef _KTESTDBMANAGER_H_
#define _KTESTDBMANAGER_H_

#include "mysql.h"


#define MAX_ASYNC_DB_SQL_SIZE    (65536 * 2)
#define _NAME_LEN 32
class KTestDBManager 
{
public:
    KTestDBManager();
	~KTestDBManager();

    BOOL Init();
    
	BOOL ClearTable(const char* szTableName);
	BOOL DeleteDB(const char* szDBName);
	BOOL InsertRoleInDB(const char* pszRoleGroup, const char* pszRoleName, void* pRoleInfoBuffer, int nBufferLength);
	BOOL UpDateFellowShipData(DWORD dwPlayerID, byte byData[], size_t uDatalength);
    BOOL LoadMapCopyInfo(DWORD dwMapID, __int64 nMapCopyIndex, BYTE** ppbyData, size_t& uDataLen);
    IKG_Buffer* GetRoleData(const char szTablename[], const char szIndex[], const char szCondition[], const char szRoleName[]);
    IKG_Buffer* GetRoleData(const char szTablename[], const char szIndex[], const char szCondition[], DWORD dwRoleID);
    int AccountRoleCount(const char szAccount[]);
	IKG_Buffer* QueyRueslt(const char szSQL[]);
private:
	BOOL _CheckDB();
    BOOL _CheckCaseInsensitive();
    BOOL _CheckCharacterSet();
    BOOL _CheckTables();
    BOOL _DoQuey(const char cszSQL[], unsigned long ulLength);
    BOOL DoQuery(MYSQL_STMT* pStmt);
    
private:	
    MYSQL*      m_pDBHandle;
    char       m_szSQL[MAX_ASYNC_DB_SQL_SIZE]; // (size == MAX_ASYNC_DB_SQL_SIZE + 1)
	char       m_szSQL2[MAX_ASYNC_DB_SQL_SIZE]; // (size == MAX_ASYNC_DB_SQL_SIZE + 1)
    char        m_szGroupName[_NAME_LEN];
    char        m_szTransformedGroupName[_NAME_LEN * 2];
    
};
extern  KTestDBManager g_DBManager;
#endif //_KTESTDBMANAGER_H_
