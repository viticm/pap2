#pragma once
#include "KG3DTemplates.h"
#include "IEKG3DTerrain.h"
#define Terrain_Info_Convermap     1
#define Terrain_Info_DetailMtlMgr  2
#define Terrain_Info_GrassSet      3
#define Terrain_Info_StoneSet      4
#define Terrain_Info_FrutexSet     5
#define Terrain_Info_RoadMgr       6
#define Terrain_Info_RiverMgr      7

//#define Terrain_Block_Convermap    1
#define Terrain_Block_GroundHeight 2
#define Terrain_Block_Water        3
#define Terrain_Block_DetailMtl    4
#define Terrain_Block_Grass        5
#define Terrain_Block_Stone        6
#define Terrain_Block_Frutex       7
#define Terrain_Block_Road         8
#define Terrain_Block_River        9

#define MAP_FILE_VERSION_00        0
#define MAP_FILE_VERSION           1
#define MAP_FILE_VERSION_PACKREGION 2


#define INVALID_OBJSHELL_HANDLE 0xffffffff

class KG3DRepresentObject;

struct KG3DEntityTransformation_v0
{
	D3DXVECTOR3    m_Scaling;
	D3DXQUATERNION m_Rotation;
	D3DXVECTOR3    m_Translation;
	D3DXVECTOR3    m_vBox_A;//包围盒
	D3DXVECTOR3    m_vBox_B;
};

struct KG3DEntityTransformation
{
    /* the scaling rotation translation is in local space */
    D3DXVECTOR3    m_Scaling;
    D3DXQUATERNION m_Rotation;
    D3DXVECTOR3    m_Translation;

    /* world space position */
    D3DXVECTOR3    m_vWorldPos;

    /* world space bbox */
    D3DXVECTOR3    m_vBox_A;//包围盒
    D3DXVECTOR3    m_vBox_B;
};

struct KG3DSceneDataClip
{
	DWORD m_dwType;
	DWORD m_dwLength;
	PVOID m_lpBuffer;

	int   m_nRefrence;//引用计数

	KG3DSceneDataClip()
	{
		m_dwType = 0;
		m_dwLength = 0;
		m_lpBuffer = NULL;
		m_nRefrence = 0;
	}

	~KG3DSceneDataClip()
	{
		UnInit();
	}

	HRESULT UnInit()
	{
		SAFE_DELETE_ARRAY(m_lpBuffer);
		m_dwType = 0;
		m_dwLength = 0;
		m_nRefrence = 0;
		return S_OK;
	}

	HRESULT SetData(DWORD dwType,DWORD dwLength,PVOID lpBuffer);
	HRESULT GetData(PVOID lpBuffer);
	BOOL IsEqual(DWORD dwDataType,DWORD dwLength,PVOID pData)
	{
		if(m_dwType != dwDataType)
			return FALSE;
		if(m_dwLength != dwLength)
			return FALSE;

		if(m_lpBuffer && pData)	
		{
			if (memcmp(m_lpBuffer,pData,m_dwLength) != 0)
				return FALSE;
			else
				return TRUE;
		}
		else
			return FALSE;
		return FALSE;
	}

	HRESULT SaveDataToFile(LPCTSTR pFileName);
	HRESULT LoadDataFromFile(LPCTSTR pFileName);

	HRESULT SaveDataToFile(FILE* pFile);
	HRESULT LoadDataFromFile(IFile* pFile);

};

struct KG3DEntityDataEx;
struct KG3DSceneBlockData;

struct KG3DSceneBlockDataChildData
{
	KG3DSceneBlockData* m_zChildBlock[64];
	DWORD               m_zFilePosition[64];
	KG3DSceneBlockDataChildData()
	{
		ZeroMemory(m_zChildBlock,sizeof(m_zChildBlock));
		ZeroMemory(m_zFilePosition,sizeof(m_zFilePosition));
	}
};

