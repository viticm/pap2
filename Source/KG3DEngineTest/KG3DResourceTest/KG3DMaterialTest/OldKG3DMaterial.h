#pragma once
#include "kg3dinterface.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月8日
#include "kg3deffect.h"
#include "IEKG3DMaterial.h"


#define TEXTURE_TYPE_DIFFUSE "*MAP_DIFFUSE"
#define TEXTURE_TYPE_AMBIENT "*MAP_AMBIENT"

void g_InitTextureData(_TextureData *pData);
void g_UnInitTextureData(_TextureData *pData);

////////////////////////////////////////////////////////////////////////////////
class KG3DModel;
class KG3DTexture;
class Semantic2TextureTool;

struct KG3DMaterialDetail
{
	D3DCOLORVALUE m_cMainColor;
	float         m_fMainColorMin;
	float         m_fMainColorMax;
	float         m_fMainColorScale;
	float         m_fEnv;
	float         m_fSpecular;
	string        m_szChannelFileName;

	string m_szDetail0FileName;
	string m_szDetail1FileName;

	KG3DTexture* m_lpChannelTexture;
	KG3DTexture* m_lpDetailTexture0;
	KG3DTexture* m_lpDetailTexture1;

	float  m_fDetail0UVScale;
	float  m_fDetail1UVScale;
	float  m_fDetail0ColorScale;
	float  m_fDetail1ColorScale;
	float  m_fDetail0Env;
	float  m_fDetail0Specular;
	float  m_fDetail1Env;
	float  m_fDetail1Specular;

	D3DCOLORVALUE m_cDetial0Color;
	D3DCOLORVALUE m_cDetial1Color;
	string m_szName;

	KG3DMaterialDetail();
	~KG3DMaterialDetail();

	HRESULT SaveDefineToIni(IIniFile* pFile,LPSTR SectionName);
	HRESULT LoadFormIni(IIniFile* pFile,LPSTR SectionName);

	HRESULT LoadTextures();

	BOOL IsResourceReady();
};

class KG3DMaterial : public IKG3DModelMaterial, public IEKG3DMaterial
{
	friend class Semantic2TextureTool;
public:
	virtual HRESULT GetEffParaSetsVectorItemVectorSize(DWORD dwPosition, DWORD *pdwSize);
	virtual HRESULT GetEffParaSetsVectorItemShadeItemName(
		DWORD dwEffParaSetsIndex,
		DWORD dwShadeIndex, 
		LPCTSTR *ppszName
		);
	virtual HRESULT GetEffParaSetsVectorItemShadeItemType(
		DWORD dwEffParaSetsIndex,
		DWORD dwShadeIndex, 
		DWORD *pdwType
		);
	virtual HRESULT GetEffParaSetsVectorItemShadeItemData(
		DWORD dwEffParaSetsIndex,
		DWORD dwShadeIndex, 
		PVOID *ppvData
		);
	virtual HRESULT OnLostDevice(){return S_OK;};
	virtual HRESULT OnResetDevice(){return S_OK;};

	virtual HRESULT GetSubMaterialItemEffectString(DWORD dwIndex, LPCTSTR pszName, LPCTSTR *ppszString);
	virtual HRESULT GetName(LPCTSTR *ppszName);
	virtual HRESULT GetMaterialCount(DWORD *pdwCount);
	virtual HRESULT GetSubMaterialItemAlphaOp(DWORD dwIndex, _MtlAlphaOperation **ppAlphaOp);

	virtual HRESULT SetSubMaterialColorRef2Value(
		DWORD dwIndex, 
		DWORD dwAmbientColor,
		DWORD dwDiffuseColor,
		DWORD dwEmissive,
		DWORD dwSpecular
		);
	virtual HRESULT GetSubMaterialOption(DWORD dwIndex, DWORD *pdwOption);
	virtual HRESULT SetSubMaterialOption(DWORD dwIndex, DWORD dwOption);

	virtual HRESULT GetSubMaterial(DWORD dwIndex, PVOID *ppSubMat);
	virtual void GetUseResource(vector<string>& vNames);
	virtual HRESULT GetSubMaterialTextureID(DWORD dwIndex, DWORD *pdwTextureIDs);

