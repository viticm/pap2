/************************************************************************/
/* 剑三角色数据结构定义			                                        */
/* Copyright : Kingsoft 2005										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2005.10.21	Create												*/
/* Comment	 :															*/
/*		本文件只定义角色数据的总体结构＆Base中的基本数据结构			*/
/*		其他数据块内部的结构在各自的读写函数中定义即可					*/
/************************************************************************/
/* 存盘数据结构描述:													*/
/*		KRoleDBData														*/
/*			KRoleDataHeader				整个存盘数据的头结构			*/
/*			WORD wBlockCount			数据块块数						*/
/*			KRoleDataBlock BlockList[0]	数据块的描述列表				*/
/*				byBlockType				数据块类型						*/
/*				wDataLen;				数据块长度						*/
/*				byReserved;				保留							*/
/*			分布连续的数据块											*/
/************************************************************************/
#ifndef _KROLE_DB_DATA_DEF_H_
#define _KROLE_DB_DATA_DEF_H_

#include "SO3ProtocolBasic.h"

#define ROLE_DATA_VERSION			1
#define MAX_PLAYER_DB_DATA_SIZE		60000		//角色数据最大容量

#pragma	pack(1)

struct KRoleDataHeader
{
	//(一个)角色存档数据的总大小，值大小小于PLAYER_SAVE_DATA_SIZE_LIMIT
	//从TRoleData结构起始处就开始计算拉
	DWORD		dwDataLen;
	//(该)角色存档数据的CRC校验数值，计算产生该数值的缓冲区范围为：从该dwCRC数值之后TRoleData::wVersion开始
	//长度为TRoleData->dwDataLen - sizeof(TRoleData->dwDataLen) - sizeof(TRoleData->dwCRC)
	DWORD		dwCRC;
	//存档数据版本
	WORD		wVersion;
	//----上面dwDataLen、dwCRC、wVersion位置固定，
	//如要调整，需要改变上述数据关联的规则，以及ROLE_SAVE_DATA_CRC_BEGIN_OFFSET定义。----
};

// 如果要增加数据块，并且希望兼容以前数据的话，必须在后面加，不能中间插入。
enum ROLEDATA_BLOCK_TYPE
{
	rbtInvalid = 0,

	rbtBaseData,
	rbtSkillList,
	rbtItemList,
	rbtQuestList,
	rbtProfessionList,
	rbtRecipeList,
    rbtAttribData,
    rbtBuffList,
	rbtRepute,
    rbtDeathInfo,

	rbtTotal
};

struct KRoleDataBlock
{
	BYTE	byBlockType;
	WORD	wDataLen;
	BYTE	byReserved;
};

struct KRoleDBData : KRoleDataHeader
{
	WORD				wBlockCount;		//数据块总数
	KRoleDataBlock		BlockList[0];		//角色数据块列表，第一块必定是BaseData
};

//基本角色数据，Bishop＆GameCenter上用到的数据
struct KBaseData
{
	char	szAccount[_NAME_LEN];
	char	szRoleName[_NAME_LEN];
	DWORD	dwPlayerID;
	char    cRoleType;  // see ROLE_TYPE;
	DWORD	dwMapID;
	DWORD	dwMapCopyIndex;
	int		nX;
	int		nY;
	int		nZ;
    BYTE    byFaceDirection;
	int		nReturnPosIndex;
	BYTE	byLevel;
    bool    bAlive;
    int     nExperience;
    WORD    wRepresentID[REPRESENT_COUNT];
	int		nDoubleExp;
	time_t	lLastSaveTime;
};

//角色属性数据
struct KRoleAttribData
{
	int		nCurrentLife;				
	int		nCurrentMana;				
    int     nCurrentStamina;
    int     nCurrentRage;
};

struct KROLE_GENERATOR_PARAM
{
	char	szAccountName[_NAME_LEN];
	char	szRoleName[_NAME_LEN];
	char    cRoleType;
    DWORD   dwMapID;
    DWORD   dwMapCopyIndex;
    WORD    wRepresentID[REPRESENT_COUNT];
};

struct KROLE_REVIVE_INFO
{
    time_t  nLastDieTime;
    time_t  nLastSituReviveTime;
    WORD    wSituReviveCounter;
    DWORD   dwKillerID;
    int     nRevivePosIndex;
};

struct KItemData
{

};

struct KSkillData
{

};

#pragma pack()

#endif	//_KROLE_DB_DATA_DEF_H_