struct KG3DSceneBlockData
{
	int m_nLevel;
	int m_nIndexX;
	int m_nIndexZ;

	int m_nLastUseTime;
	DWORD m_dwDataSize;

	BOOL m_bDataInMemory;//数据是否在内存中 0表示已经交换到了硬盘上

	BOOL m_bTerrainBlock;//是否创建有地形BLOCK
	BOOL m_bModified;//是否被修改过
	BOOL m_bDisableDynamicUnload;//禁止动态卸载，表示该数据正在被使用
	BOOL m_bSaveChildData; //是否要保存ChildData,也是指Region的数据.这个标志只是当m_nLevel == BLOCK_TYPE_SECTION 时有用
	KG3DSceneBlockData* m_pParentBlockData;
	KG3DSceneBlockDataChildData* m_lpChildBlockData;

	map<DWORD,KG3DSceneDataClip*>m_mapData;
	list<KG3DSceneDataClip*>m_listDataHistroy;

	HRESULT Init(TKG3DResourcePool<KG3DSceneDataClip>* pPool);
	HRESULT UnInit();
	HRESULT FreeDataClips();
	HRESULT FreeDataHistroyClips();

	KG3DSceneBlockData();
	~KG3DSceneBlockData();


	KG3DSceneDataClip* FindDataByType(DWORD dwType);
	HRESULT ModifyData(DWORD dwType,DWORD dwLength,PVOID lpBuffer,BOOL bRecord);

	TKG3DResourcePool<KG3DSceneDataClip>* m_lpSceneDataClipPool;

	HRESULT SaveDataToFile(LPCTSTR pFileName);
	HRESULT SaveDataToFile(FILE* pFile);

	HRESULT LoadDataFromFile(TKG3DResourcePool<KG3DSceneDataClip>* pPool,LPCTSTR pFileName,BOOL bPackRegion);
	HRESULT LoadDataFromFile(IFile* pFile);


	HRESULT SaveChildDataToFile(LPCTSTR pFileName);
	HRESULT SaveChildDataToFile(FILE* pFile);
	HRESULT LoadDataFromPackFile(IFile* pFile);

	//////////////////////////////////////////////////////////////////////////
	set<KG3DEntityDataEx*>m_setEntity;
	HRESULT InputEntity(KG3DEntityDataEx* pEntity);
	HRESULT RemoveEntity(KG3DEntityDataEx* pEntity);

	DWORD GetMemoryUse();
};

class KG3DSceneDataEventConnection
{
public:
	virtual HRESULT OnEntityCreate(UINT uHandle, int bForceLoad) = 0;
	virtual HRESULT OnEntityDelete(UINT uHandle) = 0;

	virtual HRESULT OnEntityTransformModified(UINT uHandle, BOOL bCheckInput2Pvs) = 0;
	virtual HRESULT OnEntityInformationModified(UINT uHandle) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT OnSceneBlockCreate(int nLevel,POINT Index) = 0;
	virtual HRESULT OnSceneBlockDelete(int nLevel,POINT Index) = 0;
	virtual HRESULT OnTerrainBlockCreate(int nLevel,POINT Index) = 0;
	virtual HRESULT OnTerrainBlockDelete(int nLevel,POINT Index) = 0;
	virtual HRESULT OnTerrainBlockModified(int nLevel,POINT index,DWORD dwDataType) = 0;

	virtual HRESULT OnTerrainInformationModified(UINT uInfoHandle,DWORD dwDataType) = 0;
};