	virtual HRESULT RemoveTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex);

	virtual HRESULT ChangeTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex, 
		LPCSTR strNewTextureFileName);

	virtual HRESULT GetTexturePathName(DWORD dwSubsetIndex,
		DWORD dwIndex,
		LPSTR strBuffer, 
		DWORD dwSize);
public:
	struct _MtlLogical
	{
		DWORD dwOption;
	};

	struct _MtlCull 
	{
		DWORD CullMode;
		DWORD BakCullMode;
	};

	struct _MtlBlend
	{
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BakSrcBlend;
		DWORD BakDestBlend;
		_MtlBlend();
	};

	struct _MtlTextureMap
	{
		DWORD TextureMap;
		DWORD BakTextureMap;
		_MtlTextureMap();

	};

	struct _MtlTextureOp
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD TFactor;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakTFactor;

		_MtlTextureOp();
	};

	//////////////////////////////////////////////////////////////////////////
	//高级的图层操作
	struct _MtlTextureOpEx
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD ColorArg0;
		DWORD Result;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD AlphaArg0;
		DWORD AlphaResult;
		DWORD TFactor;
		DWORD Constant;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakColorArg0;
		DWORD BakResult;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakAlphaArg0;
		DWORD BakTFactor;
		DWORD BakConstant;

		_MtlTextureOpEx();

	};	

	struct _MtlTextureTf
	{
		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Speed_Scaling;
		D3DXVECTOR3 Speed_Rotation;
		D3DXVECTOR3 Speed_Position;
	};

	struct _MtlTextureData
	{
		std::string FileName;
		DWORD  TextureID;

		_MtlTextureData();

		~_MtlTextureData();

	};

	struct _ShaderConstant
	{
		DWORD  dwType;
		std::string scName;
		PVOID  pData;
		D3DXHANDLE m_hParameter;

		_ShaderConstant()
		{
			dwType = 0;
			pData  = NULL;
			m_hParameter = NULL;
		}

		_ShaderConstant(DWORD Type, std::string Name,PVOID Data,D3DXHANDLE hParameter)
		{
			dwType = Type;
			scName = Name;
			pData = Data;
			m_hParameter = hParameter;
		}
		~_ShaderConstant()
		{
			Clear();
		}
		//GetDesc(): When return S_OK, means the parameter is not automatic alternated by material
		//           Otherwise, it could be modified manually
		HRESULT GetDesc(LPSTR strDesc);
		HRESULT Clear();
		bool operator == (_ShaderConstant& Const);

	};

	struct _MatCull 
	{
		DWORD CullMode;
		DWORD BakCullMode;
	};

	struct _TextureMap
	{
		DWORD TextureMap;
		DWORD BakTextureMap;
		_TextureMap();
	};

	struct _TextureTf
	{
		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Speed_Scaling;
		D3DXVECTOR3 Speed_Rotation;
		D3DXVECTOR3 Speed_Position;
	};

	struct _TextureOp
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD TFactor;
		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakTFactor;
		_TextureOp();
	};

	struct _TextureOpEx
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD ColorArg0;
		DWORD Result;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD AlphaArg0;
		DWORD AlphaResult;
		DWORD TFactor;
		DWORD Constant;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakColorArg0;
		DWORD BakResult;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakAlphaArg0;
		DWORD BakTFactor;
		DWORD BakConstant;
		_TextureOpEx();

	};	

	struct _MatBlend
	{
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BakSrcBlend;
		DWORD BakDestBlend;
		_MatBlend();	
	};

	struct _MtlOption
	{
		DWORD Type;
		PVOID pData;
		_MtlOption();

		~_MtlOption();

	};

	struct KG3DMaterialSubset
	{
		const static int cNumID = 8;
		const static int cNumColorCast = 8;
		D3DMATERIAL9      m_sMaterial9;
		DWORD            m_dwNumFaces;
		//DWORD            m_dwTextureIDs[cNumID];
		KG3DTexture*     m_lpTextures[cNumID];
		DWORD            m_pTextureNumOption[cNumID];
		DWORD            m_dwOption;
		DWORD            m_dwNumOptions;
		DWORD            m_dwHashValue;//
		DWORD            m_dwNumUsedTexture;
		KG3DTexture*     m_lpNormalMap;

		D3DCOLORVALUE    m_ColorCast[cNumColorCast];
		float            m_fSpecPower;
		float            m_fEmssPower;

		std::string      m_scTextureName[cNumID];
		std::string      m_scTextureType[cNumID];

		std::vector<_MtlOption*> m_pTextureOptions[cNumID];
		std::vector<_MtlOption*> m_pMatOptions;

		LPD3DXEFFECT m_pEffect;
		TCHAR strEffectFileName[MAX_PATH];

		int m_nEffParaIndex;
		int m_nCurTechnique;

		DWORD m_dwMaterialID;//用来做材质排序用的id
		DWORD m_dwPassID;//使用的Shader的Pass
		KG3DMaterialSubset();
		~KG3DMaterialSubset();

		DWORD GetAlphaType();
		_MtlAlphaOperation* GetAlphaOperation();
		_MtlRenderTwiceAlphaOption* GetAlphaRenderTwice();

		_MtlTextureOpEx* GetTextureStageOperation(DWORD TextIndex);

		KG3DTexture* GetSubMeshTexture();
		KG3DTexture* GetSubMeshTextureNormal();

		HRESULT SetMaterialEffect(float fAlpha,DWORD dwOption,int nShader,LPD3DXEFFECT pEffect);

		HRESULT SetBillboardCloudMaterial();
		HRESULT RestoreBillboardCloudMaterial();

		HRESULT SetMaterial(float fAlpha,DWORD dwOption);
		HRESULT RestoreMaterial();
		HRESULT SetOption(DWORD dwTextureStage, _MtlOption* pOption);
		HRESULT LoadTexturOption(IFile* pFile, DWORD dwTextureStageIndex);
		HRESULT LoadMaterialOption(IFile* pFile);
		HRESULT LoadEffectFromFile(const char* cszFilename);

		void  ClearTextureStage(DWORD dwStageIndex);
		void* GetMtlOption(DWORD dwType, DWORD dwExtInfo);
		void* FindOption(DWORD dwType, DWORD dwExtInfo);

		DWORD ComputeHashValue();

		void CleanMtlOptions();
		void CleanTextureStageOptions();
		void CleanTextureStageOptions(DWORD dwStage);

	};

	typedef KG3DMaterialSubset* LPMATERIAL;

	struct _EffectParameterHeader
	{
		DWORD dwMask;
		DWORD dwVersion;
		DWORD dwNumParaSets;
		const static DWORD m_dwEffHeaderMask = 0x45465041;
		const static DWORD m_dwCurrentVersion = 0x1;
	};
	DWORD      m_dwType;//类型
	DWORD      m_dwID;
	DWORD      m_dwNumMaterials;

	int        m_nLastEffect;
	BOOL       m_bSortAsSFX;
	BOOL       m_bHasDetail;
	BOOL       m_bInMultiThreadLoadList;
	UINT       m_uOption;
	ULONG      m_ulRefCount;//物品使用计数
	std::string m_scName;
	KG3DMaterial(void);
	virtual ~KG3DMaterial(void);
	//////////////////////////////////////////////////////////////////////////
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT LoadFromFile(const char cszFileName[], UINT uFileNameHash, UINT uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   UINT uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual HRESULT RefreshOption(unsigned uOption);
	//////////////////////////////////////////////////////////////////////////
	//ID,type
	virtual DWORD GetID();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}
	virtual unsigned GetType();
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption) ;//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT RestoreMaterial(DWORD MaterialID);

	virtual HRESULT SetMaterial(DWORD dwIndex, float fAlpha, DWORD dwOption, void** pExtInfo);
	virtual HRESULT SetMaterial(DWORD AmbientColor, DWORD MaterialID, float fAlpha);
	HRESULT Effect_SetParameter(int nEffectParaIndex);
	HRESULT Effect_OnLoad(int nIndex, KG3DMaterialSubset* pMtl);
	HRESULT CreateSubset(DWORD dwNumSubset);
	HRESULT LoadFromFileMultiThread();

	//shader type
	virtual void SetShaderType(DWORD dwType);
	virtual DWORD GetShaderType();

	BOOL IsLoaded()
	{
		return m_eLoadingState == STATE_LOADCOMPLETE;
	}

	enum MtlLoadState
	{
		STATE_NONE,
		STATE_MULTITHREADFINISH,
		STATE_LOADCOMPLETE,
	}m_eLoadingState;

	vector<KG3DMaterialDetail>m_vecDetailDefine;
	virtual int GetMaterialDetailDefineCount();
	virtual HRESULT AddMaterialDetailDefine()     ;
	virtual HRESULT DeleteMaterialDetailDefine(int nIndex);

	HRESULT SaveMaterialDetails(LPSTR pFileName);
	HRESULT LoadMaterialDetails(LPSTR pFileName);

