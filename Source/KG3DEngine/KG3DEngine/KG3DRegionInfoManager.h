////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRegionInfoManager.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-3-27 19:00:53
/*  Comment     : 这个类的主要用途是用于辨别当前所处Region以及取得Region的
相关信息。同时也负责处理设定Region的相关信息。
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRegionInfoManager_H_
#define _INCLUDE_KG3DRegionInfoManager_H_
#include "IEKG3DRegionInfoManager.h"
#include "KG3DTemplates.h"


////////////////////////////////////////////////////////////////////////////////

class KG3DTexture;
namespace KG_CUSTOM_HELPERS
{
	class ArrayCompressor;
};
class KG3DRegionInfoManager : public IKG3DRegionInfoManager, public IEKG3DRegionInfoManager, public KG3D_CUSTOM_HELPERS::KG3DLifeTimeDependedClassBase<1>
{
public:
	friend class KG3DRegionInfoManagerTable;
	enum
	{
		TEX_MAX_SIZE = 4096,
	};

	//<ResourceBase>
public:
	KG3DRegionInfoManager(void);
	virtual ~KG3DRegionInfoManager(void);
public:///模板要用到的
	ULONG m_ulRefCount;
	DWORD m_dwID;
	std::string m_scName;
	//</ResourceBase>
public:
	
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void **ppvObject);
	//ID,type
	virtual DWORD GetID(){return m_dwID;}
	virtual unsigned GetType(){return KG3DTYPE_REGION_INFO_MANAGER;}
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){NULL;}
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam);
	virtual HRESULT RefreshOption(unsigned uOption);
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption);//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数
	//////////////////////////////////////////////////////////////////////////
	static KG3DREGION_INFORMER_LOADFILE_PARAM GetDefaultLoadFileStruct();
	INT GetHeight();
	INT GetWidth();
	BOOL GetData(INT nX, INT nZ, INT& nRet);///贴图左下为0点进行过贴图左上到右下的转换了
	//HRESULT LoadImageWithDefaultParam(const TCHAR cszFileName[], DWORD_PTR uFileNameHash);
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT IELoadFromFile(const TCHAR cszFileName[], DWORD_PTR uFileNameHash, DWORD_PTR uOption);
	virtual HRESULT GetRegionTexState(BOOL* pbIsExist, INT* pnID, LPSTR czPathBuffer, INT nBufferSize);
	virtual HRESULT GetDiffArrayCount(INT* pnCount);
	virtual HRESULT GetDiffArray(DWORD* pBuffer, INT nBufferSize);
	virtual HRESULT GetDefaultParam(KG3DREGION_INFORMER_LOADFILE_PARAM* pParam);
	virtual HRESULT GenerateRegionTex(DWORD_PTR dwExtraParam, DWORD* pdwTexID);
	virtual HRESULT GetPath(LPSTR pPathBuffer, INT nBufferSize);
	virtual HRESULT Save(const TCHAR cszFileName[], DWORD_PTR uOption);
	virtual HRESULT GetState(KG3DREGION_INFORMER_STATE* pState);//得到一些调试变量
	virtual HRESULT IEGetState(KG3DREGION_INFORMER_STATE* pState){return GetState(pState);}//IE的版本
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetRegionInfoData(D3DXVECTOR3 vecPos, DWORD* pdwData);
public:
	HRESULT SetTexture(UINT uStage);
#if defined(DEBUG) | defined(_DEBUG)
	void DebugPrintCompresedData();
#endif
private:
	HRESULT LoadImage(const TCHAR cszFileName[], DWORD_PTR uFileNameHash, DWORD_PTR uOption);
	BOOL AnalizeTexture(const TCHAR cszFileName[], KG3DTexture* pTex, INT nTargetWidth
						, INT nTargetHeight, KG3DREGION_INFORMER_LOADFILE_PARAM* pParam);
	BOOL GenerateTex(LPCTSTR lpPath  = NULL);
private:
	KG_CUSTOM_HELPERS::ArrayCompressor* m_pArrayCompressor;
	KG3DTexture* m_pTexture;
	BOOL m_bHasSuccessfullyLoaded;
	std::tstring*	m_pstTexPath;					//用于共享一个贴图路径，生成一张预览用的贴图
	UINT			m_uHowManyPixelsInOneRegion;	//用于增大精度，本来默认是1的
	INT				m_nRegionXCount;
	INT				m_nRegionZCount;			//用于存储Region的大小，不再从graphicTool中取Scene

};
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*这个设定是为了处理将来可能出现的多Manager的情况 
*/
/************************************************************************/
class KG3DRegionDWInfoManager : public KG3DRegionInfoManager
{
public:
	static const char* GetDefaultMapName();
};
//////////////////////////////////////////////////////////////////////////
class KG3DRegionInfoManagerTable : public TKG3DResourceManagerBase<KG3DRegionInfoManager>, public IEKG3DRegionInfoManagerTable
{
private:
	enum
	{
		max_element_count = 100, 
	};
	typedef std::map<DWORD, IKG3DRegionInfoManager*> TypeExtraMap;///以免和ResourceMap搅和在一起，所以存I指针
	TypeExtraMap* m_pMapExtraRecord;
public:
	KG3DRegionInfoManagerTable();
	~KG3DRegionInfoManagerTable();
public:
	virtual HRESULT Get1NewResourcePointer(KG3DRegionInfoManager** ppT,DWORD Type,DWORD_PTR Option);
	virtual HRESULT GetTypeInfoByFileName(TypeInfo** ppTypeInfo,const char cszFileName[]);
	virtual HRESULT Init();
	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetCount(INT* pnRet);
	virtual HRESULT GetFirstRIM(IEKG3DRegionInfoManager** ppRIM, DWORD* dwFirstHandle, DWORD* pdwNextHandle);///使用Handle还是接口是可选的
	virtual HRESULT GetRIM(DWORD dwHandle, IEKG3DRegionInfoManager** ppRIM, DWORD* pdwNextHandle);
	virtual HRESULT Add(const TCHAR cszFileName[]
						, unsigned uFileNameHash
						, unsigned uOption
						, DWORD_PTR WParam
						, DWORD_PTR LParam
						, IEKG3DRegionInfoManager** ppRIM
						, DWORD* pdwHandle);///uOption是KG3DREGION_INFORMER_LOADFILE_PARAM*
	virtual HRESULT Del(DWORD dwHandle);
};
extern KG3DRegionInfoManagerTable g_cRegionInfoManagerTable;
//////////////////////////////////////////////////////////////////////////
#if defined(DEBUG) | defined(_DEBUG)
	void g_DebugRegionInformerPrintCompressedData(KG3DRegionInfoManager* pRI);
#endif



#endif //_INCLUDE_KG3DRegionInfoManager_H_