class KG3DSceneDataInterface
{
public:
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption) = 0;
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption) = 0;

	virtual HRESULT Init(LPCTSTR pFileName,BOOL nNewScene) = 0;
	virtual HRESULT UnInit() = 0;

	virtual HRESULT Request_CreateNewEntityInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData) = 0;
	virtual HRESULT Request_CreateNewTerrainInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData) = 0;
	virtual HRESULT Request_ModifyTerrainInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData) =0;
	virtual HRESULT Request_ReplaceEntity(vector<UINT>& uEntityHandles,DWORD dwDataType,DWORD dwLength,PVOID pData, bool all, OUT vector<UINT>& allNeedReplaceHandles) = 0; //注意:uEntityHandles指的是entity的id而不是entityinfo的id

	virtual HRESULT Request_CreateNewEntity(UINT* pHandle,UINT uEntityInfo, int bForceLoad) = 0;
	virtual HRESULT Request_DeleteEntity(UINT uHandle) = 0;

    virtual HRESULT UpdateObjTransToData(KG3DRepresentObject* obj) = 0;

	virtual HRESULT Request_ModifyEntityTransform(UINT uHandle,KG3DEntityTransformation& vTransformation ) = 0;
	//virtual HRESULT Request_ModifyEntityInformation(KG3DRepresentObject* pObj) = 0;
	virtual HRESULT Request_ModifyEntityInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData) = 0;

	virtual HRESULT GetEntityInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen) = 0;
	virtual HRESULT GetEntityTransformation(UINT uHandle,KG3DEntityTransformation& vTransformation) = 0;
	virtual HRESULT GetEntityIntersectRect(set<UINT>*psetEntity,const D3DXVECTOR2& vA,const D3DXVECTOR2& vC) = 0;
	virtual HRESULT GetTerrainInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData) = 0;
	virtual HRESULT GetEntityOnSceneBlock(set<UINT>*psetEntity,int nLevel,POINT Index) = 0;

	virtual HRESULT AddEventConnection(KG3DSceneDataEventConnection* pConnection) = 0;
	virtual HRESULT RemoveEventConnection(KG3DSceneDataEventConnection* pConnection) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT Request_CreateSceneBlock(int nLevel,POINT Index) = 0;//创建场景区域
	virtual HRESULT Request_DeleteSceneBlock(int nLevel,POINT Index) = 0;

	virtual HRESULT Request_CreateTerrainBlock(int nLevel,POINT Index) = 0;//创建地表
	virtual HRESULT Request_DeleteTerrainBlock(int nLevel,POINT Index) = 0;

	virtual HRESULT Request_ModifyTerrainBlock(int nLevel,POINT Index,DWORD dwDataType,DWORD dwLength,PVOID pData) = 0;//请求修改地形数据

	virtual BOOL    GetSceneBlock(int nLevel,POINT Index) = 0;//查询地表SECTION是否存在
	virtual BOOL    GetTerrainBlock(int nLevel,POINT Index) = 0;//查询地表SECTION是否存在
	virtual HRESULT GetTerrainBlockData(int nLevel,POINT Index,DWORD dwDataType,PVOID pData) = 0;//获取地形数据，调用者自己负责该数据的创建和删除，该函数内部完成数据复制
	virtual HRESULT GetAllTerrainBlockDataType(int nLevel,POINT Index,vector<DWORD>*vecDataType) = 0;//获取地形块上的所有数据

	virtual HRESULT GetSceneRect(D3DXVECTOR2& LowerLeft,D3DXVECTOR2& UpperRight) = 0;//获取整个场景的长宽

    virtual DWORD GetVersion() = 0;
    virtual HRESULT SetEntityParent(UINT uHandle, UINT uParent) = 0;
    virtual UINT GetEntityParent(UINT uHandle) = 0;

	virtual DWORD CheckMemoryUseMultiThread(DWORD dwLastUseTime,BOOL* pbStop) = 0;