protected:
	HRESULT LoadFromFileDirect(const TCHAR cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption);

	virtual HRESULT OnReferenceZero();
	std::vector<std::vector<_ShaderConstant*>*> m_EffParaSets;//Effect parameter sets;
	void SetSubsetPassID();
private:
	inline void Swap( D3DCOLORVALUE &color1, D3DCOLORVALUE &color2 );
	HRESULT LoadSavedEffect(IFile* pFile, KG3DMaterialSubset* pMtl);
	//HRESULT SetOption( DWORD TextureStage, _MtlOption* pOption );
	HRESULT SaveOption(_MtlOption* pOption, FILE *fpFile);
	HRESULT SaveEffectParameter(FILE* fpFile);
	HRESULT LoadEffectParameter(IFile* pFile);
	void ClearEffectParaSets();
	void Helper_MoveEffectParameter2SubMaterial();
	DWORD m_ShaderType;

	LPMATERIAL m_pMaterials;
public:
	HRESULT ClearShaderForSubset(unsigned int uIndex);//by dengzhihui Refactor 2006年11月13日
	virtual DWORD GetNumMaterial( ) { return m_dwNumMaterials; }

	DWORD GetNumEffectParameterSets() { return static_cast<DWORD>(m_EffParaSets.size());}
	HRESULT ClearUnusedEffectParameterSet(int& nNumSetCleared);
	KG3DMaterialSubset*  GetSubMaterial(DWORD dwIndex);
	LPDIRECT3DTEXTURE9  GetSpeedTreeLeafTexture(DWORD MaterialID);
	void CopyOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest);

	HRESULT LoadShaderForSubset(unsigned int uIndex, 
		LPCSTR strFileName);

	HRESULT SetSubsetTexture(unsigned int uSubsetID, 
		unsigned int uStageID,
		KG3DTexture* pTexture);

	virtual BOOL IsResourceReady();

	HRESULT ReloadTexture(DWORD dwOption);

	void ModifyLoadOption(DWORD &dwTextureLoadOption, 
		DWORD dwSubsetID);

	virtual BOOL IsSortAsSFX(){return m_bSortAsSFX;};
	virtual HRESULT SetSortAsSFX(BOOL bSort){m_bSortAsSFX = bSort;return S_OK;};

	virtual HRESULT GetMaterilDetail(int nIndex,KG3DMaterialDetail* pDest);

	virtual HRESULT GetMaterilDetail(int nDetailType,LPSTR pszName,float* pfEnv,float* pfSpecular,
		float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,LPSTR pChannelFile,
		float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
		float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File);

	virtual HRESULT SetMaterilDetail(int nDetailType,LPSTR pszName,float fEnv,float fSpecular,
		float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,LPSTR pChannelFile,
		float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
		float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File);
