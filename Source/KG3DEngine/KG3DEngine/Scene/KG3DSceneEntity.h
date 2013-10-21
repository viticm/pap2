////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneEntity.h
//  Version     : 1.0
//  Creator     : 
//  Create Date : 
//  Comment     : 2008-3-3ChenTianhong 利用Entity的m_pExecutor把Entity变成多态了。具体看CPP
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENEENTITY_H_
#define _INCLUDE_KG3DSCENEENTITY_H_

////////////////////////////////////////////////////////////////////////////////

#include "KG3DTypes.h"
#include "IEKG3DSceneEntity.h"
#include "KG3DStructs.h"

//可以选择性的派生适当的方法
class KG3DSceneEntityDummy
{
public:
	HRESULT OnSelect(){return E_FAIL;}
	HRESULT GetCenter(D3DXVECTOR3* pvCenter){return E_FAIL;}
	HRESULT GetTranslation(D3DXVECTOR3* pvPos){return E_FAIL;}
	HRESULT GetRotation(D3DXQUATERNION* pqRot){return E_FAIL;}
	HRESULT GetScaling(D3DXVECTOR3* pvScale){return E_FAIL;}
	HRESULT SetTranslation(D3DXVECTOR3* pvTrans){return E_FAIL;}
	HRESULT SetRotation(D3DXQUATERNION* pqRot){return E_FAIL;}
	HRESULT SetScaling(D3DXVECTOR3* pvScale){return E_FAIL;}
	HRESULT RenderOption(DWORD_PTR dwOption){return E_FAIL;}
	HRESULT RenderSelectMask(DWORD_PTR dwMask){return E_FAIL;}
	HRESULT GetBBox(AABBOX* pOutBox)
	{
		D3DXVECTOR3 vPos, vScale;
		KG_COM_PROCESS_ERROR(this->GetCenter(&vPos));
		{
			if(SUCCEEDED(this->GetScaling(&vScale)))
				*pOutBox = AABBOX(vPos + vScale/2, vPos - vScale/2);
			else
				*pOutBox = AABBOX(vPos + 5, vPos - 5);
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}
};

extern KG3DSceneEntityDummy	g_cKG3DSceneEntityDummy;

//class KG3DSceneEntityDummyExecutorBase
//{
//public:
//	virtual ~KG3DSceneEntityDummyExecutorBase() = 0{}
//	virtual HRESULT OnSelect(LPVOID) = 0;
//	virtual HRESULT GetCenter(LPVOID, D3DXVECTOR3*) = 0;
//	virtual HRESULT GetTranslation(LPVOID, D3DXVECTOR3*) = 0;
//	virtual HRESULT GetRotation(LPVOID, D3DXQUATERNION*) = 0;
//	virtual HRESULT GetScaling(LPVOID, D3DXVECTOR3*) = 0;
//	virtual HRESULT SetTranslation(LPVOID, D3DXVECTOR3*) = 0;
//	virtual HRESULT SetRotation(LPVOID, D3DXQUATERNION*) = 0;
//	virtual HRESULT SetScaling(LPVOID, D3DXVECTOR3*) = 0;
//	virtual HRESULT RenderOption(LPVOID, DWORD_PTR) = 0;
//	virtual HRESULT RenderSelectMark(LPVOID, DWORD_PTR) = 0;
//	virtual HRESULT GetBBox(LPVOID, AABBOX* OutBox) = 0;
//};

//template<class _Type>
//class KG3DSceneEntityDummyExecutor : public KG3DSceneEntityDummyExecutorBase
//{
//public:
//	virtual HRESULT OnSelect(LPVOID lpObj){return static_cast<_Type*>(lpObj)->OnSelect();}
//	virtual HRESULT GetCenter(LPVOID lpObj, D3DXVECTOR3* pvCenter){return static_cast<_Type*>(lpObj)->GetCenter(pvCenter);}
//	virtual HRESULT GetTranslation(LPVOID lpObj, D3DXVECTOR3* pvPos){return static_cast<_Type*>(lpObj)->GetTranslation(pvPos);}
//	virtual HRESULT GetRotation(LPVOID lpObj, D3DXQUATERNION* pqRot){return static_cast<_Type*>(lpObj)->GetRotation(pqRot);}
//	virtual HRESULT GetScaling(LPVOID lpObj, D3DXVECTOR3* pvScale){return static_cast<_Type*>(lpObj)->GetScaling(pvScale);}
//	virtual HRESULT SetTranslation(LPVOID lpObj, D3DXVECTOR3* pvTrans){return static_cast<_Type*>(lpObj)->SetTranslation(pvTrans);}
//	virtual HRESULT SetRotation(LPVOID lpObj, D3DXQUATERNION* pqRot){return static_cast<_Type*>(lpObj)->SetRotation(pqRot);}
//	virtual HRESULT SetScaling(LPVOID lpObj, D3DXVECTOR3* pvScale){return static_cast<_Type*>(lpObj)->SetScaling(pvScale);}
//	virtual HRESULT RenderOption(LPVOID lpObj, DWORD_PTR dwOption){return static_cast<_Type*>(lpObj)->RenderOption(dwOption);}
//	virtual HRESULT RenderSelectMark(LPVOID lpObj, DWORD_PTR dwMask){return static_cast<_Type*>(lpObj)->RenderSelectMask(dwMask);}
//	virtual HRESULT GetBBox(LPVOID lpObj, AABBOX* pOutBox){return static_cast<_Type*>(lpObj)->GetBBox(pOutBox);}
//};

class KG3DSceneEntityExecutor;

class KG3DSceneEntityExecutorFactory 
{
private:
	typedef std::map<DWORD, KG3DSceneEntityExecutor*> TypeMap;
	TypeMap	m_Map;
public:
	KG3DSceneEntityExecutorFactory();
	~KG3DSceneEntityExecutorFactory();
	bool RegisterExecutor(const KG3DSceneEntityExecutor& Exc);//会复制进去
	bool UnRegisterExecutor(DWORD dwType);
	KG3DSceneEntityExecutor& GetExecutor(DWORD dwType);
};

extern KG3DSceneEntityExecutorFactory g_KG3DSceneEntityExecutorFactory;

class KG3DSceneEntity;

struct KG3DSceneEntityVisitor 
{
	virtual HRESULT Accept(KG3DSceneEntity& Entity) = 0;
	virtual ~KG3DSceneEntityVisitor()=0{}
};

class KG3DSceneEntity : public IEKG3DSceneEntity
{
	enum
	{
		em_bit_mask_type = 0x4fffffff,
		em_bit_mask_selectable = 0x80000000,
	};
private:
	DWORD m_dwSelectableAndType;	//用其中的最高位来记录是否可以选择
	KG3DSceneEntityExecutor*	m_pExecutor;
public:
	PVOID           m_pPointer;
	PVOID           m_pRefData;
public:
	KG3DSceneEntity();//用默认构造函数的话是什么都不能做的，这个默认构造函数只是为了兼容std容器
	KG3DSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer);
	KG3DSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer,PVOID Data);
	//KG3DSceneEntity(KG3DSceneEntityDummyExecutorBase& Executor, PVOID Executee);//后面那个是被操作的实际类
	KG3DSceneEntity(const KG3DSceneEntity& Other);//注意：这个类没有拷贝构造函数和赋值操作符，却有virtual函数，这样会产生进行BinaryCopy的默认拷贝构造函数和默认赋值操作符。小心里面的指针，全是直接拷贝的。
	KG3DSceneEntity& operator=(const KG3DSceneEntity& Other);
	bool operator==(const KG3DSceneEntity &A) const
	{
		return /*(m_Type==A.m_Type)&&*/(m_pPointer==A.m_pPointer);// Last modify by Chentianhong 080304不需要检查Type的相等，因为Pointer相等，Type必须相等，不然强转的时候会出错,
	}
	friend bool operator<(const KG3DSceneEntity& A, const KG3DSceneEntity& B)
	{
		return A.m_pPointer < B.m_pPointer;
	}
   