#ifdef KG3D_AUTO_TEST //测试开发那边使用的函数
	virtual HRESULT GetEntityInfoByHandle(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen) = 0;//传EntityInfomation的Handle进去
	virtual HRESULT GetAllEntityInformationHandle(vector<UINT>*vecHandle) = 0;//获取场景里所有EntityInformation
	virtual HRESULT GetAllTerrianInformationHandle(vector<UINT>*vecHandle) = 0;//获取场景里所有TerrainInformation
	virtual HRESULT GetEntityData(map<UINT,KGEntityData>* mapEntityData) = 0;//获取场景里所有Entity的数据
	virtual HRESULT GetAllSceneBlock(vector<SceneBlockIndex>* vecSceneBlockIndex) = 0;
	virtual HRESULT GetSceneBlockData(int nLevel,POINT Index,DWORD& dwLenght,PVOID pData) = 0;//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制
#endif
	KG3DSceneDataInterface()
	{
		;
	}
	virtual ~KG3DSceneDataInterface()
	{
		;
	}
};


//struct KG3DEntityContainer;

struct KG3DEntityData
{
	UINT m_uHandle;
    UINT m_uParent;
	KG3DEntityTransformation m_Transformation;
	UINT m_uEntityInformationHandle;

	KG3DEntityData()
	{
		m_uHandle =0;
        m_uParent = INVALID_OBJSHELL_HANDLE;
		m_uEntityInformationHandle = 0;
		m_Transformation.m_Translation = D3DXVECTOR3(0,0,0);
		m_Transformation.m_vBox_A = D3DXVECTOR3(0,0,0);
		m_Transformation.m_vBox_B = D3DXVECTOR3(0,0,0);
		m_Transformation.m_Rotation = D3DXQUATERNION(0,0,0,0);
		m_Transformation.m_Scaling = D3DXVECTOR3(1,1,1);
	}
	virtual ~KG3DEntityData()
	{
		;
	}
};

struct KG3DEntityDataEx : public KG3DEntityData
{
	set<KG3DSceneBlockData*>m_setContainer;
	HRESULT OnInputToContainer(KG3DSceneBlockData* pContainer);
	HRESULT RemoveFromContainers();

	//////////////////////////////////////////////////////////////////////////
	//记录该物件的历史信息
	struct HistroyNode
	{
		KG3DEntityTransformation vTrans;
		UINT uInfo;
	};
	list<HistroyNode>m_listHistory;//几何信息的历史记录
	HRESULT RecordHistoryNode();

	HRESULT UnInit()
	{
		RemoveFromContainers();

		m_listHistory.clear();

		m_uHandle =0;
        m_uParent = INVALID_OBJSHELL_HANDLE;

		m_uEntityInformationHandle = 0;
		m_Transformation.m_Translation = D3DXVECTOR3(0,0,0);
		m_Transformation.m_vBox_A = D3DXVECTOR3(0,0,0);
		m_Transformation.m_vBox_B = D3DXVECTOR3(0,0,0);
		m_Transformation.m_Rotation = D3DXQUATERNION(0,0,0,0);
		m_Transformation.m_Scaling = D3DXVECTOR3(1,1,1);

		return S_OK;
	}
};

//struct KG3DEntityContainer
//{
//	set<KG3DEntityDataEx*>m_setEntity;
//	HRESULT InputEntity(KG3DEntityDataEx* pEntity);
//	HRESULT RemoveEntity(KG3DEntityDataEx* pEntity);
//};


