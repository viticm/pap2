#pragma once
#pragma warning(disable: 4819)
#pragma warning(disable: 4996)

#include "d3dx9.h"
//#include "KGPublic.h"
//#include "Engine\KGLog.h"


#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"
#include "./cs/phyexp.h"
#include "./cs/bipexp.h"
#include "particle.h"
#include "iparamb2.h"
#include "iskin.h"
#include "parray.h"
#include "control.h"


enum 
{
    MAX_BONE_NAME_LENGTH = 30,
    MAX_NUM_CHILD_BONE   = 32,
};

#include "asciitok.h"
#include "meshtable.h"
#include <algorithm>
#include <set>
#include <map>


//#include "Engine/KGLog.h"
//#include "Engine/File.h"
#include "KG3DExportOption.h"
#include "Engine/FileType.h"


#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


struct BoneInfoData
{
	TCHAR strBoneName[MAX_BONE_NAME_LENGTH];
	
	DWORD ChildIndices[MAX_NUM_CHILD_BONE];
	
	DWORD dwIndex;
	DWORD dwParentIndex;
	DWORD dwNumChildBones;
	BoneInfoData();
};

#define  ANIMATION_NONE              0
#define  ANIMATION_VERTICES          1
#define  ANIMATION_VERTICES_RELATIVE 2
#define  ANIMATION_BONE              10
#define  ANIMATION_BONE_RELATIVE     11

//////////////////////////////////////////////////////////////////////////
//Add 4-19-2005 Huangshan Add animation type
#define ANIMATION_BONE_RTS    13//12 has been used as low precision matrix
#define ANIMATION_BONE_KEYONLY 14
//////////////////////////////////////////////////////////////////////////

enum AnimationType
{
	ANIMATION_TYPE_NONE = 0,
	ANIMATION_TYPE_VERTICES,
	ANIMATION_TYPE_VERTICES_RELATIVE,
	ANIMATION_TYPE_BONE = 10,
	ANIMATION_TYPE_BONE_RELATIVE,
	ANIMAIION_TYPE_BONE_RTS = 13,
	ANIMATION_TYPE_FORCE_DWORD = 0xffffffff
};



#define KGLOG_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))       \
{                       \
	/*KGLogPrintf(        \
	//KGLOG_DEBUG,    \
	//"KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
	//); */                 \
	goto Exit0;         \
	}                       \
	} while (false)


#define KGLOG_OUTPUT_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))       \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
	); */                 \
	}                       \
	} while (false)

#define KGLOG_PROCESS_SUCCESS(Condition) \
	do  \
{   \
	if (Condition)          \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_PROCESS_SUCCESS(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
	); */                 \
	goto Exit1;         \
	}                       \
	} while (false)

#define KGLOG_PROCESS_ERROR_RET_CODE(Condition, Code) \
	do  \
{   \
	if (!(Condition))       \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_PROCESS_ERROR_RET_CODE(%s, %d) at line %d in %s\n", \
#Condition, Code, __LINE__, KG_FUNCTION                    \
	);   */               \
	nResult = Code;     \
	goto Exit0;         \
	}                       \
	} while (false)

#define KGLOG_PROCESS_ERROR_RET_COM_CODE(Condition, Code) \
	do  \
{   \
	if (!(Condition))       \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_PROCESS_ERROR_RET_CODE(%s, %d) at line %d in %s\n", \
#Condition, Code, __LINE__, KG_FUNCTION                    \
	);      */            \
	hrResult = Code;    \
	goto Exit0;         \
	}                       \
	} while (false)


#define KGLOG_COM_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (FAILED(Condition))  \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_COM_PROCESS_ERROR(0x%X) at line %d in %s\n", Condition, __LINE__, KG_FUNCTION  \
	);*/                  \
	goto Exit0;         \
	}                       \
	} while (false)


#define KGLOG_COM_PROCESS_SUCCESS(Condition)   \
	do  \
{   \
	if (SUCCEEDED(Condition))   \
{                           \
	/*KGLogPrintf(            \
	KGLOG_DEBUG,        \
	"KGLOG_COM_PROCESS_SUCCESS(0x%X) at line %d in %s\n", Condition, __LINE__, KG_FUNCTION  \
	);*/                      \
	goto Exit1;             \
	}                           \
	} while (false)


