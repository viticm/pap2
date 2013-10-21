#pragma once

#include "KG3DToolsImpl.h"
#include "SO3GlobalDef.h"
#include "IEEditor.h"
#include "KScene.h"

//[原来在SceneEditorDoc.h,cpp
class KLogicalSceneConnection : public IELogicalSceneConnection
{
public:
	KScene* m_lpLogicalScene;
	_KG3DLogicalRegionInfo* m_lpLogicalRegionInfo;
	virtual HRESULT BindLogicalScene(KScene* pScene)
	{
		m_lpLogicalScene = pScene;
		return S_OK;
	}
	virtual BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);
	virtual KRegion* GetRegion(int nRegionX, int nRegionY);

	KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		m_lpLogicalRegionInfo = NULL;
	}

	~KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		SAFE_DELETE_ARRAY(m_lpLogicalRegionInfo);
	}
	HRESULT CreateLogicalRegionInfo();
};
//]


class KNpc;
class IEKG3DSceneSceneEditor;
enum EventDocLogical
{
	EVENTDOCLOGIGAL_UPDATEUI,
	EVENTDOCLOGIGAL_WAYPOINT_ADDED,
	EVENTDOCLOGIGAL_WAYPOINT_DELETED,
	EVENTDOCLOGIGAL_WAYPOINT_MOVED,
	EVENTDOCLOGIGAL_WAYPOINT_ROTATED,

	EVENTDOCLOGIGAL_REFRESHPOINT_ADDED,
	EVENTDOCLOGIGAL_REFRESHPOINT_DELETED,
	EVENTDOCLOGIGAL_REFRESHPOINT_MOVED,
	EVENTDOCLOGIGAL_REFRESHPOINT_ROTATED,

	EVENTDOCLOGIGAL_TRAFFICPOINT_ADDED,
	EVENTDOCLOGIGAL_TRAFFICPOINT_MOVED,
	EVENTDOCLOGIGAL_TRAFFICPOINT_DELETED,

};

typedef int(*FUNC_LOADPROGRESS)(LPCTSTR pName,float a); 

struct IEKG3DRepresentPoly;


/*!
\fn splitStr(std::list<std::string>& l, const std::string& seq, char s1, char s2, bool keeptok)
\brief Splits a given std::string ( as param 'seq' ) into token separated by one starting character token and ending the token with a second given separator character.
\param std::list<std::string>& reference to a string list that will receives all the resulting token
\param std::string seq which is the string stream to split
\param char s1 the first separator character that will start the token to be put into the resulting list
\param char s2 the ending separator that will finish the token
\param bool keeptok - optional boolean that is to be given TRUE if the separator characters are needed to be part of the tokens
\return integer that has the number of token in the resulting list
*/
int splitStr(std::list<std::string>& l, const std::string& seq, char s1, char s2, bool keeptok);
/**
\fn splitStr(std::list<std::string>& l, const std::string& seq, const std::string& _1cdelim, bool keeptoken=false, bool _removews=true )
\brief Splits a string into tokens separeted by supplied delimiters as a std::string.
\param std::list<std::string>& L reference to the resulting string tokens
\param std::string seq The string stream to split
\param std::string _lcdelim - a std::string that contains all of the single delimiters
\param bool keeptok -- same as the above function
\param bool removews -- Set to TRUE if requiered to remove white space characters ( space, "\n\r" etc...)
\return integer that has the number of token in the resulting list
*/
int splitStr(std::list<std::string>& L, const std::string& seq, const std::string& _1cdelim, bool keeptoken, bool _removews );

__int64 Str2Int64(char* str);

#define EXCHANGE_OBJ_STR(field) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(logicInfo->field.c_str()); \
	} \
	else \
	{ \
		string _str(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
		logicInfo->field = _str; \
	}

#define EXCHANGE_OBJ_STR_MEM(field, cppfield) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(logicInfo->cppfield.c_str()); \
	} \
	else \
	{ \
		string _str(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
		logicInfo->cppfield = _str; \
	}

#define EXCHANGE_OBJ_INT(field) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%d", logicInfo->field); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		logicInfo->field = atoi(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
	}

#define EXCHANGE_OBJ_INT_MEM(field, cppfield) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%d", logicInfo->cppfield); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		logicInfo->cppfield = atoi(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
	}

#define EXCHANGE_OBJ_INT_MEM_MEM_0_TO_MINUS1(field, cppfield, submember) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%d", logicInfo->cppfield ? logicInfo->cppfield->submember : -1); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		int ivalue = atoi(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
		if (ivalue != -1) \
			logicInfo->cppfield->submember = ivalue; \
	}

#define EXCHANGE_OBJ_FLO(field) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%f", logicInfo->field); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		logicInfo->field = atof(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
	}

#define EXCHANGE_OBJ_FLO_MEM_MEMS(field, cppfield, submember) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%f", logicInfo->cppfield.submember); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		logicInfo->cppfield.submember = atof(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
	}