class KG3DSceneDataInterfaceCore ://数据层核心
	public KG3DSceneDataInterface
{
private:
	struct MapFileHeader
	{
		DWORD dwMask;
		DWORD dwVersion;
		UINT  uCurrentEntityID;
		UINT  uCurrentEntityInformationID;
		UINT  uCurrentTerrainInformationID;
		DWORD dwNumEntity;
		DWORD dwNumEntityInformation;
		DWORD dwNumTerrainInformation;
		DWORD dwNumSceneBlock[4];
		DWORD dwExtend[128];
	};

	string m_szDirectoryRead;
	string m_szDirectoryWrite;

	AABBOX m_vBBox;
private:
	/*
	这些数据是为了进行多用户和多线程编辑隔离用的
	还是不要public吧，到时候多线程的时候，一旦有人的代码里面直接取这些东西的话太凶险了。而且有些有对应关系
	public很容易破坏这种关系。如果要遍历的话，用GetFirst，GetNext，把一个静态的iterator的指针传出去当Handle就好了
	*/
	//map<POINT,KG3DEntityContainer*>m_mapEntityContainer;
	BOOL  m_bPackData;
    DWORD m_dwVersion;

	UINT m_uCurrentEntityID;//当前的ENTITY编号，只加不减
	UINT m_uCurrentEntityInformationID;
	UINT m_uCurrentTerrainInformationID;
	
	map<UINT,KG3DEntityDataEx*>   m_mapEntity;//场景物件
	map<UINT,KG3DSceneDataClip*>  m_mapEntityInformation;//场景物件信息，用于创建一个场景物件的数据
	map<UINT,KG3DSceneDataClip*>  m_mapTerrainInformation;//地表信息
	map<POINT,KG3DSceneBlockData*>m_mapSceneBlockData[4];//场景数据模块
    KMutex                        m_mapSceneBlockDataLock;
	KG3DSceneBlockData m_ErrorSceneBlock;

	
	list<KG3DEntityDataEx*>m_listEntityDeleted;

	KG3DSceneDataEventConnection* m_lpEventConnection;
	
	list<KG3DSceneBlockData*>m_listTerrainSectionDeleted;

	TKG3DResourcePool<KG3DSceneBlockData>m_cSceneBlockDataPool;
	TKG3DResourcePool<KG3DEntityDataEx>m_cEntityDataExPool;
	TKG3DResourcePool<KG3DSceneDataClip>m_cDataClipPool;

	DWORD m_dwMemoryUse;

	struct InfoFileData
	{
		UINT uHandle;
		DWORD dwFilePos;
		InfoFileData()
		{
			uHandle = 0;
			dwFilePos = 0;
		}
	};
protected:
/////////////////////////////////////////////////////////////////////////////////
	HRESULT SaveEntityInfoToFile(LPCTSTR pFileName);
	HRESULT LoadEntityInfoFromFile(LPCTSTR pFileName);

	HRESULT SaveTerrainInfoToFile(LPCTSTR pFileName);
	HRESULT LoadTerrainInfoFromFile(LPCTSTR pFileName);
////////////////////////////////////////////////////////////////////////////////
	HRESULT TryInputEntityToLevel(KG3DEntityDataEx* pEntity,D3DXVECTOR2 A,D3DXVECTOR2 C,int nLevel);

	HRESULT CreateMapFileHeader(MapFileHeader* pHeader);
	HRESULT SaveEntityToFile(FILE* pFile);
	HRESULT SaveEntityInfomationToFile(FILE* pFile);
	HRESULT SaveTerrainInfomationToFile(FILE* pFile);
	HRESULT SaveSceneBlockToFile(FILE* pFile,BOOL& bConversion);
	HRESULT LoadEntityFromFile(IFile* pFile);
	HRESULT LoadEntityInfomationFromFile(IFile* pFile);
	HRESULT LoadTerrainInfomationFromFile(IFile* pFile);
	HRESULT LoadSceneBlockFromFile(IFile* pFile);

	HRESULT LoadRegionBlock(KG3DSceneBlockDataChildData* pChildData,LPCTSTR pFileName,DWORD& dwMemoryUse);

	HRESULT PrepareSceneBlock(KG3DSceneBlockData* pBlock,int nLevel,POINT Index);//为SceneBlock准备数据，如果数据在硬盘上，就加载
	HRESULT UnPrepareSceneBlock(KG3DSceneBlockData* pBlock,int nLevel,POINT Index);//卸载数据，如果数据修改过，就存盘

	HRESULT InitDirectory(LPCTSTR pFileName,BOOL bRead,BOOL bWrite);

	HRESULT GetEntityInformationFileName(LPSTR pFileName,UINT uHandle,BOOL bRead);
	HRESULT GetTerrainInformationFileName(LPSTR pFileName,UINT uHandle,BOOL bRead);
	HRESULT GetSceneBlockFileName(LPSTR pFileName,KG3DSceneBlockData* pBlock,BOOL bRead);
	HRESULT GetSceneBlockChildFileName(LPSTR pFileName,KG3DSceneBlockData* pBlock,BOOL bRead);

	KG3DSceneBlockData* FindSceneBlock(int nLevel,POINT Index);

	UINT GetCurrentEntityID();
	UINT GetCurrentEntityInforamtionID();
	UINT GetCurrentTerrainInforamtionID();

	HRESULT GetRectIntersectContainers(vector<POINT>*pvecContainer,const D3DXVECTOR2& A,const D3DXVECTOR2& C,int nLevel);
	HRESULT InputEntity(KG3DEntityDataEx* pEntity);
	HRESULT FindEntity(KG3DEntityDataEx** ppEntity,UINT uHandle);
	
	KG3DSceneDataClip* FindEntityInforamtion(UINT uHandle);
	KG3DSceneDataClip* FindTerrainInforamtion(UINT uHandle);
   

	HRESULT AddEntityInformation(KG3DSceneDataClip** ppInfo,UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData);
	HRESULT AddTerrainInformation(KG3DSceneDataClip** ppInfo,UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData);

    virtual DWORD GetVersion() { return m_dwVersion; }
    virtual HRESULT SetEntityParent(UINT uHandle, UINT uParent);
    virtual UINT GetEntityParent(UINT uHandle);

	virtual DWORD CheckMemoryUseMultiThread(DWORD dwLastUseTime,BOOL* pbStop);//检查内存占用
public:

	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption);

    virtual HRESULT UpdateObjTransToData(KG3DRepresentObject* obj);

	virtual HRESULT Request_UnDeleteEntity();
	virtual HRESULT Request_UndoModifyEntity(UINT uHandle);

	virtual HRESULT Init(LPCTSTR pFileName,BOOL nNewScene);
	virtual HRESULT UnInit();

	virtual HRESULT Request_CreateNewEntityInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData);
	virtual HRESULT Request_CreateNewTerrainInfo(UINT& uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData);
	virtual HRESULT Request_ModifyTerrainInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData); //add by huangjinshou 2008-4-18

	virtual HRESULT Request_ReplaceEntity(vector<UINT>& uEntityHandles,DWORD dwDataType,DWORD dwLength,PVOID pData, bool all, OUT vector<UINT>& allNeedReplaceHandles); // add by Likan 2009-10-21

	virtual HRESULT Request_CreateNewEntity(UINT* pHandle,UINT uEntityInfo, int bForceLoad);
	virtual HRESULT Request_DeleteEntity(UINT uHandle);

	virtual HRESULT Request_ModifyEntityTransform(UINT uHandle,KG3DEntityTransformation& vTransformation );
 //   virtual HRESULT Request_ModifyEntityInformation(KG3DRepresentObject* pObj);
	virtual HRESULT Request_ModifyEntityInformation(UINT uHandle,DWORD dwDataType,DWORD dwLength,PVOID pData);

	virtual HRESULT GetEntityInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen);
	virtual HRESULT GetTerrainInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData);
	virtual HRESULT GetEntityTransformation(UINT uHandle,KG3DEntityTransformation& vTransformation);
	virtual HRESULT GetEntityIntersectRect(set<UINT>*psetEntity,const D3DXVECTOR2& vA,const D3DXVECTOR2& vC);
	virtual HRESULT GetEntityOnSceneBlock(set<UINT>*psetEntity,int nLevel,POINT Index);

	virtual HRESULT AddEventConnection(KG3DSceneDataEventConnection* pConnection);
	virtual HRESULT RemoveEventConnection(KG3DSceneDataEventConnection* pConnection);
	
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT Request_CreateSceneBlock(int nLevel,POINT Index);//创建场景区域
	virtual HRESULT Request_DeleteSceneBlock(int nLevel,POINT Index);

	virtual HRESULT Request_CreateTerrainBlock(int nLevel,POINT Index);//创建地表
	virtual HRESULT Request_DeleteTerrainBlock(int nLevel,POINT Index);

	virtual HRESULT Request_ModifyTerrainBlock(int nLevel,POINT Index,DWORD dwDataType,DWORD dwLength,PVOID pData);//请求修改地形数据

	virtual BOOL    GetSceneBlock(int nLevel,POINT Index);//查询地表SECTION是否存在
	virtual BOOL    GetTerrainBlock(int nLevel,POINT Index);//查询地表SECTION是否存在
	virtual HRESULT GetTerrainBlockData(int nLevel,POINT Index,DWORD dwDataType,PVOID pData);//获取地形数据，调用者自己负责该数据的创建和删除，该函数内部完成数据复制
	virtual HRESULT GetAllTerrainBlockDataType(int nLevel,POINT Index,vector<DWORD>*vecDataType);//获取地形块上的所有数据

	virtual HRESULT GetSceneRect(D3DXVECTOR2& LowerLeft,D3DXVECTOR2& UpperRight);//获取整个场景的长宽

