#pragma once

class KNpc;
class KSceneEditorDoc;

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
struct KNpcTemplate;
struct KDoodadTemplate;

class KSceneEditorDocLogical :
	//public IEKG3DSimpleSink,
	public CKG3DConnectionPoint<IEKG3DSimpleSink>
{
public:
	int m_nSceneID;
	float m_fLogicalXStart;
	float m_fLogicalZStart;

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
		int nPatrolPathOrderID;
		int nPatrolPathWalkSpeed;
		int nIndex;

		WayPointSet()
		{
			dwSetID = 0;
			nIndex = 0;
			nPatrolPathWalkSpeed = 6;
			nPatrolPathOrderID = 1;
		}

		~WayPointSet();
		HRESULT AddWayPoint(WayPoint** ppNewWayPoint,D3DXVECTOR3 vPos,D3DXQUATERNION vRot,PVOID pPointer);
		HRESULT InsertWayPoint(WayPoint** ppNewWayPoint,int nIndex,D3DXVECTOR3 vPos);
		HRESULT DeleteWayPoint(WayPoint* pWayPoint);
		HRESULT DeleteAllWayPoint();
		HRESULT FindWayPointByEditorWayPoint(WayPoint** ppWayPoint,PVOID pPointer);

		HRESULT Clone(WayPointSet* pSrcSet,IEKG3DSceneSceneEditor* pEditor);

	};
	DWORD m_dwCurrentWayPointSetID;
	map<DWORD,WayPointSet*>m_mapWayPointSet;

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
		D3DXVECTOR3 vPostion;
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
		string strValue;
		string strDefaultValue;
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
		//int nPathIndex;
		DWORD dwNpcAISet;

		AIParameterSet m_cAIParameterSet;

		int nProgressID;						// 副本进度ID
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
        //int     nAIMainState;
		//int 	nAIType;						// AI类型，见AI_TYPE
		//DWORD	dwAIParams[AI_PARAM_COUNT];		// AI参数
		BYTE	byReserved[32];

		D3DXVECTOR3 vPosition;
		D3DXQUATERNION vRotation;

		DWORD	dwDoodadReliveID;

		// 动态障碍
		bool    bUseDefaultObstacle;			// 如果为真，那么保存时，如果doodadTemplate表的DynamicObstacle为1，则按顺序生成groupid，如果DynamicObstacle为0，则groupid为0
		bool    bGenerateObstacle;				// bUseDefaultObstacle为假时 起作用

		int		nProgressID;					// 副本进度ID
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

public:
	BOOL m_bUseNpcWayPointSetPointer;

	HRESULT SetAIParam(KSceneEditorDocLogical::AIParameterSet& rAISet, char szParamName[], int nValue);
	HRESULT GetAIParam(KSceneEditorDocLogical::AIParameterSet& rAISet, char szParamName[], int& nValue);

	HRESULT SortWayPointSet();

	HRESULT UpdataOneNpcAIParameters(NPCInfo* pNPC,BOOL bWayPointSetToAI = FALSE,BOOL bAIParameterTOWayPointSet = FALSE);
	HRESULT ExchangeOldSettingToNpcAIParameter(NPCInfo* pNPC);
	HRESULT UpdataNpcAIParameters(BOOL bWayPointSetToAI = FALSE,BOOL bAIParameterTOWayPointSet = FALSE);

	DWORD   FindAIParameterSet(int nNpcTempleteID,AIParameterSet& set);
	WayPointSet* FindWayPointSetByIndex(int nIndex);

	HRESULT GetNpcDefauleAIParameterSetByTempleteID(int nTempleteID, AIParameterSet* pRetAIParameterSet);

	HRESULT NewOneAISet(DWORD& dwID,LPTSTR pName);
	HRESULT DeleteAISet(DWORD dwID);
	HRESULT GetAISet(DWORD dwID,AIParameterSet& set);
	HRESULT SetAISet(DWORD dwID,AIParameterSet& set);
	//////////////////////////////////////////////////////////////////////////
	HRESULT GetNpcReviveGroup(DWORD dwID,LPTSTR pName,int& nMin,int& nMax, int& IsRandom);
	HRESULT SetNpcReviveGroup(DWORD dwID,LPTSTR pName,int nMin,int nMax, int nIsRandom);
	HRESULT NewOneNpcReviveGroup(DWORD& dwID,LPTSTR pName,int nMin,int nMax, int nIsRandom);
	HRESULT DeleteNpcReviveGroup(DWORD dwID);
	//////////////////////////////////////////////////////////////////////////


	HRESULT GetDoodadReviveGroup(DWORD dwID,LPTSTR pName,int& nMin,int& nMax, int& nIsRandom);
	HRESULT SetDoodadReviveGroup(DWORD dwID,LPTSTR pName,int nMin,int nMax, int nIsRandom);
	HRESULT NewOneDoodadReviveGroup(DWORD& dwID,LPTSTR pName,int nMin,int nMax, int IsRandom);
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
	KNpcTemplate* GetNpcTemplate(DWORD dwID);
	KDoodadTemplate* GetDoodadTemplate(DWORD dwID);

	IEKG3DRepresentPoly* FindPolyByIndex(int nIndex);
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
	HRESULT LoadLogicalPolyIniFile(IIniFile* pFile,LPCTSTR pSectionName,BOOL bAddToScene,KSceneEditorDoc* pDoc);

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
	HRESULT LoadFromIniFile(LPCTSTR pFileName,FUNC_LOADPROGRESS pProgress,BOOL bAddToScene,KSceneEditorDoc* pDoc);

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

	HRESULT AddOneWayPointSet(WayPointSet** ppNewSet,LPCTSTR pName,BOOL bToMap);
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

	HRESULT ReplaceNPC(bool all, int nTempleteID, DWORD dwRepresentID);

	HRESULT DeleteNPC(NPCInfo* pNewNPC);
	HRESULT DeleteAllNPC();

	HRESULT AddOneDoodad(DoodadInfo** ppNewDoodad,LPCTSTR pName,D3DXVECTOR3& vPos,D3DXQUATERNION& vRot,
		int nDirection,int nTempleteID,PVOID pPointer,DWORD dwReliveID,bool bUseDefaultObstacle,bool bGenerateObstacle);
	HRESULT DeleteDoodad(DoodadInfo* pNewDoodad);
	HRESULT DeleteAllDoodad();

	HRESULT FindDoodadInfoByEditorDoodad(DoodadInfo** ppDoodad,PVOID pPointer);


	HRESULT AddOneTrafficPointSet(TrafficPointSet** ppNewSet,DWORD dwSetID,LPCTSTR pName);
	HRESULT DeleteTrafficPointSet(TrafficPointSet* pSet);
	HRESULT DeleteAllTrafficPointSet();
	//HRESULT UpdateTrafficPoint(TrafficPoint* pTrafficPoint);
	HRESULT GetTrafficPointSetByID(TrafficPointSet** ppSet,DWORD dwID);
	HRESULT GetTrafficPointByEditorPoint(TrafficLittlePoint** ppTrafficPoint,PVOID pPointer);
	//HRESULT SaveTrafficPointSetToIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);
	//HRESULT LoadTrafficPointSetFromIniFile(IIniFile* pFile,LPCTSTR pSectionName,TrafficPointSet* pTrafficPointSet);


	KSceneEditorDocLogical(void);
	virtual ~KSceneEditorDocLogical(void);

	HRESULT ResortNpcAISet();

};