#define EXCHANGE_OBJ_VEC(field) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%f,%f,%f", logicInfo->field.x, logicInfo->field.y, logicInfo->field.z); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		string _str(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
		list<string> _strlist; \
		string _sp(","); \
		splitStr(_strlist, _str, _sp, false, true); \
		list<string>::iterator _it = _strlist.begin(); \
		string _sub = *_it++; \
		logicInfo->field.x = atof(_sub.c_str()); \
		_sub = *_it++; \
		logicInfo->field.y = atof(_sub.c_str()); \
		_sub = *_it; \
		logicInfo->field.z = atof(_sub.c_str()); \
	}

#define EXCHANGE_OBJ_QUA(field) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		sprintf(__szTemp##field, "%f,%f,%f,%f", logicInfo->field.x, logicInfo->field.y, logicInfo->field.z, logicInfo->field.w); \
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		string _str(BSTR_TO_STRING(AtlInfo->strValues[__i++])); \
		list<string> _strlist; \
		string _sp(","); \
		splitStr(_strlist, _str, _sp, false, true); \
		list<string>::iterator _it = _strlist.begin(); \
		string _sub = *_it++; \
		logicInfo->field.x = atof(_sub.c_str()); \
		_sub = *_it++; \
		logicInfo->field.y = atof(_sub.c_str()); \
		_sub = *_it++; \
		logicInfo->field.z = atof(_sub.c_str()); \
		_sub = *_it; \
		logicInfo->field.w = atof(_sub.c_str()); \
	}


