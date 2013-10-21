//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KDumpType.h    
//  Version     :   1.0
//  Creater     :   DengWenfeng
//  Date        :   2009-9-7 19:24
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KDumpType_h__
#define _KDumpType_h__

#define IP_LENGTH           32
#define KEY_LENGTH          33
#define VERSION_LENGTH      32

/*
这里定义的操作，用于给exe那边获取参数，因为参数比较多，自己只获取自己感兴趣的，其他的就别获取了。新增的操作可以往后添加，
若参数不够，可以在后面添加,切记不要把前面的删除了
*/
enum DUMP_OP
{
    DUMP_OP_NONE = 0x0000,
    DUMP_OP_NOT_CAPTURE_BY_DUMP = 0x0001,
    DUMP_OP_CAPTURE_BY_DUMP = 0x0001 << 1,
    DUMP_OP_PLAYER_LOGIN = 0x0001 << 2
};

typedef struct  _DUMP_MESSAGE_HEAD
{
    DWORD dwDumpOP;
    DWORD dwBufferSize;    
} DUMP_MESSAGE_HEAD;

typedef struct _DUMP_BASE
{
    TCHAR   szServerIP[IP_LENGTH];
    int     nServerPort;
    TCHAR   szProductVersion[IP_LENGTH];
    TCHAR   szProductVersionEx[IP_LENGTH];
    TCHAR   szDumpKey[KEY_LENGTH];
    TCHAR   szDumpModule[IP_LENGTH];
    TCHAR   szAppFullName[MAX_PATH];
    TCHAR   szDumpPath[MAX_PATH];
    TCHAR   szAccount[IP_LENGTH];
    TCHAR   szRoleName[IP_LENGTH];
    TCHAR   szRegionName[IP_LENGTH * 2];
    TCHAR   szServerName[IP_LENGTH * 2];
    TCHAR   szVideoCard[IP_LENGTH * 2];
    TCHAR   szDriver[IP_LENGTH];
    TCHAR   szDriverVersion[IP_LENGTH];
    DWORD   dwTotalPhysMem;
    TCHAR   szCPUBrand[IP_LENGTH * 2];
    TCHAR   szGUID[KEY_LENGTH];
} DUMP_BASE;

typedef struct _DUMP_FILE
{
    TCHAR   szDumpFile[MAX_PATH];
    TCHAR   szEnvInfoFile[MAX_PATH];
    TCHAR   szConfigFile[MAX_PATH];
    TCHAR   szLogFile[MAX_PATH];
    TCHAR   szGUID[KEY_LENGTH];
} DUMP_FILE;

#endif // _KDumpType_h__