public:
	//FrameMove And Render
	HRESULT FrameMove( );	

	HRESULT Render();
	HRESULT RenderOption(DWORD dwOption);
	HRESULT RenderSelectMark(DWORD dwColor);
	HRESULT RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir);
    HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
	
	//Translation
	virtual HRESULT SetTranslation(D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	void SetRotationCenter(D3DXVECTOR3* pValue);
    virtual HRESULT SetScaling(D3DXVECTOR3* pValue);
	void SetScalingRotation(D3DXQUATERNION* pValue);
	void SetScalingCenter(D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(D3DXVECTOR3 *pValue);
    virtual HRESULT GetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	void GetRotationCenter(D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(D3DXVECTOR3* pValue);
	void GetScalingRotation(D3DXQUATERNION* pValue);
	void GetScalingCenter(D3DXVECTOR3* pValue);
    D3DXMATRIX GetWorldMatrix();
    D3DXMATRIX GetMatrix();
    virtual HRESULT UpdateTransformation();
	virtual HRESULT RotationY(FLOAT fAngelY);
	virtual HRESULT RotationX(FLOAT fAngelX);
	virtual HRESULT RotationZ(FLOAT fAngelZ);
	void SetLocalMatrix(const D3DXMATRIX& mat);
    void SetScaleSize(float Value);
    void GetScaleSize(float* pValue);

	//属性
	BOOL	IsSelectable();
	VOID	SetSelectable(BOOL bSelectable);
	SCENEENTITYTYPE GetType() const ;
	VOID SetAlpha(float fAlpha);
	HRESULT GetName(LPTSTR pName, size_t BufferSize);
	HRESULT SetLOD(float fLod);
	//BOOL IsTransformated();
	//VOID SetTransformated(BOOL bTran);
	BOOL IsSecondUVExist();
	void GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);
	DWORD GetSceneLayerID();
	void  SetSceneLayerID(DWORD dwID);
	
	//BBox
	HRESULT GetBBox(AABBOX* pOutBox);//检查物体是否被选择中

	//碰撞
	/*BOOL IsRayIntersect(D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/
	virtual BOOL RayIntersectionForEntity(const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet);
    BOOL GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[], IN CONST D3DXVECTOR3 vDst[]
		,IN CONST INT nNum, OUT FLOAT* pDis);	
    virtual BOOL IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir);
	BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin
		,D3DXVECTOR3 Direction, BOOL bFiltrate = FALSE);
	/*BOOL GetRayIntersectPointAndDir(DWORD& dwCount, D3DXVECTOR3* pInterPoint
		, float* pAngle, const D3DXVECTOR3& vec3Org, const D3DXVECTOR3& vec3Dir);*/

	//行为
	HRESULT OnSelect();
	HRESULT OnUnSelect();
	HRESULT OnDelete() const;	//OnDelete不能改变里面的成员，这个函数是const的。这是因为list要remove掉一个Entity，是要执行==比较的，而且OnDelete之后，也要马上remove掉这个Entity才对
	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
    HRESULT Clone(KG3DSceneEntity *pEntity, float fX, float fY, float fZ);
    virtual HRESULT Reload();
	HRESULT ChangeModel(INT nChangeCode);   
	
	//没有用到，暂时屏蔽
	//void SetOCState(const BOOL &IsOC);
	//BOOL GetOCState();
	
	//IE接口 
	virtual HRESULT GetType(SCENEENTITYTYPE *peRetType);
	virtual HRESULT GetPointer(VOID **ppvRetPointer);
	virtual HRESULT GetRefData(VOID **ppvRetData);	
	virtual HRESULT Compare(IEKG3DSceneEntity *piEntity, BOOL *pbRetEqualFlag);
	//virtual HRESULT Clone(const KG3DSceneEntity& Other);
};

class KG3DSceneEntityExecutor
{
public:
	virtual ~KG3DSceneEntityExecutor(){};
	//必须实现
	virtual DWORD GetType() const = 0;
	virtual KG3DSceneEntityExecutor* Clone() const = 0;

	virtual VOID CopyEntity(const KG3DSceneEntity& Src, KG3DSceneEntity& Desc)const{memcpy_s(&Desc, sizeof(KG3DSceneEntity), &Src, sizeof(KG3DSceneEntity));}
	virtual VOID SetAlpha(KG3DSceneEntity& Entity, float fAlpha)				{}

	//FrameMove And Render
	virtual HRESULT FrameMove(KG3DSceneEntity& Entity)							{return E_FAIL;}
	
	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption)		{return E_FAIL;}
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor)	{return E_FAIL;}
	virtual HRESULT RenderPlaneShadow(KG3DSceneEntity&, D3DXPLANE&, const D3DXVECTOR4&) {return E_FAIL;}
	virtual HRESULT RenderShadowMap(KG3DSceneEntity&, LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP){return E_FAIL;}
	virtual HRESULT RenderReflect(KG3DSceneEntity& Entity, float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX){return E_FAIL;}

	
	//Translation
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue) {return E_FAIL;}
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ){return E_FAIL;}
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)		{}
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)			{return E_FAIL;}
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue)	{}
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)		{}
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)		{return E_FAIL;}
	virtual HRESULT GetRotation(KG3DSceneEntity&, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)	{return E_FAIL;}
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)		{}
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)			{return E_FAIL;}
	virtual VOID	GetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue)	{}
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)		{}
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT	UpdateTransformation(KG3DSceneEntity& Entity)						{return E_FAIL;}
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat)		{}
	virtual VOID	SetScaleSize(KG3DSceneEntity& Entity, FLOAT Value)					{}
	virtual VOID	GetScaleSize(KG3DSceneEntity& Entity, FLOAT* pValue)				{}
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT	SetLOD(KG3DSceneEntity& Entity, FLOAT fLOD)							{return E_FAIL;}
	//virtual BOOL	IsTransformated(KG3DSceneEntity& Entity)							{return FALSE;}
	//virtual VOID	SetTransformated(KG3DSceneEntity& Entity, BOOL bTran)				{}
	virtual BOOL	IsSecondUVExist(KG3DSceneEntity& Entity)							{return FALSE;}
	virtual VOID	GetProfile(KG3DSceneEntity& Entity, DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID){}
	virtual DWORD	GetSceneLayerID(KG3DSceneEntity& Entity)							{return 0;}
	virtual VOID	SetSceneLayerID(KG3DSceneEntity& Entity, DWORD ID)					{}
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox)					{return E_FAIL;}

	/*virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL)	{return FALSE;}*/
	virtual BOOL RayIntersectionForEntity(KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet);
	virtual BOOL GetCameraNearRayIntersect(KG3DSceneEntity& Entity, IN CONST D3DXVECTOR3 vSrc[]
		, IN CONST D3DXVECTOR3 vDst[],IN CONST INT nNum, OUT FLOAT* pDis)				{return FALSE;}	
	virtual BOOL IsVertexInside(KG3DSceneEntity& Entity, D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir){return FALSE;}
	virtual BOOL GetRayIntersect(KG3DSceneEntity& Entity, DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin
		,D3DXVECTOR3 Direction, BOOL bFiltrate = FALSE)									{return FALSE;}
	/*virtual BOOL GetRayIntersectPointAndDir(KG3DSceneEntity& Entity, DWORD& dwCount, D3DXVECTOR3* pInterPoint
		, float* pAngle, const D3DXVECTOR3& vec3Org, const D3DXVECTOR3& vec3Dir)		{return FALSE;}*/

	virtual HRESULT OnSelect(KG3DSceneEntity& Entity)									{return S_OK;}
	virtual HRESULT OnUnSelect(KG3DSceneEntity& Entity)									{return S_OK;}
	virtual HRESULT ProcessMessage(KG3DSceneEntity& Entity, UINT message, WPARAM wParam, LPARAM lParam)	{return S_OK;}//<PROCESS_MESSAGE>
	virtual HRESULT Clone(KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ)	{return E_FAIL;}
	virtual HRESULT Reload(KG3DSceneEntity& Entity)										{return E_FAIL;}
	virtual HRESULT ChangeModel(KG3DSceneEntity& Entity, INT nChangeCode)				{return E_FAIL;}
	virtual HRESULT OnDelete(const KG3DSceneEntity& Entity)								{return S_OK;}//默认应该是Ok必须保证不能改变Entity，不然remove没法进行
};
//SSE的意思就是SceneEntityExecutor
class KG3DSEE_None : public KG3DSceneEntityExecutor
{
public:
	virtual KG3DSceneEntityExecutor* Clone()const {return new KG3DSEE_None;}
	virtual DWORD GetType() const {return SCENEENTITY_NONE;}
	~KG3DSEE_None(){}
};
//KG3DSEETest	用于感知方法调用的正确与否
class KG3DSSETest : public KG3DSceneEntityExecutor
{
public:
	KG_CUSTOM_HELPERS::KGBitSet<2>	m_Bits;
	enum
	{
		SET_ALPHA,
		FRAME_MOVE,
	};
	~KG3DSSETest(){}
	virtual DWORD	GetType() const{return SCENEENTITY_TEST;}
	virtual KG3DSceneEntityExecutor* Clone() const {return new KG3DSSETest;}