// KG_COM_PROCESS_ERROR_RETURN_ERROR
#define KGLOG_COM_PROC_ERR_RET_ERR(Condition)  \
	do  \
{   \
	if (FAILED(Condition))      \
{                           \
	/*KGLogPrintf(            \
	KGLOG_DEBUG,        \
	"KGLOG_COM_PROC_ERR_RET_ERR(0x%X) at line %d in %s\n", Condition, __LINE__, KG_FUNCTION  \
	);     */                 \
	hrResult = Condition;   \
	goto Exit0;             \
	}                           \
	} while (false)


#define KGLOG_COM_PROC_ERROR_RET_CODE(Condition, Code)     \
	do  \
{   \
	if (FAILED(Condition))      \
{                           \
	/*KGLogPrintf(            \
	KGLOG_DEBUG,        \
	"KGLOG_COM_PROC_ERROR_RET_CODE(0x%X, 0x%X) at line %d in %s\n", Condition, Code, __LINE__, KG_FUNCTION  \
	);  */                    \
	hrResult = Code;        \
	goto Exit0;             \
	}                           \
	} while (false)

#define KGLOG_CHECK_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))       \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_CHECK_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
	);  */                \
	}                       \
	} while (false)

#define KGLOG_COM_CHECK_ERROR(Condition) \
	do  \
{   \
	if (FAILED(Condition))       \
{                       \
	/*KGLogPrintf(        \
	KGLOG_DEBUG,    \
	"KGLOG_COM_CHECK_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, KG_FUNCTION  \
	);   */               \
	}                       \
	} while (false)



#define KG_PROCESS_ERROR(Condition) \
	do  \
	{   \
	if (!(Condition))   \
	goto Exit0;     \
	} while (false)


#define KG_PROCESS_SUCCESS(Condition) \
	do  \
	{   \
	if (Condition)      \
	goto Exit1;     \
	} while (false)



#define KG_PROCESS_ERROR_RET_CODE(Condition, Code) \
	do  \
	{   \
	if (!(Condition))       \
		{                       \
		nResult = Code;     \
		goto Exit0;         \
		}                       \
	} while (false)

#define KG_PROCESS_ERROR_RET_COM_CODE(Condition, Code) \
	do  \
	{   \
	if (!(Condition))       \
		{                       \
		hrResult = Code;     \
		goto Exit0;         \
		}                       \
	} while (false)


#define KG_COM_PROCESS_ERROR(Condition) \
	do  \
	{   \
	if (FAILED(Condition))  \
	goto Exit0;         \
	} while (false)


#define KG_COM_PROCESS_SUCCESS(Condition)   \
	do  \
	{   \
	if (SUCCEEDED(Condition))   \
	goto Exit1;             \
	} while (false)


// KG_COM_PROCESS_ERROR_RETURN_ERROR
#define KG_COM_PROC_ERR_RET_ERR(Condition)  \
	do  \
	{   \
	if (FAILED(Condition))      \
		{                           \
		hrResult = Condition;   \
		goto Exit0;             \
		}   \
	} while (false)


#define KG_COM_PROC_ERROR_RET_CODE(Condition, Code)     \
	do  \
	{   \
	if (FAILED(Condition))  \
		{                       \
		hrResult = Code;    \
		goto Exit0;         \
		}                       \
	} while (false)


#define KG_COM_RELEASE(pInterface) \
	do  \
	{   \
	if (pInterface)                 \
		{                               \
		(pInterface)->Release();    \
		(pInterface) = NULL;        \
		}                               \
	} while (false)


#define KG_DELETE_ARRAY(pArray)     \
	do  \
	{   \
	if (pArray)                 \
		{                           \
		delete [](pArray);      \
		(pArray) = NULL;        \
		}                           \
	} while (false)


#define KG_DELETE(p)    \
	do  \
	{   \
	if (p)              \
		{                   \
		delete (p);     \
		(p) = NULL;     \
		}                   \
	} while (false)

#define KG_CHECK_ERROR(Condition) \
	do  \
	{   \
	if (!(Condition))       \
		{                       \
		}                       \
	} while (false)


#define KG_COM_CHECK_ERROR(Condition) \
	do  \
	{   \
	if (FAILED(Condition))       \
		{                       \
		}                       \
	} while (false)