#define EXCHANGE_OBJ_STRUCT(field, size) \
	if (bGet) \
	{ \
		AtlInfo->strFields[__i  ] = STRING_TO_BSTR(#field); \
		TCHAR __szTemp##field[256]; \
		__szTemp##field[0] = '\0'; \
		TCHAR __szBuf##field[64]; \
		for (int i = 0; i < size; i++)\
		{\
			UCHAR c = *(((UCHAR*)&(logicInfo->field)) + i); \
			sprintf(__szBuf##field, "%02X", c); \
			strcat(__szTemp##field, __szBuf##field); \
		}\
		AtlInfo->strValues[__i++] = STRING_TO_BSTR(__szTemp##field); \
	} \
	else \
	{ \
		TCHAR originArray[64]; \
		sprintf(originArray, "%s", AtlInfo->strValues[__i++]); \
		TCHAR copyArray[11]; \
		copyArray[0] = '0'; \
		copyArray[1] = 'x'; \
		copyArray[2] = originArray[6]; \
		copyArray[3] = originArray[7]; \
		copyArray[4] = originArray[4]; \
		copyArray[5] = originArray[5]; \
		copyArray[6] = originArray[2]; \
		copyArray[7] = originArray[3]; \
		copyArray[8] = originArray[0]; \
		copyArray[9] = originArray[1]; \
		copyArray[10] = '\0'; \
		logicInfo->field = (LONG)Str2Int64(copyArray); \
	}


interface IEKSceneEditorDocLogical KG_INTERFACE_ROOT
{
	KG_BEGIN_INTERFACE_PURE
	virtual HRESULT KG_METHODCALLTYPE GetObjCount(BSTR ObjType, LONG fatherindex, LONG* ObjCount) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetObjDisplayInfo(BSTR ObjType, LONG index, LONG fatherindex, BSTR* name, BSTR* nickname, LONG* hasscript, LONG* representObjPtr, LONG* logicObjPtr) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetSelectedLogicObjPtr(LONG index, LONG* logicObjPtr, BSTR* ObjType) PURE;	
	virtual HRESULT KG_METHODCALLTYPE GetSetObjInfo(BSTR ObjType, AtlObjInfo* AtlInfo, LONG LogicObj, LONG bGet) PURE;
	virtual HRESULT KG_METHODCALLTYPE UpdateObjRepresentByLogicObj(AtlObjInfo* objInfo, LONG logicObj) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneWayPointSet(LONG* ppNewSet, BSTR pName) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteWayPointSet(LONG pSet) PURE;
	virtual HRESULT KG_METHODCALLTYPE AddOneWayPoint(LONG wayPointSetID, BSTR pName) PURE;
	virtual HRESULT KG_METHODCALLTYPE RefreshWayPoint(LONG wayPointPtr, INT nType) PURE;
	virtual HRESULT KG_METHODCALLTYPE AddOneBigTrafficPoint() PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneTrafficPointSet(int trafficID, INT bAddPoint) PURE;
	virtual HRESULT KG_METHODCALLTYPE DeleteTrafficPointAndSet(LONG pSelectedObject, INT bDeletePoint) PURE;
	virtual HRESULT KG_METHODCALLTYPE InsertTrafficPoint() PURE;
	//virtual HRESULT KG_METHODCALLTYPE SelectLogicObject(LONG pSelectedObject, BSTR objectType) PURE;
	virtual HRESULT KG_METHODCALLTYPE RefreshTraffic() PURE;
	virtual HRESULT KG_METHODCALLTYPE RefreshWay() PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedNewOneNpcReviveGroup(LONG* dwID, BSTR pName, int nMin, int nMax, BSTR npcString) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedNewOneDoodadReviveGroup(LONG* dwID, BSTR pName, int nMin, int nMax, BSTR doodadString) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteNpcReviveGroup(LONG dwID) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteDoodadReviveGroup(LONG dwID) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetHatredGroupInfo(INT index, INT* groupID, BSTR* groupName) PURE;
	virtual HRESULT KG_METHODCALLTYPE AddOneNpcHatredGroup(BSTR groupName) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetAIGroupInfo(BSTR* infoString) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneAIGroup(BSTR groupName, INT patrolPathID, INT npcOrderID, BOOL reviveTeammate, BSTR npcString, BSTR orderString) PURE;
	virtual HRESULT KG_METHODCALLTYPE ClearNpcAIGroup(INT npcID) PURE;
	virtual HRESULT KG_METHODCALLTYPE SetAIGroupOrder(INT groupID, INT npcOrderID, BSTR npcString, BSTR orderString) PURE;
	virtual HRESULT KG_METHODCALLTYPE UpdateDoodadReviveGroup(LONG dwID, BSTR doodadString) PURE;
	virtual HRESULT KG_METHODCALLTYPE UpdateNpcReviveGroup(LONG dwID, BSTR npcString) PURE;
	virtual HRESULT KG_METHODCALLTYPE DeleteSelectedObject() PURE;
	virtual HRESULT KG_METHODCALLTYPE DeleteHatredGroup(INT groupID) PURE;
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteAIGroup(INT groupID) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetNpcReviveGroupInfo(LONG dwID, int* nMin, int* nMax, int* nRandom, BSTR* npcString) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetDoodadReviveGroupInfo(LONG dwID, int* nMin, int* nMax, int* nRandom, BSTR* doodadString) PURE;
	virtual HRESULT KG_METHODCALLTYPE GetAIGroupMemberInfo(INT groupID, BSTR* memberInfo) PURE;	
};

class KSceneEditorDocLogical :
	public IEKSceneEditorDocLogical,
	public CKG3DConnectionPoint<IEKG3DSimpleSink>
{
public:
	KG_BEGIN_INTERFACE
	virtual HRESULT KG_METHODCALLTYPE GetObjCount(BSTR ObjType, LONG fatherindex, LONG* ObjCount);
	virtual HRESULT KG_METHODCALLTYPE GetObjDisplayInfo(BSTR ObjType, LONG index, LONG fatherindex, BSTR* name, BSTR* nickname, LONG* hasscript, LONG* representObjPtr, LONG* logicObjPtr);
	virtual HRESULT KG_METHODCALLTYPE GetSelectedLogicObjPtr(LONG index, LONG* logicObjPtr, BSTR* ObjType);	
	virtual HRESULT KG_METHODCALLTYPE GetSetObjInfo(BSTR ObjType, AtlObjInfo* AtlInfo, LONG LogicObj, LONG bGet);
	virtual HRESULT KG_METHODCALLTYPE UpdateObjRepresentByLogicObj(AtlObjInfo* objInfo, LONG logicObj);
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneWayPointSet(LONG* ppNewSet, BSTR pName);
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteWayPointSet(LONG pSet);
	virtual HRESULT KG_METHODCALLTYPE AddOneWayPoint(LONG wayPointSetID, BSTR pName);
	virtual HRESULT KG_METHODCALLTYPE RefreshWayPoint(LONG wayPointPtr, INT nType);
	virtual HRESULT KG_METHODCALLTYPE AddOneBigTrafficPoint();
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneTrafficPointSet(int trafficID, INT bAddPoint);
	virtual HRESULT KG_METHODCALLTYPE DeleteTrafficPointAndSet(LONG pSelectedObject, INT bDeletePoint);
	virtual HRESULT KG_METHODCALLTYPE InsertTrafficPoint();
	//virtual HRESULT KG_METHODCALLTYPE SelectLogicObject(LONG pSelectedObject, BSTR objectType);
	virtual HRESULT KG_METHODCALLTYPE RefreshTraffic();
	virtual HRESULT KG_METHODCALLTYPE RefreshWay();
	virtual HRESULT KG_METHODCALLTYPE DecoratedNewOneNpcReviveGroup(LONG* dwID, BSTR pName, int nMin, int nMax, BSTR npcString);
	virtual HRESULT KG_METHODCALLTYPE DecoratedNewOneDoodadReviveGroup(LONG* dwID, BSTR pName, int nMin, int nMax, BSTR doodadString);
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteNpcReviveGroup(LONG dwID);
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteDoodadReviveGroup(LONG dwID);
	virtual HRESULT KG_METHODCALLTYPE GetHatredGroupInfo(INT index, INT* groupID, BSTR* groupName);
	virtual HRESULT KG_METHODCALLTYPE AddOneNpcHatredGroup(BSTR groupName);
	virtual HRESULT KG_METHODCALLTYPE GetAIGroupInfo(BSTR* infoString);
	virtual HRESULT KG_METHODCALLTYPE DecoratedAddOneAIGroup(BSTR groupName, INT patrolPathID, INT npcOrderID, BOOL reviveTeammate, BSTR npcString, BSTR orderString);
	virtual HRESULT KG_METHODCALLTYPE ClearNpcAIGroup(INT npcID);
	virtual HRESULT KG_METHODCALLTYPE SetAIGroupOrder(INT groupID, INT npcOrderID, BSTR npcString, BSTR orderString);
	virtual HRESULT KG_METHODCALLTYPE UpdateDoodadReviveGroup(LONG dwID, BSTR doodadString);
	virtual HRESULT KG_METHODCALLTYPE UpdateNpcReviveGroup(LONG dwID, BSTR npcString);
	virtual HRESULT KG_METHODCALLTYPE DeleteSelectedObject();
	virtual HRESULT KG_METHODCALLTYPE DeleteHatredGroup(INT groupID);
	virtual HRESULT KG_METHODCALLTYPE DecoratedDeleteAIGroup(INT groupID);
	virtual HRESULT KG_METHODCALLTYPE GetNpcReviveGroupInfo(LONG dwID, int* nMin, int* nMax, int* nRandom, BSTR* npcString);
	virtual HRESULT KG_METHODCALLTYPE GetDoodadReviveGroupInfo(LONG dwID, int* nMin, int* nMax, int* nRandom, BSTR* doodadString);
	virtual HRESULT KG_METHODCALLTYPE GetAIGroupMemberInfo(INT groupID, BSTR* memberInfo);			 

private:
	HRESULT KG_METHODCALLTYPE GetAtlObjRepresentInfo(AtlObjInfo* atlinfo, D3DXVECTOR3* translation, D3DXQUATERNION* rotation, D3DXVECTOR3* scale);

public:
	int m_nSceneID;
	float m_fLogicalXStart;
	float m_fLogicalZStart;
	// ahpho suntao
	map<INT, string> m_npcHatredGroupMap; // npc仇恨组
	int m_npcHatredGroupIndex; // npc仇恨组起始ID
	int m_npcIndex; // npc索引
	string m_SceneName; // 场景名
	// end ahpho

	struct WayPoint
	{
		D3DXVECTOR3 vPosition;
		D3DXQUATERNION vRotation;

		PVOID pPointer;
		int nX;
		int nY;
		int nZ;

		int nStayFrame;
		int nStayDirection;
		int nStayAnimation;
		BOOL bStayAniLoop;
		BOOL bIsRun;
		string szScriptName;
		string szInfo;
	};

	struct WayPointSet
	{
		vector<WayPoint*>vectorWayPoint;
		string szName;
		DWORD dwSetID;
		int nIndex;
		int nOrderID;
		~WayPointSet();
		HRESULT AddWayPoint(WayPoint** ppNewWayPoint,D3DXVECTOR3 vPos,D3DXQUATERNION vRot,PVOID pPointer);
		HRESULT InsertWayPoint(WayPoint** ppNewWayPoint,int nIndex,D3DXVECTOR3 vPos);
		HRESULT DeleteWayPoint(WayPoint* pWayPoint);
		HRESULT DeleteAllWayPoint();
		HRESULT FindWayPointByEditorWayPoint(WayPoint** ppWayPoint,PVOID pPointer);

	};
	DWORD m_dwCurrentWayPointSetID;
	list<WayPointSet*>m_listWayPointSet;

	struct TrafficPoint
	{
		PVOID pPointer;
		D3DXVECTOR3 vPosition;
		DWORD m_dwType;//0 big，1 small
		int m_nLinkID;//-2:CAN'T LINK, -1:CAN LINK,BUT LINK NOTHING, OTHER:LINK SOME POINT WITH THE M_DWLINKID ID.
		DWORD m_dwNodeID;
		std::string m_strDescription;
		DWORD m_dwMapID;
		DWORD m_dwCityID;
		DWORD m_dwForceID;
		DWORD m_dwTongID;
		BOOL m_bNeedOpen;
		std::string m_strScriptFile;	
	};

	struct TrafficLittlePoint
	{
		PVOID pPointer;
		D3DXVECTOR3 vPosition;
	};
	struct TrafficPointSet
	{
		vector<TrafficLittlePoint*> vecTrafficPoint;
		string szName;
		DWORD m_dwID;
		DWORD m_dwCostMoney;
		DWORD m_dwVelocity;
		DWORD m_dwStep;
		DWORD m_dwFromID;
		DWORD m_dwToID;
		//D3DXVECTOR3 m_vecCurPos;
        
		TrafficPointSet();
		~TrafficPointSet();
		HRESULT AddTrafficPoint(TrafficLittlePoint** ppNewWayPoint,D3DXVECTOR3 vPos,PVOID pPointer);
		HRESULT InsertTrafficPoint(TrafficLittlePoint** ppNewWayPoint,int nIndex,D3DXVECTOR3 vPos,PVOID pPointer);
		HRESULT DeleteTrafficPoint(TrafficLittlePoint* pWayPoint);
		HRESULT DeleteAllTrafficPoint();
		HRESULT FindTrafficPointByEditorWayPoint(TrafficLittlePoint** ppWayPoint,int* nTempID,PVOID pPointer);
	};
	DWORD m_dwCurrentTrafficPointSetID;
	list<TrafficPointSet*>m_listTrafficPointSet;
	map<DWORD,TrafficPoint>m_mapTrafficPoint;

	//跨地图地相关
	map<DWORD,string>m_mapLinkAbleList;
	list<TrafficPointSet*>m_listOverMapPointSet;
	//交通工具相关
	map<DWORD,string>m_mapTrackByWhat;

	struct AIParameter
	{
		string strName;//ziduan
		int    nValue;
		int    nDefaultValue;
		string strDesc;
	};
	struct AIParameterSet
	{
		DWORD dwID;
		string szName;
		vector<AIParameter>vecParameters;

		BOOL IsEqual(AIParameterSet& set);
		int  FindParameterByName(LPSTR pName);
	};
	struct NPCInfo
	{
		string szName;
		string szNickName;
		string szScriptName;

		int nIndex;
		int    nTempleteID;
		PVOID  pPointer;

		//新添加的NPC警戒和攻击范围变量 
		float fAlertRange;			//警戒范围
		float fAttackRange;			//攻击范围

		int	nLevel;
		int		nX;
		int		nY;
		int		nZ;
		int		nKind;							// Npc类型，见CHARACTER_KIND
		DWORD	dwScriptID;
		int	    nFaceDirection;			     	// 朝向
		// ahpho 以下4个是旧ai，去掉
        //int     nAIMainState;
		//int	    nAIType;						// AI类型，见AI_TYPE
		//DWORD	dwAIParams[AI_PARAM_COUNT];		// AI参数
        //int     nDisableAutoTurn;

		BYTE	byReserved[32];

		D3DXVECTOR3 vPosition;
		D3DXQUATERNION vRotation;
        DWORD   dwRepresentID;

		WayPointSet* pWayPointSet;

		DWORD dwNpcReliveID;
		DWORD dwNpcRandomID;
		DWORD dwNpcTeamID;
		DWORD dwNpcTeamIndex;
		int nPathIndex;
		DWORD dwNpcAISet;

		AIParameterSet m_cAIParameterSet;
	};
	list<NPCInfo*>m_listNPCInfo;

	struct DoodadInfo
	{
		string szName;
		string szNickName;
		string szScriptName;

		int    nIndex;
		int    nTempleteID;
		PVOID  pPointer;

		int 	nLevel;
		int		nX;
		int		nY;
		int		nZ;
		int		nKind;							// Npc类型，见CHARACTER_KIND
		DWORD	dwScriptID;
		int  	nFaceDirection;			    	// 朝向
  //      int     nAIMainState;
		//int 	nAIType;						// AI类型，见AI_TYPE
		//DWORD	dwAIParams[AI_PARAM_COUNT];		// AI参数
		BYTE	byReserved[32];

		D3DXVECTOR3 vPosition;
		D3DXQUATERNION vRotation;

		DWORD dwDoodadReliveID;
	};
	list<DoodadInfo*>m_listDoodadInfo;

	struct RefreshPoint//刷新点
	{
		D3DXVECTOR3 vPosition;
		D3DXQUATERNION vRotation;

		int nX;
		int nY;
		int nZ;
		PVOID  pPointer;
		int nDirection;
	};

	struct RefreshPointlist
	{
		list<RefreshPoint*>listRefreshPoint;

		HRESULT FindRefreshPointByPointer(RefreshPoint** ppNewPoint,PVOID pPointer);
		HRESULT NewOneRefreshPoint(RefreshPoint** ppNewPoint,PVOID pPointer,D3DXVECTOR3 vPos,D3DXQUATERNION vRot);
		HRESULT DeleteRefreshPoint(RefreshPoint* pPoint);
		HRESULT DeleteAllRefreshPoint();

		~RefreshPointlist()
		{
			DeleteAllRefreshPoint();
		}
	};
	struct NPCRefreshPointSet
	{
		RefreshPointlist listRefreshPoint;
		list<NPCInfo*>listNPCInfo;
		string szName;
		DWORD dwSetID;
	};
	struct DoodadRefreshPointSet
	{
		RefreshPointlist listRefreshPoint;
		list<DoodadInfo*>listDoodadInfo;
		string szName;
		DWORD dwSetID;
	};


	//list<NPCRefreshPointSet*>m_listNPCRefreshPointSet;
	//DWORD m_dwCurrentNPCRefreshPointSetID;
	//list<DoodadRefreshPointSet*>m_listDoodadRefreshPointSet;
	//DWORD m_dwCurrentDoodadRefreshPointSetID;

	struct AIGroup
	{
		string szName;
		string szFileName;
		DWORD dwSetID;

		vector<NPCInfo*>vecNpcInfo;
		int nKing;
		WayPointSet* pWayPointSet;
		int nArrayIndex;

		// ahpho suntao
		int npcOrderID; // npc阵型ID
		bool reviveTeammate; // 是否复活队友
		string npcOrder; // npc的阵型
		// end ahpho
	};
	list<AIGroup*>m_listAIGroup;
	DWORD m_dwCurrentAIGroupID;

	IEKG3DSceneSceneEditor* m_lpSceneSceneEditor;

	list<IEKG3DRepresentPoly*>m_listLogicalPoly;

	//////////////////////////////////////////////////////////////////////////
	struct ReviveGroup
	{
		DWORD dwGroupID;
		string szName;
		int nMinNpcCount;
		int nMaxNpcCount;
		int nRandom;

		// ahpho suntao
		vector<NPCInfo*> vecNpcInfo;
		vector<DoodadInfo*> vecDoodadInfo;
		// end ahpo
	};
	map<DWORD,ReviveGroup>m_mapNpcReviveGroup;//npc 重生组
	map<DWORD,ReviveGroup>m_mapDoodadReviveGroup;//doodad 重生组
	DWORD m_dwCurrentNpcReviveGroupID;
	DWORD m_dwCurrentDoodadReviveGroupID;

	struct RandomGroup
	{
		DWORD dwGroupID;
		string szName;
		vector<DWORD>vecNpcTempleteID;
	};
	map<DWORD,RandomGroup>m_mapRandomGroup;//npc 随机组
	DWORD m_dwCurrentRandomGroupID;

	//////////////////////////////////////////////////////////////////////////

    AIParameterSet              m_AIParameterDefine;
    map<DWORD, AIParameterSet>  m_mapNpcAITemplateParameterSet;
	map<DWORD, AIParameterSet>  m_mapNpcAISceneParameterSet;
    DWORD                       m_dwCurrentAIParameterSetID;


private:
	DWORD GetCurrentNpcReviveGroupID()
	{
		return m_dwCurrentNpcReviveGroupID++;
	}
	DWORD GetCurrentDoodadReviveGroupID()
	{
		return m_dwCurrentDoodadReviveGroupID++;
	}
	DWORD GetCurrentRandomGroupID()
	{
		return m_dwCurrentRandomGroupID++;
	}
	DWORD GetCurrentAISetID()
	{
		return m_dwCurrentAIParameterSetID++;
	}
	
	HRESULT GetDefaultAIParameterDefine();
	HRESULT GetNpcAIDefaultValue();

	HRESULT SaveAISetToFile(IIniFile* pFile);
	HRESULT LoadAISetFromFile(IIniFile* pFile);

	HRESULT SaveAIParameters(IIniFile* pFile,LPSTR pSectionName,AIParameter& par,int nIndex);
	HRESULT LoadAIParameters(IIniFile* pFile,LPSTR pSectionName,AIParameter& par,int i);

	// ahpho suntao
	HRESULT SaveNpcHatredGroups(IIniFile* pFile);	
	HRESULT LoadNpcHatredGroups(IIniFile *pFile);
	HRESULT GetNpcInfoByIndex(NPCInfo** npcInfo, INT nIndex);
	HRESULT GetDoodadInfoByIndex(DoodadInfo** doodadInfo, INT nIndex);
	HRESULT SetAIParameterValue(int aiSetID, string paraName, int newValue);
	HRESULT GetWayPointSetByIndex(WayPointSet** pSet, int index);
	HRESULT SaveNpcAIParameters();
	HRESULT UpdateAIParameter(AIParameterSet &aiSet, string field, int newValue);	
	HRESULT ResetAISet(DWORD dwID, AIParameterSet &newAISet);
	// end ahpho
public:
	HRESULT UpdataOneNpcAIParameters(NPCInfo* pNPC);
	HRESULT ExchangeOldSettingToNpcAIParameter(NPCInfo* pNPC);
	HRESULT UpdataNpcAIParameters();

	DWORD   FindAIParameterSet(AIParameterSet& set);

	HRESULT GetNpcDefauleAIParameterSetByTempleteID(int nTempleteID, AIParameterSet* pRetAIParameterSet);

	HRESULT NewOneAISet(DWORD& dwID,LPTSTR pName);
	HRESULT DeleteAISet(DWORD dwID);
	HRESULT GetAISet(DWORD dwID,AIParameterSet& set);
	HRESULT SetAISet(DWORD dwID,AIParameterSet& set);
	//////////////////////////////////////////////////////////////////////////
	HRESULT GetNpcReviveGroup(DWORD dwID,LPTSTR pName,int& nMin,int& nMax);
	HRESULT SetNpcReviveGroup(DWORD dwID,LPTSTR pName,int nMin,int nMax);
	HRESULT NewOneNpcReviveGroup(DWORD& dwID,LPTSTR pName,int nMin,int nMax);
	HRESULT DeleteNpcReviveGroup(DWORD dwID);
	//////////////////////////////////////////////////////////////////////////


	HRESULT GetDoodadReviveGroup(DWORD dwID,LPTSTR pName,int& nMin,int& nMax);
	HRESULT SetDoodadReviveGroup(DWORD dwID,LPTSTR pName,int nMin,int nMax);
	HRESULT NewOneDoodadReviveGroup(DWORD& dwID,LPTSTR pName,int nMin,int nMax);
	HRESULT DeleteDoodadReviveGroup(DWORD dwID);
	//////////////////////////////////////////////////////////////////////////

	HRESULT GetRandomGroup(DWORD dwID,LPTSTR pName,vector<DWORD>*pvecNpcTempleteID);
	HRESULT SetRandomGroup(DWORD dwID,LPTSTR pName,vector<DWORD>*pvecNpcTempleteID);
	HRESULT NewOneRandomGroup(DWORD& dwID);
	HRESULT DeleteRandomGroup(DWORD dwID);

	HRESULT SaveNpcReviveGroups(IIniFile* pFile);
	HRESULT LoadNpcReviveGroups(IIniFile* pFile);
	HRESULT SaveDoodadReviveGroups(IIniFile* pFile);
	HRESULT LoadDoodadReviveGroups(IIniFile* pFile);
	HRESULT SaveNpcRandomGroups(IIniFile* pFile);
	HRESULT LoadNpcRandomGroups(IIniFile* pFile);

	HRESULT ExchangeNpcReviveGroups(ITabFile* pFile);
	HRESULT ExchangeDoodadReviveGroups(ITabFile* pFile);
	HRESULT ExchangeNpcRandomGroups(ITabFile* pFile,LPCTSTR pDir);

	HRESULT ExchangeNpcAISet(DWORD dwMapID,LPSTR pFileName);

public:
	HRESULT AddOneNewLogicalPoly(IEKG3DRepresentPoly* pScenePoly);
	HRESULT DeleteLogicalPoly(IEKG3DRepresentPoly* pScenePoly);
	HRESULT DeleteAllLogicalPoly();

	HRESULT SetSceneEditor(IEKG3DSceneSceneEditor* pEditor)
	{
		m_lpSceneSceneEditor = pEditor;
		return S_OK;
	}
	HRESULT FindNpcInfoByIndex(NPCInfo** ppNpc,int nIndex);
	HRESULT FindDoodadInfoByIndex(DoodadInfo** ppDoodad,int nIndex);

	HRESULT SaveLogicalPolyIniFile(IIniFile* pFile,LPCTSTR pSectionName,IEKG3DRepresentPoly* pPoly);
	HRESULT LoadLogicalPolyIniFile(IIniFile* pFile,LPCTSTR pSectionName,BOOL bAddToScene);

	HRESULT SaveAIGroupToIniFile(IIniFile* pFile,LPCTSTR pSectionName,AIGroup* pAI);
	HRESULT SaveDoodadRefreshPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,DoodadRefreshPointSet* pRefreshSet);
	HRESULT SaveNPCRefreshPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,NPCRefreshPointSet* pRefreshSet);
	HRESULT SaveWayPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,WayPointSet* pWayPointSet);	
	HRESULT SaveTrafficPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);	
	HRESULT SaveTrafficPointToIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPoint* pTrafficPoint);
	HRESULT SaveOverMapTrafficPointToIniFile(IIniFile* pFile);
	HRESULT SaveDoodadToIniFile(IIniFile* pFile,LPCTSTR pSectionName,DoodadInfo* pDoodad);
	HRESULT SaveNPCToIniFile(IIniFile* pFile,LPCTSTR pSectionName,NPCInfo* pNpc);
	HRESULT SaveToIniFile(LPCTSTR pFileName);

	HRESULT LoadAIGroupFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,AIGroup* pAI);
	HRESULT LoadDoodadRefreshPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,DoodadRefreshPointSet* pRefreshSet);
	HRESULT LoadNPCRefreshPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,NPCRefreshPointSet* pRefreshSet);
	HRESULT LoadWayPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,WayPointSet* pWayPointSet);
	HRESULT LoadTrafficPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);
	HRESULT LoadTrafficPointFromIniFile(IIniFile* pFile,LPCTSTR pSectionName);
	HRESULT LoadOverMapTrafficPointToIniFile(IIniFile* pFile);
	HRESULT LoadDoodadFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,DoodadInfo* pDoodad);
	HRESULT LoadNPCFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,NPCInfo* pNpc);
	HRESULT LoadFromIniFile(LPCTSTR pFileName,FUNC_LOADPROGRESS pProgress,BOOL bAddToScene);

	HRESULT FitDoodadGroup(DoodadRefreshPointSet* pSet);
	HRESULT FitNpcGroup(NPCRefreshPointSet* pSet);

	int SceneToLogicalAngle(D3DXQUATERNION vRot);
	void SceneToLogical(int& X,int& Y,int& Z,D3DXVECTOR3 vPos,float fGroundHeight);

	HRESULT UpdateNPC(NPCInfo* pNpc);
	HRESULT UpdateDoodad(DoodadInfo* pDoodad);
	HRESULT UpdateWayPoint(WayPoint* pWayPoint);
	HRESULT UpdateRefreshPoint(RefreshPoint* pPoint);

	HRESULT AddOneAIGroup(AIGroup** ppNewSet,LPCTSTR pName);
	HRESULT DeleteAIGroup(AIGroup* pSet);
	HRESULT DeleteAllAIGroup();
	HRESULT GetAIGroupByID(AIGroup** ppSet,DWORD dwID);

	HRESULT RemoveNPCFromAIGroup(NPCInfo* pNPC);
	//HRESULT RemoveNPCFromNpcRefreshSet(NPCInfo* pNPC);
	//HRESULT RemoveDoodadFromDoodadRefreshSet(DoodadInfo* pDoodad);

	//HRESULT AddNPCOneRefreshPointSet(NPCRefreshPointSet** ppNewSet,LPCTSTR pName);
	//HRESULT DeleteNPCRefreshPointSet(NPCRefreshPointSet* pSet);
	//HRESULT DeleteAllNPCRefreshPointSet();
	//HRESULT GetNPCRefreshPointSetByID(NPCRefreshPointSet** ppSet,DWORD dwID);

	//HRESULT AddDoodadOneRefreshPointSet(DoodadRefreshPointSet** ppNewSet,LPCTSTR pName);
	//HRESULT DeleteDoodadRefreshPointSet(DoodadRefreshPointSet* pSet);
	//HRESULT DeleteAllDoodadRefreshPointSet();
	//HRESULT GetDoodadRefreshPointSetByID(DoodadRefreshPointSet** ppSet,DWORD dwID);

	virtual void SinkFunc(INT nSinkID){}
	virtual void SinkFunc2(INT nSinkID,INT nsubSinkID,DWORD dwValue,PVOID pPoint);

	HRESULT AddOneWayPointSet(WayPointSet** ppNewSet,LPCTSTR pName);
	HRESULT DeleteWayPointSet(WayPointSet* pSet);
	HRESULT DeleteAllWayPointSet();
	HRESULT GetWayPointSetByID(WayPointSet** ppSet,DWORD dwID);

	HRESULT FindNPCInfoByEditorNPC(NPCInfo** ppNPC,PVOID pPointer);

	HRESULT AddOneNPC(
        NPCInfo** ppNewNPC,
        LPCTSTR pName,
        D3DXVECTOR3& vPos,
        D3DXQUATERNION& vRot,
		int nDirection,
        int nTempleteID,
        PVOID pPointer,
		int nLevel,
        int nKind,
        //int nAIType, 
        DWORD dwRepresentID,
        float fAlertRange,
        float fAttackRange,
        //int nDiableAutoTurn,
		DWORD dwReliveID,DWORD dwRandomID,DWORD dwAISetID
        );

	HRESULT DeleteNPC(NPCInfo* pNewNPC);
	HRESULT DeleteAllNPC();

	HRESULT AddOneDoodad(DoodadInfo** ppNewDoodad,LPCTSTR pName,D3DXVECTOR3& vPos,D3DXQUATERNION& vRot,
		int nDirection,int nTempleteID,PVOID pPointer,DWORD dwReliveID);
	HRESULT DeleteDoodad(DoodadInfo* pNewDoodad);
	HRESULT DeleteAllDoodad();

	HRESULT FindDoodadInfoByEditorDoodad(DoodadInfo** ppDoodad,PVOID pPointer);


	HRESULT AddOneTrafficPointSet(TrafficPointSet** ppNewSet,DWORD dwSetID,LPCTSTR pName);
	HRESULT DeleteTrafficPointSet(TrafficPointSet* pSet);
	HRESULT DeleteAllTrafficPointSet();
	//HRESULT UpdateTrafficPoint(TrafficPoint* pTrafficPoint);
	HRESULT GetTrafficPointSetByID(TrafficPointSet** ppSet,DWORD dwID);
	HRESULT GetTrafficPointByEditorPoint(TrafficLittlePoint** ppTrafficPoint,PVOID pPointer);
	HRESULT GetBigTrafficPointByEditorPoint(TrafficPoint** ppTrafficPoint,PVOID pPointer);
	//HRESULT SaveTrafficPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);
	//HRESULT LoadTrafficPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);


	KSceneEditorDocLogical(void);
	virtual ~KSceneEditorDocLogical(void);

	HRESULT ResortNpcAISet();
};