private:
	LPMATERIAL m_pMaterialsMulti;
	DWORD      m_dwNumMaterialsMulti;
	vector<KG3DMaterialDetail>m_vecDetailDefineMulti;

	BOOL bTest;
public:


};

enum enuMatrixIndex
{
	ENUM_MATRIX_WORLD,
	ENUM_MATRIX_VIEW,
	ENUM_MATRIX_PROJ,
	ENUM_MATRIX_WORLDVIEW, 
	ENUM_MATRIX_VIEWPROJ,
	ENUM_MATRIX_WORLDVIEWPROJ,
	ENUM_MATRIX_COUNT,
};

class EffectParameterSettingTool
{
private:
	//ENU_MSEFFECT_WORLD
	static HRESULT SetWorldMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDI
	static HRESULT SetWorldIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_VIEW
	static HRESULT SetViewMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_PROJECTION
	static HRESULT SetProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEW
	static HRESULT SetWorldViewMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEWPROJ
	static HRESULT SetWorldViewProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_VIEWI
	static HRESULT SetViewIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDIT
	static HRESULT SetWorldITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_VIEWIT
	static HRESULT SetViewITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_PROJECTIONI
	static HRESULT SetProjIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_PROJECTIONIT
	static HRESULT SetProjITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEWI
	static HRESULT SetWorldViewIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEWIT
	static HRESULT SetWorldViewITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEWPROJI
	static HRESULT SetWorldViewProjIMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_WORLDVIEWPROJIT
	static HRESULT SetWorldViewProjITMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_VIEWPROJ
	static HRESULT SetViewProjMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_POSITION
	static HRESULT SetPos(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_DIRECTION
	static HRESULT SetDir(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_COLORRGBA
	static HRESULT SetColor(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_VOID
	static HRESULT SetVoid(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_BOOL
	static HRESULT SetBOOL(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_INT
	static HRESULT SetInt(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FLOAT
	static HRESULT SetFloat(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FLOAT2
	static HRESULT SetFloat2(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FLOAT3
	static HRESULT SetFloat3(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FLOAT4
	static HRESULT SetFloat4(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTURE
	static HRESULT SetTexture(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTURE1D
	static HRESULT SetTexture1D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTURE2D
	static HRESULT SetTexture2D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTURE3D
	static HRESULT SetTexture3D(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTURECUBE
	static HRESULT SetTextureCube(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_TEXTUREVOLUME
	static HRESULT SetTextureVolume(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_MATERIALAMBIENT
	static HRESULT SetAmbient(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_MATERIALDIFFUSE
	static HRESULT SetDiffuse(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_MATERIALSPECULAR
	static HRESULT SetSpecular(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_MATERIALEMISSIVE
	static HRESULT SetEmissive(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_MATERIALPOWER
	static HRESULT SetPower(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_LOCALBBOXMIN
	static HRESULT SetBBoxMin(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_LOCALBBOXMAX
	static HRESULT SetBBoxMax(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_LOCALBBOXSIZE
	static HRESULT SetBoxSize(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_STRING
	static HRESULT SetString(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_LIGHTDIR0-7
	static HRESULT SetLightDir(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_LIGHTDIFF0-7
	static HRESULT SetLightDiff(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_ENVAMB		
	static HRESULT SetEnvAmbient(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_SUNDIRMATRIX
	static HRESULT SetSunDirMatrix(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FOGINFO
	static HRESULT SetFogInfo(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	//ENU_MSEFFECT_FOGCOLOR
	static HRESULT SetFogColor(D3DXMATRIX* pMats, void* pData, KG3DMaterial::_ShaderConstant* pConst);
	static LPD3DXEFFECT m_pEffect;
	static BOOL m_bMatrixSet[ENUM_MATRIX_COUNT];
public:
	HRESULT (*m_pFunctions[ENU_MSEFFECT_COUNT])(D3DXMATRIX*, void*, KG3DMaterial::_ShaderConstant*);
	static void SetEffect(LPD3DXEFFECT pEffect);
	EffectParameterSettingTool();
};

static EffectParameterSettingTool* GetEffectParameterSettingTool();


struct Semantic2TextureFinder
{
	TCHAR* strSemantic;
	HRESULT(*pFunc)(KG3DMaterial*, int, TCHAR*);
};

class Semantic2TextureTool
{
public:
	Semantic2TextureTool();

	HRESULT Apply(KG3DMaterial *pMaterial, 
		int nNumToSet, 
		DWORD* pIndices);

private:

	HRESULT AutoFindMapsBySemantics(KG3DMaterial* pMaterial, 
		int nIndex, 
		const TCHAR* strSemantic, 
		TCHAR* strRetFileName);
	std::vector<Semantic2TextureFinder> m_vecSemantic2MapFinder;
};

extern Semantic2TextureTool g_S2TTool;
extern float g_fHightLight;