	virtual VOID SetAlpha(KG3DSceneEntity& Entity, float fAlpha)				{m_Bits.SetTrue<SET_ALPHA>();}
	virtual HRESULT FrameMove(KG3DSceneEntity& Entity)							{m_Bits.SetTrue<FRAME_MOVE>();return S_OK;}
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox)			{*pOutBox = AABBOX(D3DXVECTOR3(10,10,10),D3DXVECTOR3(-10,-10,-10)); return S_OK;}
};


//////////////////////////////////////////////////////////////////////////
class KG3DSEEObject : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEEObject(){}
	virtual DWORD	GetType() const{return SCENEENTITY_OBJECT;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT	FrameMove(KG3DSceneEntity& Entity);
	virtual	HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);
	virtual HRESULT RenderPlaneShadow(KG3DSceneEntity& Entity, D3DXPLANE& plane,const D3DXVECTOR4& LightDir);
	virtual HRESULT RenderShadowMap(KG3DSceneEntity& Entity, LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	virtual HRESULT RenderReflect(KG3DSceneEntity& Entity, float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	GetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT	UpdateTransformation(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
	virtual VOID	SetScaleSize(KG3DSceneEntity& Entity, FLOAT Value);
	virtual VOID	GetScaleSize(KG3DSceneEntity& Entity, FLOAT* pValue);

	virtual VOID	SetAlpha(KG3DSceneEntity& Entity, float fAlpha);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT	SetLOD(KG3DSceneEntity& Entity, FLOAT fLOD);
	//virtual BOOL	IsTransformated(KG3DSceneEntity& Entity);
	//virtual VOID	SetTransformated(KG3DSceneEntity& Entity, BOOL bTran);
	virtual BOOL	IsSecondUVExist(KG3DSceneEntity& Entity);
	virtual VOID	GetProfile(KG3DSceneEntity& Entity, DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);
	virtual DWORD	GetSceneLayerID(KG3DSceneEntity& Entity);
	virtual VOID	SetSceneLayerID(KG3DSceneEntity& Entity, DWORD ID);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

	/*virtual BOOL	IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/
	virtual BOOL RayIntersectionForEntity(KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet);
	virtual BOOL	GetCameraNearRayIntersect(KG3DSceneEntity& Entity, IN CONST D3DXVECTOR3 vSrc[]
		, IN CONST D3DXVECTOR3 vDst[],IN CONST INT nNum, OUT FLOAT* pDis);
	virtual BOOL	IsVertexInside(KG3DSceneEntity& Entity, D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir);
	virtual BOOL	GetRayIntersect(KG3DSceneEntity& Entity, DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin
		,D3DXVECTOR3 Direction, BOOL bFiltrate = FALSE);
	/*virtual BOOL	GetRayIntersectPointAndDir(KG3DSceneEntity& Entity, DWORD& dwCount, D3DXVECTOR3* pInterPoint
		, float* pAngle, const D3DXVECTOR3& vec3Org, const D3DXVECTOR3& vec3Dir);*/

	virtual HRESULT OnSelect(KG3DSceneEntity& Entity);
	virtual HRESULT OnUnSelect(KG3DSceneEntity& Entity);
	virtual HRESULT ProcessMessage(KG3DSceneEntity& Entity, UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	virtual HRESULT Reload(KG3DSceneEntity& Entity);
	virtual HRESULT ChangeModel(KG3DSceneEntity& Entity, INT nChangeCode);
};	
//////////////////////////////////////////////////////////////////////////
class KG3DSEEModel : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEEModel(){}
	virtual DWORD	GetType() const{return SCENEENTITY_MODEL;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT	FrameMove(KG3DSceneEntity& Entity);
	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);
	virtual HRESULT RenderPlaneShadow(KG3DSceneEntity& Entity, D3DXPLANE& plane,const D3DXVECTOR4& LightDir);
	virtual HRESULT RenderShadowMap(KG3DSceneEntity& Entity, LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
	virtual HRESULT RenderReflect(KG3DSceneEntity& Entity, float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
	
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	GetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
	virtual VOID	SetScaleSize(KG3DSceneEntity& Entity, FLOAT fSize);
	virtual VOID	GetScaleSize(KG3DSceneEntity& Entity, FLOAT* pValue);

	virtual VOID	SetAlpha(KG3DSceneEntity& Entity, float fAlpha);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT	SetLOD(KG3DSceneEntity& Entity, FLOAT fLOD);
	//virtual BOOL	IsTransformated(KG3DSceneEntity& Entity);
	//virtual VOID	SetTransformated(KG3DSceneEntity& Entity, BOOL bTran);
	virtual BOOL	IsSecondUVExist(KG3DSceneEntity& Entity);
	virtual VOID	GetProfile(KG3DSceneEntity& Entity, DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);
	virtual DWORD	GetSceneLayerID(KG3DSceneEntity& Entity);
	virtual VOID	SetSceneLayerID(KG3DSceneEntity& Entity, DWORD ID);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

	/*virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/
	virtual BOOL RayIntersectionForEntity(KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet);
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEEGroup : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEEGroup(){}
	virtual DWORD	GetType() const{return SCENEENTITY_GROUP;}
	virtual KG3DSceneEntityExecutor* Clone() const; 

	virtual HRESULT	FrameMove(KG3DSceneEntity& Entity);
	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual HRESULT	UpdateTransformation(KG3DSceneEntity& Entity);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEESFXMeteor : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESFXMeteor(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SFX_METEOR;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	GetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
	/*virtual BOOL	IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/ //改在基类中实现
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEESFXLauncher : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESFXLauncher(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SFX_LAUNCHER;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT FrameMove(KG3DSceneEntity& Entity);
	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);

	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
	
	/*virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/ //改在基类中实现

	virtual HRESULT Clone(KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ);
};

//////////////////////////////////////////////////////////////////////////
class KG3DSEESFXBoxProjection : public KG3DSceneEntityExecutor
{
public:
    ~KG3DSEESFXBoxProjection(){}
    virtual DWORD	GetType() const{return SCENEENTITY_SFX_BOX_PROJ;}
    virtual KG3DSceneEntityExecutor* Clone() const;

    virtual HRESULT FrameMove(KG3DSceneEntity& Entity);
    virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
    virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

    virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
    virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
    virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
    virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
    virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
    virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
    virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);

    virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
    virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

    /*virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
    , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/ //改在基类中实现

    virtual HRESULT Clone(KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ);
};

//////////////////////////////////////////////////////////////////////////
class KG3DSEESFXTrack : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESFXTrack(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SFX_TRACK;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT FrameMove(KG3DSceneEntity& Entity);
	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	SetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	GetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	GetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual VOID	GetScalingCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEESFXBillboard : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESFXBillboard(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SFX_BILLBOARD;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual VOID	SetScalingRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
	virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
	virtual	VOID	SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
	virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

	/*virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin
		, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/	//改在基类中实现

	virtual HRESULT Clone(KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ);
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEEPosition : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEEPosition(){}
	virtual DWORD	GetType() const{return SCENEENTITY_POSITION;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);

	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
};


/*
 * Pvs Portal Entity (Just for Scene Editor)
 */

class KG3DSEEPvsPortal : public KG3DSceneEntityExecutor
{
public :
    virtual ~KG3DSEEPvsPortal(){}
    virtual DWORD GetType() const { return SCENEENTITY_PVS_PORTAL; }
    virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT FrameMove(KG3DSceneEntity& Entity);
    virtual	HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
    virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

    virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
    virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
    virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
    virtual HRESULT	GetName(KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize);

    virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);
    virtual HRESULT OnSelect(KG3DSceneEntity& Entity);

    virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
    virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
    virtual void  SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
   /* virtual BOOL IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin,
        D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys = NULL);*/	//改在基类中实现
};


//////////////////////////////////////////////////////////////////////////
class KG3DSEESpeedTreeLeaf : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESpeedTreeLeaf(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SPEEDTREELEAF;}
	virtual KG3DSceneEntityExecutor* Clone() const;

	virtual HRESULT RenderOption(KG3DSceneEntity& Entity, DWORD dwOption);
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor);

	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);

	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

	virtual HRESULT ProcessMessage(KG3DSceneEntity& Entity, UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	virtual HRESULT OnDelete(const KG3DSceneEntity& Entity);
};
//////////////////////////////////////////////////////////////////////////
class KG3DSEESpeedTreeLeafGroup : public KG3DSceneEntityExecutor
{
public:
	~KG3DSEESpeedTreeLeafGroup(){}
	virtual DWORD	GetType() const{return SCENEENTITY_SPEEDTREELEAFGROUP;}
	virtual KG3DSceneEntityExecutor* Clone() const;
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual VOID	SetRotationCenter(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT SetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT GetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ);
	virtual HRESULT GetScaling(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue);
	virtual HRESULT UpdateTransformation(KG3DSceneEntity& Entity);
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox);

    virtual D3DXMATRIX GetWorldMatrix(KG3DSceneEntity& Entity);
    virtual D3DXMATRIX GetMatrix(KG3DSceneEntity& Entity);
    virtual	VOID	   SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat);
};
//////////////////////////////////////////////////////////////////////////
typedef std::list<KG3DSceneEntity>	KG3DSceneEntityList;//统一使用这个类型，而不直接用KG3DSceneEntityList,以后可以就这个类型进行重构


class EntityList 
{
public:
	EntityList();
	EntityList(const std::list<KG3DSceneEntity>& Entitys);
	size_t      GetSize();
	void        Clear();
	void        AddEntity(const KG3DSceneEntity& Entity);
	D3DXVECTOR3 GetCenter();
	D3DXMATRIX  GetWorldMatrix();
	D3DXMATRIX  GetMatrix();
	void        SetLocalMatrix(const D3DXMATRIX& Mat);
	void        Translation(const D3DXVECTOR3& vMove);
	void        Scale(const D3DXVECTOR3& vScaling, DWORD dwMode);
	void        Rotation(const D3DXMATRIX& Mat);
	void        Update();
	void        GetBBox(AABBOX* Box);
    const std::list<KG3DSceneEntity>& GetEntityList() const { return listEntitys; }
private:
	std::list<KG3DSceneEntity> listEntitys;
};

#endif //_INCLUDE_KG3DSCENEENTITY_H_