#ifdef KG3D_AUTO_TEST 
	//测试开发那边使用的函数
	virtual HRESULT GetEntityInfoByHandle(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen);//传EntityInfomation的Handle进去
	virtual HRESULT GetAllEntityInformationHandle(vector<UINT>*vecHandle);//获取场景里所有EntityInformation
	virtual HRESULT GetAllTerrianInformationHandle(vector<UINT>*vecHandle);//获取场景里所有TerrainInformation
	virtual HRESULT GetEntityData(map<UINT,KGEntityData>* mapEntityData);
	virtual HRESULT GetAllSceneBlock(vector<SceneBlockIndex>* vecSceneBlockIndex);
	virtual HRESULT GetSceneBlockData(int nLevel,POINT Index,DWORD& dwLenght,PVOID pData);//获取地形数据，调用者自己负责该数据的创建和删除，该函数背部完成数据复制

#endif
	KG3DSceneDataInterfaceCore();
	virtual ~KG3DSceneDataInterfaceCore();
};

class KG3DSceneDataInterfaceSingle ://单人编辑
	public KG3DSceneDataInterface
{
public:

	virtual HRESULT Request_CreateNewEntity(UINT* pHandle);
	virtual HRESULT Request_DeleteEntity(UINT uHandle);

	virtual HRESULT Request_ModifyEntityTransform(UINT uHandle,KG3DEntityTransformation& vTransformation );
	virtual HRESULT Request_ModifyEntityInformation(KG3DRepresentObject* pObj);

	virtual HRESULT GetEntityInformation(UINT uHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen);
	virtual HRESULT GetEntityTransformation(UINT uHandle,KG3DEntityTransformation& vTransformation);
	virtual HRESULT GetEntityIntersectRect(set<UINT>*psetEntity,D3DXVECTOR2& vA,D3DXVECTOR2& vC);

	virtual HRESULT AddEventConnection(KG3DSceneDataEventConnection* pConnection);
	virtual HRESULT RemoveEventConnection(KG3DSceneDataEventConnection* pConnection);

	KG3DSceneDataInterfaceSingle();
	virtual ~KG3DSceneDataInterfaceSingle();
};

class KG3DSceneDataInterfaceMultiple ://多人在线编辑
	public KG3DSceneDataInterface
{
};


extern void SceneBlockFindLength(int nLevel,float& BlockLength,float& CellLength);
extern D3DXVECTOR4 GetSceneBlockRect(int nLevel,int nX,int nZ);
extern BOOL IsIntersectBlockRect(D3DXVECTOR4& blockRect,D3DXVECTOR2& A,D3DXVECTOR2& C);
