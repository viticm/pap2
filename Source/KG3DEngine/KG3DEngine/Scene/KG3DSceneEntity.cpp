/************************************************************************/
/* 
KG3DSceneEntity利用m_pExecutor加入多态，不需要Switch Case了。Entity只负责保留数据
，行为由Executor来实现。所有对Entity的调用都转移到Executor，而Executor不保留数据。
数据和行为分离。

好处1.添加新种类的Entity不需要改动SceneEntity的cpp，只需要
g_KG3DSceneEntityExecutorFactory.RegisterExecutor(XXEntityExecutor());
就可以了，KG3DSceneEntity的构造函数会根据类型得到正确的Executor。
好处2.是感知和改变行为变得很方便，只需要派生一个现成的Executor，那么
喜欢感知或者覆盖某个行为的话，只要覆盖那个虚函数就可以了。

新的Executor（相当于新的Entity）的要求很简单：
1.从KG3DSceneExecutorBase派生，如果需要全局使用，需要添加一个SCENEENTITY_XXX的定值
2.派生类必须写出DWORD GetType()const和KG3DSceneExecutorBase* Clone()const两个函数
其中GetType只需要返回一个DWORD类型就可以了，Clone只需要new XXEntityExecutor;就可以了
3.覆盖你需要覆盖的函数。可以什么都不覆盖
4.注册你的新的Executor
g_KG3DSceneEntityExecutorFactory.RegisterExecutor(XXEntityExecutor());
5.当你用KG3DSceneEntity XX(SCENEENTITY_XX);的时候，KG3DSceneEntity会根据Type
在g_KG3DSceneEntityExecutorFactory中找到你的Executor，然后填入到m_pExecutor中
，这个Entity的行为就是你定义的Executor的行为

坏处是纯函数调用方面，因为转移了一层调用，会损失一次多态函数调用的时间。

*/
/************************************************************************/
#include "StdAfx.h"
#include "KG3DSceneEntity.h"
#include "KG3DSFX.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneEntityGroup.h"
#include "KG3DSpeedTreeLeafGroup.h"
#include "MathTool.h"
#include "KG3DMessenger.h"
#include "KG3DRepresentObjectPVS.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



extern BOOL g_bClient;
KG3DSceneEntityExecutorFactory g_KG3DSceneEntityExecutorFactory;
#if defined(KG_ENABLE_TDD)
#define  ILLEGAL_PTR ((LPVOID)0x00000003) //SceneEntity的构造函数不允许空指针，这里骗它一下
KG_TEST_BEGIN(KG3DSceneEntity)
{		
	/*bool bRet = */g_KG3DSceneEntityExecutorFactory.RegisterExecutor(KG3DSSETest());
	//_ASSERTE(bRet);

	KG3DSceneEntityExecutor& SEE = g_KG3DSceneEntityExecutorFactory.GetExecutor(SCENEENTITY_TEST);
	_ASSERTE(SEE.GetType() == SCENEENTITY_TEST);
	KG3DSSETest& SEETest = dynamic_cast<KG3DSSETest&>(SEE);

	//两个方法没有被调用过
	_ASSERTE(! SEETest.m_Bits.Is<KG3DSSETest::FRAME_MOVE>());
	_ASSERTE(! SEETest.m_Bits.Is<KG3DSSETest::SET_ALPHA>());

	KG3DSceneEntity TempSE(SCENEENTITY_TEST, ILLEGAL_PTR);
	TempSE.SetAlpha(1);
	TempSE.FrameMove();

	//现在两个方法应该被用过
	_ASSERTE(SEETest.m_Bits.Is<KG3DSSETest::FRAME_MOVE>());
	_ASSERTE(SEETest.m_Bits.Is<KG3DSSETest::SET_ALPHA>());

	//反注册之后，应该没有了
	/*bRet = g_KG3DSceneEntityExecutorFactory.UnRegisterExecutor(SCENEENTITY_TEST);
	_ASSERTE(bRet);
	KG3DSceneEntityExecutor& SEE2 = g_KG3DSceneEntityExecutorFactory.GetExecutor(SCENEENTITY_TEST);
	_ASSERTE(SEE2.GetType() == SCENEENTITY_NONE);*/

	//测试Selectable
	{
		{
			KG3DSceneEntity SEObj(SCENEENTITY_OBJECT, ILLEGAL_PTR);
			_ASSERTE(SEObj.GetType() == SCENEENTITY_OBJECT);
			_ASSERTE(! SEObj.IsSelectable());
			SEObj.SetSelectable(TRUE);
			_ASSERTE(SEObj.IsSelectable());
			SEObj.SetSelectable(FALSE);
			_ASSERTE(! SEObj.IsSelectable());
		}

		{
			KG3DSceneEntity SEObj(SCENEENTITY_OBJECT, ILLEGAL_PTR, reinterpret_cast<LPVOID>(0x00000001));
			_ASSERTE(SEObj.GetType() == SCENEENTITY_OBJECT);
			_ASSERTE(! SEObj.IsSelectable());
		}

		{
			KG3DSceneEntity SEObj(SCENEENTITY_OBJECT, ILLEGAL_PTR, reinterpret_cast<LPVOID>(0x00000001));
			KG3DSceneEntity SEObjCopy = SEObj;
			_ASSERTE(! SEObjCopy.IsSelectable());
			SEObjCopy.SetSelectable(TRUE);
			KG3DSceneEntity SEObjCopy2 = SEObjCopy;
			_ASSERTE(SEObjCopy2.IsSelectable());
		}
	}

	//测试GetType
	{
		KG3DSceneEntity SEObj(SCENEENTITY_OBJECT, ILLEGAL_PTR);
		_ASSERTE(SEObj.GetType() == SCENEENTITY_OBJECT);
		KG3DSceneEntity SEModel(SCENEENTITY_MODEL, ILLEGAL_PTR);
		_ASSERTE(SEModel.GetType() == SCENEENTITY_MODEL);
		KG3DSceneEntity SEEG(SCENEENTITY_GROUP, ILLEGAL_PTR);
		_ASSERTE(SEEG.GetType() == SCENEENTITY_GROUP);
		KG3DSceneEntity SESFXMeteor(SCENEENTITY_SFX_METEOR, ILLEGAL_PTR);
		_ASSERTE(SESFXMeteor.GetType() == SCENEENTITY_SFX_METEOR);
		KG3DSceneEntity SESFXLauncher(SCENEENTITY_SFX_LAUNCHER, ILLEGAL_PTR);
		_ASSERTE(SESFXLauncher.GetType() == SCENEENTITY_SFX_LAUNCHER);
		KG3DSceneEntity SETrack(SCENEENTITY_SFX_TRACK, ILLEGAL_PTR);
		_ASSERTE(SETrack.GetType() == SCENEENTITY_SFX_TRACK);
		KG3DSceneEntity SEBillBoard(SCENEENTITY_SFX_BILLBOARD, ILLEGAL_PTR);
		_ASSERTE(SEBillBoard.GetType() == SCENEENTITY_SFX_BILLBOARD);
		KG3DSceneEntity SEPosition(SCENEENTITY_POSITION, ILLEGAL_PTR);
		_ASSERTE(SEPosition.GetType() == SCENEENTITY_POSITION);
		KG3DSceneEntity SELeaf(SCENEENTITY_SPEEDTREELEAF, ILLEGAL_PTR);
		_ASSERTE(SELeaf.GetType() == SCENEENTITY_SPEEDTREELEAF);
		KG3DSceneEntity SELeafGroup(SCENEENTITY_SPEEDTREELEAFGROUP, ILLEGAL_PTR);
		_ASSERTE(SELeafGroup.GetType() == SCENEENTITY_SPEEDTREELEAFGROUP);
		KG3DSceneEntity SETerrainCP(SCENEENTITY_TERRAINCONTROLPOINT, ILLEGAL_PTR);
		_ASSERTE(SETerrainCP.GetType() == SCENEENTITY_TERRAINCONTROLPOINT);

	}
	//检查复制函数
	{
		KG3DSceneEntity Src(SCENEENTITY_OBJECT, ILLEGAL_PTR);
		KG3DSceneEntity Desc(Src);
		_ASSERTE(0 == memcmp(&Src, &Desc, sizeof(KG3DSceneEntity)));

		KG3DSceneEntity Desc2;
		Desc2 = Src;
		_ASSERTE(0 == memcmp(&Src, &Desc2, sizeof(KG3DSceneEntity)));
	}
	//_KG3D_DEBUG_REPORT("KG3DSceneEntityExecutorFactory Runtime Test Passed\n");
}
KG_TEST_END(KG3DSceneEntity)
#endif

////////////////////////////////////////////////////////////////////////////////
KG3DSceneEntityDummy	g_cKG3DSceneEntityDummy;


enum
{
	FRAME_QUALITY_SPEED		=	8,
	FRAME_QUALITY_LOW		=	2,
	FRAME_QUALITY_HIGH		=	0,
	FRAME_QUALITY_NORMAL	=	1,

};
KG3DSceneEntity::KG3DSceneEntity(void)
: m_dwSelectableAndType( SCENEENTITY_NONE )
, m_pPointer( NULL )
, m_pRefData( NULL )
, m_pExecutor(&g_KG3DSceneEntityExecutorFactory.GetExecutor(SCENEENTITY_NONE))

{
	//m_dwFrameTimes = 0;
	//m_dwFrameQuality = 0;
}

KG3DSceneEntity::KG3DSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer)
: m_dwSelectableAndType( EntityType )
, m_pPointer( EntityPointer )
, m_pRefData( NULL )
, m_pExecutor(&g_KG3DSceneEntityExecutorFactory.GetExecutor(EntityType))

{
	//m_dwFrameTimes = 0;
	//m_dwFrameQuality = 0;

	_ASSERTE(EntityType != SCENEENTITY_DUMMY);
	_ASSERTE(NULL != m_pPointer);	//下面的很多函数根本就没有判断Pointer，所以构建的时候就应该默认是有值的
	//当然也可以每个函数对m_pPointer判空，但是这样比在构造函数中判空麻烦多了。
}

KG3DSceneEntity::KG3DSceneEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer,PVOID Data)
: m_dwSelectableAndType( EntityType )
, m_pPointer( EntityPointer )
, m_pRefData( Data )
, m_pExecutor(&g_KG3DSceneEntityExecutorFactory.GetExecutor(EntityType))
{
	//m_dwFrameTimes = 0;
	//m_dwFrameQuality = 0;
	_ASSERTE (EntityType != SCENEENTITY_DUMMY);
	_ASSERTE(NULL != m_pPointer);	//下面的很多函数根本就没有判断Pointer，所以构建的时候就应该默认是有值的
	//当然也可以每个函数对m_pPointer判空，但是这样比在构造函数中判空麻烦多了

}

KG3DSceneEntity::KG3DSceneEntity( const KG3DSceneEntity& Other )
	:m_dwSelectableAndType(Other.m_dwSelectableAndType)
{
	const KG3DSceneEntityExecutor* pExe = Other.m_pExecutor;
	_ASSERTE(NULL != pExe);
	pExe->CopyEntity(Other, *this);

	_ASSERTE(NULL != m_pExecutor);
	_ASSERTE(NULL != m_pPointer);	//下面的很多函数根本就没有判断Pointer，所以构建的时候就应该默认是有值的
	//当然也可以每个函数对m_pPointer判空，但是这样比在构造函数中判空麻烦多了
}
KG3DSceneEntity& KG3DSceneEntity::operator=(const KG3DSceneEntity& Other)
{
	const KG3DSceneEntityExecutor* pExe = Other.m_pExecutor;
	_ASSERTE(NULL != pExe);
	pExe->CopyEntity(Other, *this);

	_ASSERTE(NULL != m_pPointer);	//下面的很多函数根本就没有判断Pointer，所以构建的时候就应该默认是有值的
	//当然也可以每个函数对m_pPointer判空，但是这样比在构造函数中判空麻烦多了
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneEntity::GetType(SCENEENTITYTYPE *peRetType)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(peRetType);

	*peRetType = this->GetType();
	hResult = S_OK;
Exit0:
	return hResult;
}

SCENEENTITYTYPE KG3DSceneEntity::GetType()const
{
	return static_cast<SCENEENTITYTYPE>(m_dwSelectableAndType & em_bit_mask_type);
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneEntity::GetPointer(VOID **ppvRetPointer)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(ppvRetPointer);
	{
		*ppvRetPointer = m_pPointer;
	}
	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneEntity::GetRefData(VOID **ppvRetData)
{
	HRESULT hResult  = E_FAIL;

	KGLOG_PROCESS_ERROR(ppvRetData);

	*ppvRetData = m_pRefData;

	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

//////////////////////////////////////////////////////////////////////////
void KG3DSceneEntity::SetAlpha(float fAlpha)
{
	return m_pExecutor->SetAlpha(*this, fAlpha);
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneEntity::FrameMove()
{
	return m_pExecutor->FrameMove(*this);	
}
HRESULT KG3DSceneEntity::RenderOption(DWORD dwOption)
{
	return m_pExecutor->RenderOption(*this, dwOption);
}

HRESULT KG3DSceneEntity::RenderSelectMark(DWORD dwColor)
{
	return m_pExecutor->RenderSelectMark(*this, dwColor);
}

HRESULT KG3DSceneEntity::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
{
	return m_pExecutor->RenderReflect(*this, fWaterHeight, WaterPlane, WaterPlaneFX);
}

HRESULT KG3DSceneEntity::RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP)
{
	return m_pExecutor->RenderShadowMap(*this, lpEffect, matVP, matWVP);
}

HRESULT KG3DSceneEntity::RenderPlaneShadow(D3DXPLANE& plane,const D3DXVECTOR4& LightDir)
{
	return m_pExecutor->RenderPlaneShadow(*this, plane, LightDir);
}
BOOL KG3DSceneEntity::IsSecondUVExist()
{
	return m_pExecutor->IsSecondUVExist(*this);
}

HRESULT KG3DSceneEntity::GetName(LPTSTR pName, size_t BufferSize)
{
	return m_pExecutor->GetName(*this, pName, BufferSize);
}

HRESULT KG3DSceneEntity::Reload()
{
	return m_pExecutor->Reload(*this);
}

HRESULT KG3DSceneEntity::Clone(KG3DSceneEntity *pEntity, float fX, float fY, float fZ)
{
	return m_pExecutor->Clone(*this, pEntity, fX, fY, fZ);
}

HRESULT KG3DSceneEntity::Render()
{
	return RenderOption(0);
}
//
//HRESULT KG3DSceneEntity::GetBBoxOld(AABBOX* pOutBox)
//{
//	return m_pExecutor->GetBBoxOld(*this, pOutBox);
//}

HRESULT KG3DSceneEntity::GetBBox(AABBOX* pOutBox)//检查物体是否被选择中
{
	return m_pExecutor->GetBBox(*this, pOutBox);
}



void KG3DSceneEntity::SetLocalMatrix(const D3DXMATRIX& mat)
{
	return m_pExecutor->SetLocalMatrix(*this, mat);
}

HRESULT KG3DSceneEntity::SetTranslation(D3DXVECTOR3* pValue)
{
	return m_pExecutor->SetTranslation(*this, pValue);  
}

HRESULT KG3DSceneEntity::SetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)
{
	return m_pExecutor->SetRotation(*this, pValue, pValueXYZ);
}
void KG3DSceneEntity::SetRotationCenter(D3DXVECTOR3* pValue)
{
	return m_pExecutor->SetRotationCenter(*this, pValue);
}

void KG3DSceneEntity::SetScaleSize(float Value)
{
	m_pExecutor->SetScaleSize(*this, Value);
}

void KG3DSceneEntity::GetScaleSize(float* pValue)
{
	m_pExecutor->GetScaleSize(*this, pValue);
}

HRESULT KG3DSceneEntity::SetScaling(D3DXVECTOR3* pValue)
{
	return m_pExecutor->SetScaling(*this, pValue);
}
void KG3DSceneEntity::SetScalingRotation(D3DXQUATERNION* pValue)
{
	return m_pExecutor->SetScalingRotation(*this, pValue);
}
void KG3DSceneEntity::SetScalingCenter(D3DXVECTOR3* pValue)
{
	m_pExecutor->SetScalingCenter(*this, pValue);
}


HRESULT KG3DSceneEntity::RotationZ(FLOAT fAngelZ)
{
	D3DXQUATERNION	qOld;
	D3DXVECTOR3		vRotOld;
	D3DXQUATERNION	qToRotate;
	HRESULT hr =  this->GetRotation(&qOld, &vRotOld);
	KG_COM_PROCESS_ERROR(hr);
	D3DXQuaternionRotationYawPitchRoll(&qToRotate, 0.0f, 0.0f, fAngelZ);
	qOld *= qToRotate;
	vRotOld.z += fAngelZ;
	this->SetRotation(&qOld, &vRotOld);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneEntity::RotationX(FLOAT fAngelX)
{
	D3DXQUATERNION	qOld;
	D3DXVECTOR3		vRotOld;
	D3DXQUATERNION	qToRotate;
	HRESULT hr =  this->GetRotation(&qOld, &vRotOld);
	KG_COM_PROCESS_ERROR(hr);
	D3DXQuaternionRotationYawPitchRoll(&qToRotate, 0.0f, fAngelX, 0.0f);
	qOld *= qToRotate;
	vRotOld.x += fAngelX;
	this->SetRotation(&qOld, &vRotOld);
	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KG3DSceneEntity::RotationY(FLOAT fAngelY)
{
	D3DXQUATERNION	qOld;
	D3DXVECTOR3		vRotOld;
	D3DXQUATERNION	qToRotate;
	HRESULT hr =  this->GetRotation(&qOld, &vRotOld);
	KG_COM_PROCESS_ERROR(hr);
	D3DXQuaternionRotationYawPitchRoll(&qToRotate, fAngelY, 0.0f, 0.0f);
	qOld *= qToRotate;
	vRotOld.y += fAngelY;
	this->SetRotation(&qOld, &vRotOld);
	return S_OK;
Exit0:
	return E_FAIL;

}


D3DXMATRIX KG3DSceneEntity::GetWorldMatrix()
{
	return m_pExecutor->GetWorldMatrix(*this);
}


D3DXMATRIX KG3DSceneEntity::GetMatrix()
{
	return m_pExecutor->GetMatrix(*this);
}
HRESULT KG3DSceneEntity::GetTranslation(D3DXVECTOR3* pValue)
{
	return m_pExecutor->GetTranslation(*this, pValue);
}

HRESULT KG3DSceneEntity::GetRotation(D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)
{
	return m_pExecutor->GetRotation(*this, pValue, pValueXYZ);
}
void KG3DSceneEntity::GetRotationCenter(D3DXVECTOR3* pValue)
{
	return m_pExecutor->GetRotationCenter(*this, pValue);
}
HRESULT KG3DSceneEntity::GetScaling(D3DXVECTOR3* pValue)
{
	return m_pExecutor->GetScaling(*this, pValue);
}

HRESULT KG3DSceneEntity::OnDelete() const
{
	return m_pExecutor->OnDelete(*this);
}

HRESULT KG3DSceneEntity::ChangeModel(INT nChangeCode)
{
	return m_pExecutor->ChangeModel(*this, nChangeCode);
}
//)
HRESULT KG3DSceneEntity::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return m_pExecutor->ProcessMessage(*this, message, wParam, lParam);
}

void KG3DSceneEntity::GetScalingRotation(D3DXQUATERNION* pValue)
{
	return m_pExecutor->GetScalingRotation(*this, pValue);
}
void KG3DSceneEntity::GetScalingCenter(D3DXVECTOR3* pValue)
{
	m_pExecutor->GetScalingCenter(*this, pValue);
}

HRESULT KG3DSceneEntity::UpdateTransformation()
{
	return m_pExecutor->UpdateTransformation(*this);
}

BOOL KG3DSceneEntity::IsVertexInside(D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir)
{
	return m_pExecutor->IsVertexInside(*this, vVertex, vDir);
}


BOOL KG3DSceneEntity::GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[],
												IN CONST D3DXVECTOR3 vDst[], 
												IN CONST INT         nNum,
												OUT      FLOAT*      pDis 
												)
{
	return m_pExecutor->GetCameraNearRayIntersect(*this, vSrc, vDst, nNum, pDis);
}


BOOL KG3DSceneEntity::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction, BOOL bFiltrate)
{
	return m_pExecutor->GetRayIntersect(*this, dwCount, pInterPoint,dwMaxNum, Origin, Direction, bFiltrate);
}

//BOOL KG3DSceneEntity::GetRayIntersectPointAndDir(DWORD& dwCount,
//												 D3DXVECTOR3* pInterPoint,
//												 float* pAngle,
//												 const D3DXVECTOR3& vec3Org,
//												 const D3DXVECTOR3& vec3Dir)
//{
//	return m_pExecutor->GetRayIntersectPointAndDir(*this, dwCount, pInterPoint, pAngle, vec3Org, vec3Dir);
//}

//BOOL KG3DSceneEntity::IsRayIntersect(D3DXVECTOR3&Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /* = NULL  */)
//{
//	return m_pExecutor->IsRayIntersect(*this, Inter, Origin, Direction, vecDarkEntitys);
//}

BOOL KG3DSceneEntity::RayIntersectionForEntity(const D3DXVECTOR3& vSrc
												  , const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet)
{
	return m_pExecutor->RayIntersectionForEntity(*this, vSrc, vNormalizeDir, pfRet);
}

HRESULT KG3DSceneEntity::SetLOD(float fLod)
{
	return m_pExecutor->SetLOD(*this, fLod);
}

void KG3DSceneEntity::GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID)
{
	return m_pExecutor->GetProfile(*this, dwNumFacesTotal, dwNumVertices, dwNumTextures, dwNumID);
}

DWORD KG3DSceneEntity::GetSceneLayerID()
{
	return m_pExecutor->GetSceneLayerID(*this);
}

void  KG3DSceneEntity::SetSceneLayerID(DWORD dwID)
{
	m_pExecutor->SetSceneLayerID(*this, dwID);
}

HRESULT KG3DSceneEntity::Compare(IEKG3DSceneEntity *piEntity, BOOL *pbRetEqualFlag)
{
	HRESULT hResult  = E_FAIL;
	KG3DSceneEntity *pEntity = NULL;

	KGLOG_PROCESS_ERROR(piEntity);
	KGLOG_PROCESS_ERROR(pbRetEqualFlag);

	pEntity = dynamic_cast<KG3DSceneEntity *>(piEntity);
	KGLOG_PROCESS_ERROR(pEntity);

	*pbRetEqualFlag = (*pEntity == *this);
	//Exit1:
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSceneEntity::OnSelect()
{
	return m_pExecutor->OnSelect(*this);
}

HRESULT KG3DSceneEntity::OnUnSelect()
{
	return m_pExecutor->OnUnSelect(*this);
}

VOID KG3DSceneEntity::SetSelectable( BOOL bSelectable )
{
	if (bSelectable)
	{
		m_dwSelectableAndType |= em_bit_mask_selectable;
	}
	else
	{
		m_dwSelectableAndType &= (~em_bit_mask_selectable);
	}
}

BOOL KG3DSceneEntity::IsSelectable()
{
	return m_dwSelectableAndType & em_bit_mask_selectable;
}

KG3DSceneEntityExecutorFactory::KG3DSceneEntityExecutorFactory()
{
	this->RegisterExecutor(KG3DSEEPvsPortal());
	//this->RegisterExecutor(KG3DSEEDummy());
	this->RegisterExecutor(KG3DSEEObject());
	this->RegisterExecutor(KG3DSEEModel());
	this->RegisterExecutor(KG3DSEEGroup());
	this->RegisterExecutor(KG3DSEESFXMeteor());
	this->RegisterExecutor(KG3DSEESFXLauncher());
	this->RegisterExecutor(KG3DSEESFXTrack());
	this->RegisterExecutor(KG3DSEESFXBillboard());
	this->RegisterExecutor(KG3DSEEPosition());
	this->RegisterExecutor(KG3DSEESpeedTreeLeaf());
	this->RegisterExecutor(KG3DSEESpeedTreeLeafGroup());
	this->RegisterExecutor(KG3DSSETest());
    this->RegisterExecutor(KG3DSEESFXBoxProjection());
}

KG3DSceneEntityExecutorFactory::~KG3DSceneEntityExecutorFactory()
{
	KG_CUSTOM_HELPERS::TContainerDelete(m_Map);
}

bool KG3DSceneEntityExecutorFactory::RegisterExecutor( const KG3DSceneEntityExecutor& Exc )
{
	KG3DSceneEntityExecutor* pSEE = Exc.Clone();
	if (! pSEE)
	{
		return false;
	}
	TypeMap::_Pairib PIB = m_Map.insert(TypeMap::value_type(Exc.GetType(), pSEE));
	if (! PIB.second)
	{
		SAFE_DELETE(pSEE);
	}
	return PIB.second;
}

bool KG3DSceneEntityExecutorFactory::UnRegisterExecutor(DWORD dwType)
{
	TypeMap::iterator it = m_Map.find(dwType);
	KG_PROCESS_ERROR(it != m_Map.end());
	{
		delete it->second;
		m_Map.erase(it);
		return true;
	}
Exit0:
	return false;
}

KG3DSceneEntityExecutor& KG3DSceneEntityExecutorFactory::GetExecutor( DWORD dwType )
{
	//这两个类在构造函数中就注册了
	static KG3DSceneEntityExecutor& SEEObj= *((m_Map.find(SCENEENTITY_OBJECT))->second);
	static KG3DSceneEntityExecutor& SEEModel = *((m_Map.find(SCENEENTITY_MODEL))->second);

	//这两种Entity用的最多，做一下加速
	switch(dwType)
	{
	case SCENEENTITY_OBJECT:
		return SEEObj;
		break;
	case SCENEENTITY_MODEL:
		return SEEModel;
		break;
	default:
		break;
	}

	TypeMap::iterator it = m_Map.find(dwType);
	if(it != m_Map.end())
		return *(it->second);

	static KG3DSEE_None NoneExecutor;//什么也不做的Executor
	return NoneExecutor;
}


D3DXMATRIX KG3DSceneEntityExecutor::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	return mIdentity;
}

D3DXMATRIX KG3DSceneEntityExecutor::GetMatrix( KG3DSceneEntity& Entity )
{
	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	return mIdentity;
}

HRESULT KG3DSceneEntityExecutor::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _tcscpy_s(pName, BufferSize, _T("场景实体对象"));
	return nRet == 0 ? S_OK : E_FAIL;
}

BOOL KG3DSceneEntityExecutor::RayIntersectionForEntity( KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet )
{
	AABBOX boxTemp;
	if (FAILED(GetBBox(Entity, &boxTemp)))
		return FALSE;

	return boxTemp.RayIntersection(vSrc, vNormalizeDir, pfRet, NULL);
}
//////////////////////////////////////////////////////////////////////////

KG3DSceneEntityExecutor* KG3DSEEPvsPortal::Clone() const
{
	return new KG3DSEEPvsPortal();
}

HRESULT KG3DSEEPvsPortal::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->Render();
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->RenderSelectMark();
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	static int sPortalIndex = 0;
	wsprintf(pName, "%d-%d", "PvsPortal", sPortalIndex++);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetBBox(pOutBox);
	return S_OK;
}

HRESULT KG3DSEEPvsPortal::OnSelect( KG3DSceneEntity& Entity )
{
	return S_OK;
}

D3DXMATRIX KG3DSEEPvsPortal::GetWorldMatrix(KG3DSceneEntity& Entity)
{
	return static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetWorldMatrix();
}

D3DXMATRIX KG3DSEEPvsPortal::GetMatrix(KG3DSceneEntity& Entity)
{
	return static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->GetMatrix();
}

void KG3DSEEPvsPortal::SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat)
{
	return static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}
//在基类中就实现了，用BBOX的结果作为碰撞结果
//BOOL KG3DSEEPvsPortal::IsRayIntersect(KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin, 
//									  D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /* = NULL */)
//{
//	AABBOX Box;
//
//	if (FAILED(this->GetBBox(Entity, &Box)))
//		return FALSE;
//
//	float fDistanceSq = PointDistanceLineSq(&Box.GetCenter(), &Origin, &(Origin + Direction));
//	float fRadiusSq   = D3DXVec3LengthSq(&(Box.A - Box.GetCenter()));
//
//	if (fDistanceSq > fRadiusSq)
//		return FALSE;
//
//    KG3DPvsPortal* ptl = static_cast<KG3DPvsPortal*>(Entity.m_pPointer);
//
//    D3DXVECTOR3 tri1[] = {
//        ptl->m_vWorldVector[0],
//        ptl->m_vWorldVector[1],
//        ptl->m_vWorldVector[2]
//    };
//
//    D3DXVECTOR3 tri2[] = {
//        ptl->m_vWorldVector[0],
//        ptl->m_vWorldVector[2],
//        ptl->m_vWorldVector[3]
//    };
//
//    float fDist = 0.f;
//
//    if (!D3DXIntersectTri(tri1, tri1 + 1, tri1 + 2, &Origin, &Direction, NULL, NULL, &fDist))
//        if (!D3DXIntersectTri(tri2, tri2 + 1, tri2 + 2, &Origin, &Direction, NULL, NULL, &fDist))
//            return FALSE;
//
//    Inter = Origin + Direction * fDist;
//
//	return TRUE;
//}

HRESULT KG3DSEEPvsPortal::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DPvsPortal*>(Entity.m_pPointer)->FrameMove();
}

KG3DSceneEntityExecutor* KG3DSEEObject::Clone() const
{
	return new KG3DSEEObject;
}

VOID KG3DSEEObject::SetAlpha( KG3DSceneEntity& Entity, float fAlpha )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetAlpha(fAlpha);
}

HRESULT KG3DSEEObject::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->FrameMove();
}
HRESULT KG3DSEEObject::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->Render(dwOption);
}

HRESULT KG3DSEEObject::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	HRESULT hr = S_OK;
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	if (pObject->GetType() == REPRESENTOBJECT_SET)
	{
		hr = pObject->RenderSelectMark(0xffffff00);
	}
	else
	{
		hr = pObject->RenderSelectMark(dwColor);
	}
	return hr;
}

HRESULT KG3DSEEObject::RenderPlaneShadow( KG3DSceneEntity& Entity, D3DXPLANE& plane,const D3DXVECTOR4& LightDir )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->RenderPlaneShadow(plane, LightDir);
}

HRESULT KG3DSEEObject::RenderShadowMap( KG3DSceneEntity& Entity, LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP )
{
	HRESULT hr = S_OK;
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	if ((pObject->GetType() == REPRESENTOBJECT_DEFAULT)||(pObject->GetType() == REPRESENTOBJECT_SET))
	{
		hr = pObject->RenderShadowMap(lpEffect,matVP,matWVP);
	}
	return hr;
}
HRESULT KG3DSEEObject::RenderReflect( KG3DSceneEntity& Entity, float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->RenderReflect(fWaterHeight, WaterPlane, WaterPlaneFX);
}

HRESULT KG3DSEEObject::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEObject::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

VOID KG3DSEEObject::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEEObject::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

VOID KG3DSEEObject::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

VOID KG3DSEEObject::SetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetScalingCenter(pValue);
}

HRESULT KG3DSEEObject::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEObject::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

VOID KG3DSEEObject::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetRotationCenter(pValue);
}

HRESULT KG3DSEEObject::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

VOID KG3DSEEObject::GetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetScalingRotation(pValue);
}

VOID KG3DSEEObject::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

D3DXMATRIX KG3DSEEObject::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetMatrixWorld();
}

D3DXMATRIX KG3DSEEObject::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->m_Matrix;
}

HRESULT KG3DSEEObject::UpdateTransformation( KG3DSceneEntity& Entity )
{
	KG3DRepresentObject* p = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	p->UpdateTransformation();
	return S_OK;
}

VOID KG3DSEEObject::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

VOID KG3DSEEObject::SetScaleSize( KG3DSceneEntity& Entity, FLOAT Value )
{
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

	DWORD dwModelCnt = pObject->GetModelCount();
	for (DWORD i = 0; i < dwModelCnt; ++i)
	{
		KG3DModel *pModel = NULL;
		pObject->GetModel(pModel, i,FALSE);
		if (pModel && pModel->GetType() == MESHTYPE_SFX)
		{
			KG3DSFX* psfx = (KG3DSFX*)pModel;
			psfx->SetScaleSize(Value);
		}
	}
	pObject->ComputeBBox();
}

VOID KG3DSEEObject::GetScaleSize( KG3DSceneEntity& Entity, FLOAT* pValue )
{
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);

	DWORD dwModelCnt = pObject->GetModelCount();
	for (DWORD i = 0; i < dwModelCnt; ++i)
	{
		KG3DModel *pModel = NULL;
		pObject->GetModel(pModel, i,FALSE);
		if (pModel && pModel->GetType() == MESHTYPE_SFX)
		{
			KG3DSFX* psfx = (KG3DSFX*)pModel;
			*pValue = psfx->GetScaleSize();
			break;
		}
	}
}

HRESULT KG3DSEEObject::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetName(pName, BufferSize);
}

HRESULT KG3DSEEObject::SetLOD( KG3DSceneEntity& Entity, FLOAT fLOD )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetLOD(fLOD);
}

BOOL KG3DSEEObject::IsSecondUVExist( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->IsSecondUVExist();
}

VOID KG3DSEEObject::GetProfile(KG3DSceneEntity& Entity, DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID )
{
	static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetProfile(dwNumFacesTotal, dwNumVertices, dwNumTextures, dwNumID);
}

DWORD KG3DSEEObject::GetSceneLayerID( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->GetSceneLayerID();
}

VOID KG3DSEEObject::SetSceneLayerID( KG3DSceneEntity& Entity, DWORD ID )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->SetSceneLayerID(ID);
}

//HRESULT KG3DSEEObject::GetBBoxOld( KG3DSceneEntity& Entity, AABBOX* pOutBox )
//{
//	KG3DRepresentObject* p = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
//	p->ComputeBBox();
//	(*pOutBox) = p->m_BBox;
//	return S_OK;
//}

HRESULT KG3DSEEObject::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	KG3DRepresentObject* p = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	p->ComputeDynamicBBox();
	(*pOutBox) = p->m_BBox;
	return S_OK;
}

//BOOL KG3DSEEObject::IsRayIntersect( KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /*= NULL*/ )
//{
//	AABBOX Box;
//	if (FAILED(this->GetBBox(Entity, &Box)))
//		return FALSE;
//
//	//这部分模糊选择的功能写在PointSelection中就行了
//	/*float fDiameterScaleFactor = 0.5F;
//
//	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
//	int nCount = 0;
//	pObject->GetModelCount(&nCount);
//
//    bool bPointLight = false;
//
//	for (int i = 0; i < nCount; i++)
//	{
//		KG3DModel* pModel = NULL;
//		pObject->GetModel(pModel, (DWORD)i);
//
//		if (!pModel)
//			continue;
//		DWORD dwType = 0;
//		pModel->GetType(&dwType);
//
//		if (dwType == MESHTYPE_MODELST)
//		{
//			fDiameterScaleFactor = 1.f;
//			break;
//		}
//		else if (dwType == MESHTYPE_DEFAULT)
//		{
//			if(!pModel->IsLoaded())
//				return FALSE;
//			ASSERT(((KG3DMesh*)pModel->GetMesh()));
//			if (
//				pModel->GetMesh()->m_pSkinInfo 
//				&& 
//				pModel->GetMesh()->m_ppMeshes[KG3DMesh::SMBT_ACTIVE]
//			)
//			{
//				fDiameterScaleFactor = 1.f;
//				break;
//			}
//		}
//        else if (dwType == MESHTYPE_POINTLIGHT)
//        {
//            bPointLight = true;
//            break;
//        }
//	}
//
//	float fDistance = PointDistanceLine(&Box.GetCenter(),&Origin,&(Origin+Direction));
//	float fBoxDiameter = Box.GetDiameter();
//
//    if (bPointLight)
//        fBoxDiameter = 500.f;
//
//	float R = fBoxDiameter * fDiameterScaleFactor;
//
//	if (fDistance > R)
//		return FALSE;
//
//	if (g_bClient)
//	{
//		if (vecDarkEntitys && fDistance < fBoxDiameter * 0.58f)
//			vecDarkEntitys->push_back(Entity);
//	}*/
//
//	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
//
//	return pObject->IsRayIntersect(Inter, Origin, Direction);
//}

BOOL KG3DSEEObject::RayIntersectionForEntity(KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc
												  , const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet)
{
	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	return pObject->RayIntersection(vSrc, vNormalizeDir, pfRet, FALSE, NULL);
}

BOOL KG3DSEEObject::GetCameraNearRayIntersect( KG3DSceneEntity& Entity, IN CONST D3DXVECTOR3 vSrc[] , IN CONST D3DXVECTOR3 vDst[],IN CONST INT nNum, OUT FLOAT* pDis )
{
	AABBOX Box;

	if(FAILED(this->GetBBox(Entity, &Box)))
		return FALSE;

	D3DXVECTOR3 vBoxCenter = Box.GetCenter();

	float fDisSq = PointDistanceLineSq(&vBoxCenter, &vSrc[0], &vDst[0]);
	float RSq    = D3DXVec3LengthSq(&(vBoxCenter - Box.A));      

	if(fDisSq > RSq)
		return FALSE;

	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	return pObject->GetCameraNearRayIntersect(vSrc, vDst, nNum, pDis);
}

BOOL KG3DSEEObject::IsVertexInside( KG3DSceneEntity& Entity, D3DXVECTOR3& vVertex, D3DXVECTOR3& vDir )
{
	AABBOX Box;

	if(FAILED(this->GetBBox(Entity, &Box)))
		return FALSE;

	D3DXVECTOR3 vBoxCenter = Box.GetCenter();

	float fDisSq = PointDistanceLineSq(&vBoxCenter,&vVertex, &(vVertex + D3DXVECTOR3(0.f, 0.f, 0.f)));
	float RSq    = D3DXVec3LengthSq(&(vBoxCenter - Box.A));      

	if(fDisSq > RSq)
		return FALSE;

	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	return pObject->IsVertexInside(vVertex, vDir);
}

BOOL KG3DSEEObject::GetRayIntersect( KG3DSceneEntity& Entity, DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin ,D3DXVECTOR3 Direction, BOOL bFiltrate /*= FALSE*/ )
{
	AABBOX Box;

	if(FAILED(this->GetBBox(Entity, &Box)))
		return FALSE;

	D3DXVECTOR3 vBoxCenter = Box.GetCenter();

	float fDisSq = PointDistanceLineSq(&vBoxCenter,&Origin,&(Origin + Direction));
	float RSq    = D3DXVec3LengthSq(&(vBoxCenter - Box.A));      

	if(fDisSq > RSq)
		return FALSE;


	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
	return pObject->GetRayIntersect(dwCount,pInterPoint,dwMaxNum,Origin,Direction, bFiltrate);

}

//BOOL KG3DSEEObject::GetRayIntersectPointAndDir( KG3DSceneEntity& Entity, DWORD& dwCount, D3DXVECTOR3* pInterPoint , float* pAngle, const D3DXVECTOR3& vec3Org, const D3DXVECTOR3& vec3Dir )
//{
//	BOOL bReturn = FALSE;
//	HRESULT hr = E_FAIL;
//	AABBOX Box;
//	float fDistance = 0.0f;
//	float R = 0.0f;
//
//	hr = this->GetBBox(Entity, &Box);
//	KG_COM_PROCESS_ERROR(hr);
//
//	fDistance = PointDistanceLine(&Box.GetCenter(),
//		&vec3Org,
//		&(vec3Org + vec3Dir));
//
//	R = D3DXVec3Length(&(Box.A - Box.B)) * 0.5f;
//	KG_PROCESS_ERROR(fDistance <= R);
//
//	KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
//	bReturn = pObject->GetRayIntersectPointAndDir(dwCount,
//		pInterPoint,
//		pAngle,
//		vec3Org,
//		vec3Dir);
//
//Exit0:
//	return bReturn;
//}
HRESULT KG3DSEEObject::OnSelect( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->OnSelect();
}

HRESULT KG3DSEEObject::OnUnSelect(KG3DSceneEntity& Entity)
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->OnUnSelect();
}

HRESULT KG3DSEEObject::ProcessMessage( KG3DSceneEntity& Entity, UINT message, WPARAM wParam, LPARAM lParam )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->ProcessMessage(message,wParam,lParam);
}

HRESULT KG3DSEEObject::Reload( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->Reload();
}

HRESULT KG3DSEEObject::ChangeModel(KG3DSceneEntity& Entity, INT nChangeCode )
{
	return static_cast<KG3DRepresentObject*>(Entity.m_pPointer)->ChangeModel(nChangeCode);
}
//SSEModel////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEEModel::Clone() const
{
	return new KG3DSEEModel;
}

VOID KG3DSEEModel::SetAlpha( KG3DSceneEntity& Entity, float fAlpha )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->SetAlpha(fAlpha, FALSE);
}

HRESULT KG3DSEEModel::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->FrameMove();
}
HRESULT KG3DSEEModel::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->Render(dwOption, NULL);
}

HRESULT KG3DSEEModel::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->RenderSelectMark(dwColor);
}

HRESULT KG3DSEEModel::RenderPlaneShadow( KG3DSceneEntity& Entity, D3DXPLANE& plane,const D3DXVECTOR4& LightDir )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->RenderPlaneShadow(plane, LightDir);
}

HRESULT KG3DSEEModel::RenderShadowMap( KG3DSceneEntity& Entity, LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->RenderShadowMap(lpEffect,matVP,matWVP);
}

HRESULT KG3DSEEModel::RenderReflect( KG3DSceneEntity& Entity, float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->RenderReflect(fWaterHeight,WaterPlane,WaterPlaneFX);
}

HRESULT KG3DSEEModel::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEModel::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

VOID KG3DSEEModel::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEEModel::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

VOID KG3DSEEModel::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

VOID KG3DSEEModel::SetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->SetScalingCenter(pValue);
}

HRESULT KG3DSEEModel::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEModel::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

VOID KG3DSEEModel::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->GetRotationCenter(pValue);
}

HRESULT KG3DSEEModel::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

VOID KG3DSEEModel::GetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->GetScalingRotation(pValue);
}

VOID KG3DSEEModel::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

D3DXMATRIX KG3DSEEModel::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->m_matWorld;
}

D3DXMATRIX KG3DSEEModel::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->m_Matrix;
}

HRESULT KG3DSEEModel::UpdateTransformation( KG3DSceneEntity& Entity )
{
	KG3DModel* p = static_cast<KG3DModel*>(Entity.m_pPointer);
	p->UpdateTransformation();
	p->ComputeBBox();
	return S_OK;
}

VOID KG3DSEEModel::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

VOID KG3DSEEModel::SetScaleSize( KG3DSceneEntity& Entity, FLOAT Value )
{
	KG3DModel *pModel = static_cast<KG3DModel*>(Entity.m_pPointer);
	if (pModel->GetType() == MESHTYPE_SFX)
	{
		KG3DSFX *psfx = (KG3DSFX*)pModel;
		psfx->SetScaleSize(Value);
	}
}

VOID KG3DSEEModel::GetScaleSize( KG3DSceneEntity& Entity, FLOAT* pValue )
{
	KG3DModel *pModel = static_cast<KG3DModel*>(Entity.m_pPointer);
	if (pModel->GetType() == MESHTYPE_SFX)
	{
		KG3DSFX *psfx = (KG3DSFX*)pModel;
		*pValue = psfx->GetScaleSize();
	}
}

HRESULT KG3DSEEModel::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _stprintf_s(pName, BufferSize, _T("模型 : %s")
		, static_cast<KG3DModel*>(Entity.m_pPointer)->m_scName.c_str());
	return nRet > 0 ? S_OK : E_FAIL;
}

HRESULT KG3DSEEModel::SetLOD( KG3DSceneEntity& Entity, FLOAT fLOD )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->SetLOD(fLOD);
}

BOOL KG3DSEEModel::IsSecondUVExist( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->IsSecondUVExist();
}

VOID KG3DSEEModel::GetProfile( KG3DSceneEntity& Entity, DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID )
{
	static_cast<KG3DModel*>(Entity.m_pPointer)->GetProfile(dwNumFacesTotal, dwNumVertices, dwNumTextures, dwNumID);
}

DWORD KG3DSEEModel::GetSceneLayerID( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->GetSceneLayerID();
}

VOID KG3DSEEModel::SetSceneLayerID( KG3DSceneEntity& Entity, DWORD ID )
{
	return static_cast<KG3DModel*>(Entity.m_pPointer)->SetSceneLayerID(ID);
}

//HRESULT KG3DSEEModel::GetBBoxOld( KG3DSceneEntity& Entity, AABBOX* pOutBox )
//{
//	KG3DModel* p = static_cast<KG3DModel*>(Entity.m_pPointer);
//	p->ComputeBBox();
//	(*pOutBox) = p->m_BBox;
//	return S_OK;
//}

HRESULT KG3DSEEModel::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	KG3DModel* p = static_cast<KG3DModel*>(Entity.m_pPointer);
	(*pOutBox) = p->m_BBox;
	return S_OK;
}

//BOOL KG3DSEEModel::IsRayIntersect( KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /*= NULL*/ )
//{
//	AABBOX Box;
//	if (FAILED(this->GetBBox(Entity, &Box)))
//		return FALSE;
//
//	//这部分模糊选择的功能写在PointSelection里面就好了
//	//float K = 0.5F;
//
//	//KG3DModel* pModel = static_cast<KG3DModel*>(Entity.m_pPointer);
//	//if(!pModel->IsLoaded())
//	//	return FALSE;
//
//	//DWORD dwType = 0;
//	//pModel->GetType(&dwType);
//
//	//if (dwType == MESHTYPE_MODELST)
//	//{
//	//	K = 1.f;
//	//}
//	//else if (dwType == MESHTYPE_DEFAULT)
//	//{
//	//	ASSERT(((KG3DMesh*)pModel->GetMesh()));
//	//	if (
//	//		pModel->GetMesh()->m_pSkinInfo 
//	//		&& 
//	//		pModel->GetMesh()->m_ppMeshes[KG3DMesh::SMBT_ACTIVE]
//	//	)
//	//	{
//	//		K = 1.f;
//	//	}
//	//}
//
//
//
//	//float fDistance = PointDistanceLine(&Box.GetCenter(),&Origin,&
//
//	//	(Origin+Direction));
//	//float L = D3DXVec3Length(&(Box.A - Box.B));
//	//float R = L * K;
//
//	//if (fDistance > R)
//	//	return FALSE;
//
//	//if (g_bClient)
//	//{
//	//	if (vecDarkEntitys && fDistance < L * 0.58f)
//	//		vecDarkEntitys->push_back(Entity);
//	//}
//
//	KG3DModel* pModel = static_cast<KG3DModel*>(Entity.m_pPointer);
//	return pModel->IsRayIntersect(Inter, Origin, Direction);
//}

BOOL KG3DSEEModel::RayIntersectionForEntity(KG3DSceneEntity& Entity, const D3DXVECTOR3& vSrc
												  , const D3DXVECTOR3& vNormalizeDir, FLOAT* pfRet)
{
	KG3DModel* pModel = static_cast<KG3DModel*>(Entity.m_pPointer);
	return pModel->RayIntersection(vSrc, vNormalizeDir, pfRet, FALSE, NULL);
}

//Group////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEEGroup::Clone() const
{
	return new KG3DSEEGroup;
}

HRESULT KG3DSEEGroup::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->FrameMove();
}

HRESULT KG3DSEEGroup::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->Render(dwOption);
}

HRESULT KG3DSEEGroup::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->RenderSelectMark(dwColor);
}

HRESULT KG3DSEEGroup::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEGroup::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->SetRotation(pValue, pValueXYZ);
	return S_OK;
}

VOID KG3DSEEGroup::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

VOID KG3DSEEGroup::SetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->SetScalingCenter(pValue);
}

HRESULT KG3DSEEGroup::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEEGroup::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->GetRotation(pValue, pValueXYZ);
	return S_OK;
}

HRESULT KG3DSEEGroup::UpdateTransformation( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->ComputeBBox();
}

HRESULT KG3DSEEGroup::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _stprintf_s(pName, BufferSize, _T("群组 : %s")
		, static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer)->m_scName.c_str());
	return nRet > 0 ? S_OK : E_FAIL;
}

HRESULT KG3DSEEGroup::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	KG3DSceneEntityGroup* pGroup = static_cast<KG3DSceneEntityGroup*>(Entity.m_pPointer);
	pGroup->ComputeBBox();
	(*pOutBox) = pGroup->m_BBox;
	return S_OK;
}
//SFXMeteor////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEESFXMeteor::Clone() const
{
	return new KG3DSEESFXMeteor;
}

HRESULT KG3DSEESFXMeteor::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->EditorRender();
}

HRESULT KG3DSEESFXMeteor::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->RenderSelectMark();
}	

HRESULT KG3DSEESFXMeteor::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

VOID KG3DSEESFXMeteor::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEESFXMeteor::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXMeteor::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

VOID KG3DSEESFXMeteor::SetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetScalingCenter(pValue);
}

HRESULT KG3DSEESFXMeteor::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXMeteor::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXMeteor::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXMeteor::GetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetScalingRotation(pValue);
}

VOID KG3DSEESFXMeteor::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

HRESULT KG3DSEESFXMeteor::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

D3DXMATRIX KG3DSEESFXMeteor::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->m_Matrix;
}

D3DXMATRIX KG3DSEESFXMeteor::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->m_Matrix;
}

HRESULT KG3DSEESFXMeteor::UpdateTransformation( KG3DSceneEntity& Entity )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->UpdateTransformation();
	return S_OK;
}

VOID KG3DSEESFXMeteor::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

VOID KG3DSEESFXMeteor::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetRotationCenter(pValue);	
}

HRESULT KG3DSEESFXMeteor::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _tcscpy_s(pName, BufferSize, _T("流星"));
	return nRet == 0? S_OK : E_FAIL; 
}

HRESULT KG3DSEESFXMeteor::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	return static_cast<KG3DSFXMeteor*>(Entity.m_pPointer)->GetAABBox(pOutBox);
}
//在基类中就实现了，用BBOX的结果作为碰撞结果
//BOOL KG3DSEESFXMeteor::IsRayIntersect( KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /*= NULL*/ )
//{
//	AABBOX Box;
//	if (FAILED(GetBBox(Entity, &Box)))
//		return FALSE;
//
//	float K = 0.5F;
//	float fDistance = PointDistanceLine(&Box.GetCenter(),&Origin,&
//
//		(Origin+Direction));
//	float L = D3DXVec3Length(&(Box.A - Box.B));
//	float R = L * K;
//
//	if (fDistance > R)
//		return FALSE;
//
//	if (g_bClient)
//	{
//		if (vecDarkEntitys && fDistance < L * 0.58f)
//			vecDarkEntitys->push_back(Entity);
//	}
//
//	KG3DSFXMeteor *pMeteor = static_cast<KG3DSFXMeteor*>(Entity.m_pPointer);
//	return pMeteor->IsRayIntersect(Inter, Origin, Direction);
//}

//SFXParticleLauncher////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEESFXLauncher::Clone() const
{
	return new KG3DSEESFXLauncher;
}
HRESULT KG3DSEESFXLauncher::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->FrameMove();
}
HRESULT KG3DSEESFXLauncher::Clone( KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ )
{
	KG3DSFXParticleLauncher *pLauncher = static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer);
	KG3DSFXParticleLauncher *pClone = NULL;
	pLauncher->Clone(&pClone);

	_ASSERTE(pClone);

	D3DXVECTOR3 vSrcPos;
	pLauncher->m_RtsTimeLine.m_TranslationTimeLine.GetData(&vSrcPos, 0);

	D3DXVECTOR3 vOffset = D3DXVECTOR3(fX, fY, fZ) - vSrcPos;
	int nFrameNum = (int)pClone->m_RtsTimeLine.m_TranslationTimeLine.GetSize();
	for (int i = 0; i < nFrameNum; ++i)
	{
		int nFrame;
		D3DXVECTOR3 vPos;
		pClone->m_RtsTimeLine.m_TranslationTimeLine.GetKeyFrameByIndex(&nFrame, 

			&vPos, i);
		vPos += vOffset;
		pClone->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(nFrame, vPos);
	}
	pClone->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
	pClone->UpdateTransformation();

	_ASSERTE(pOtherEntity);
	*pOtherEntity = KG3DSceneEntity(SCENEENTITY_SFX_LAUNCHER, pClone);
	return S_OK;
}
HRESULT KG3DSEESFXLauncher::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->EditorRender();
}

HRESULT KG3DSEESFXLauncher::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->RenderSelectMark(0xffff0000);
}

HRESULT KG3DSEESFXLauncher::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXLauncher::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

VOID KG3DSEESFXLauncher::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEESFXLauncher::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXLauncher::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

HRESULT KG3DSEESFXLauncher::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXLauncher::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

VOID KG3DSEESFXLauncher::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetRotationCenter(pValue);
}

HRESULT KG3DSEESFXLauncher::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXLauncher::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

D3DXMATRIX KG3DSEESFXLauncher::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->m_matWorld;
}

D3DXMATRIX KG3DSEESFXLauncher::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->m_Matrix;
}

VOID KG3DSEESFXLauncher::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

HRESULT KG3DSEESFXLauncher::UpdateTransformation( KG3DSceneEntity& Entity )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->UpdateTransformation();
	return S_OK;
}

HRESULT KG3DSEESFXLauncher::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _tcscpy_s(pName, BufferSize, _T("粒子发射器"));
	return nRet == 0? S_OK : E_FAIL;
}

HRESULT KG3DSEESFXLauncher::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer)->GetAABBox(pOutBox);
	return S_OK;
}


// sfx box projection


KG3DSceneEntityExecutor* KG3DSEESFXBoxProjection::Clone() const
{
    return new KG3DSEESFXBoxProjection;
}

HRESULT KG3DSEESFXBoxProjection::FrameMove( KG3DSceneEntity& Entity )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->FrameMove();
}

HRESULT KG3DSEESFXBoxProjection::Clone( KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ )
{
    //KG3DSFXParticleLauncher *pLauncher = static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer);
    //KG3DSFXParticleLauncher *pClone = NULL;
    //pLauncher->Clone(&pClone);

    //_ASSERTE(pClone);

    //D3DXVECTOR3 vSrcPos;
    //pLauncher->m_RtsTimeLine.m_TranslationTimeLine.GetData(&vSrcPos, 0);

    //D3DXVECTOR3 vOffset = D3DXVECTOR3(fX, fY, fZ) - vSrcPos;
    //int nFrameNum = (int)pClone->m_RtsTimeLine.m_TranslationTimeLine.GetSize();
    //for (int i = 0; i < nFrameNum; ++i)
    //{
    //    int nFrame;
    //    D3DXVECTOR3 vPos;
    //    pClone->m_RtsTimeLine.m_TranslationTimeLine.GetKeyFrameByIndex(&nFrame, 

    //        &vPos, i);
    //    vPos += vOffset;
    //    pClone->m_RtsTimeLine.m_TranslationTimeLine.InsertKeyFrame(nFrame, vPos);
    //}
    //pClone->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));
    //pClone->UpdateTransformation();

    //_ASSERTE(pOtherEntity);
    //*pOtherEntity = KG3DSceneEntity(SCENEENTITY_SFX_LAUNCHER, pClone);
    return S_OK;
}
HRESULT KG3DSEESFXBoxProjection::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
    return E_NOTIMPL;
    //return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->EditorRender();
}

HRESULT KG3DSEESFXBoxProjection::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
    return E_NOTIMPL;
    //return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->RenderSelectMark(0xffff0000);
}

HRESULT KG3DSEESFXBoxProjection::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->SetTranslation(pValue);
    return S_OK;
}

HRESULT KG3DSEESFXBoxProjection::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->SetRotation(pValue);
    return S_OK;
}

VOID KG3DSEESFXBoxProjection::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEESFXBoxProjection::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->SetScaling(pValue);
    return S_OK;
}

VOID KG3DSEESFXBoxProjection::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

HRESULT KG3DSEESFXBoxProjection::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetTranslation(pValue);
    return S_OK;
}

HRESULT KG3DSEESFXBoxProjection::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetRotation(pValue);
    return S_OK;
}

VOID KG3DSEESFXBoxProjection::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetRotationCenter(pValue);
}

HRESULT KG3DSEESFXBoxProjection::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetScaling(pValue);
    return S_OK;
}

VOID KG3DSEESFXBoxProjection::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

D3DXMATRIX KG3DSEESFXBoxProjection::GetWorldMatrix( KG3DSceneEntity& Entity )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->m_matWorld;
}

D3DXMATRIX KG3DSEESFXBoxProjection::GetMatrix( KG3DSceneEntity& Entity )
{
    return static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->m_Matrix;
}

VOID KG3DSEESFXBoxProjection::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

HRESULT KG3DSEESFXBoxProjection::UpdateTransformation( KG3DSceneEntity& Entity )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->UpdateTransformation();
    return S_OK;
}

HRESULT KG3DSEESFXBoxProjection::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
    int nRet = _tcscpy_s(pName, BufferSize, _T("立方投影"));
    return nRet == 0? S_OK : E_FAIL;
}

HRESULT KG3DSEESFXBoxProjection::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
    static_cast<KG3DSfxBoxProjection*>(Entity.m_pPointer)->GetAABBox(pOutBox);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////////













//在基类中就实现了，用BBOX的结果作为碰撞结果
//BOOL KG3DSEESFXLauncher::IsRayIntersect( KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /*= NULL*/ )
//{
//	AABBOX Box;
//	if (FAILED(this->GetBBox(Entity, &Box)))
//		return FALSE;
//
//	float K = 0.5F;
//	float fDistance = PointDistanceLine(&Box.GetCenter(),&Origin,&
//
//		(Origin+Direction));
//	float L = D3DXVec3Length(&(Box.A - Box.B));
//	float R = L * K;
//
//	if (fDistance > R)
//		return FALSE;
//
//	if (g_bClient)
//	{
//		if (vecDarkEntitys && fDistance < L * 0.58f)
//			vecDarkEntitys->push_back(Entity);
//	}
//
//	KG3DSFXParticleLauncher *pLauncher = static_cast<KG3DSFXParticleLauncher*>(Entity.m_pPointer);
//	return pLauncher->IsRayIntersect(Inter, Origin, Direction);
//}

//Track////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEESFXTrack::Clone() const
{
	return new KG3DSEESFXTrack;
}

HRESULT KG3DSEESFXTrack::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->Render();
}

HRESULT KG3DSEESFXTrack::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->RenderSelectMark();
}

HRESULT KG3DSEESFXTrack::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXTrack::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetRotation(pValue);
	return S_OK;
}

VOID KG3DSEESFXTrack::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetRotationCenter(pValue);
}

HRESULT KG3DSEESFXTrack::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXTrack::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetScalingRotation(pValue);
}

VOID KG3DSEESFXTrack::SetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->SetScalingCenter(pValue);
}

HRESULT KG3DSEESFXTrack::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXTrack::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

VOID KG3DSEESFXTrack::GetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetRotationCenter(pValue);
}

HRESULT KG3DSEESFXTrack::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

VOID KG3DSEESFXTrack::GetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetScalingRotation(pValue);
}

VOID KG3DSEESFXTrack::GetScalingCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetScalingCenter(pValue);
}

D3DXMATRIX KG3DSEESFXTrack::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->m_Matrix;
}

D3DXMATRIX KG3DSEESFXTrack::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->m_Matrix;
}

VOID KG3DSEESFXTrack::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->UpdateByMatrix(mat);
}

HRESULT KG3DSEESFXTrack::UpdateTransformation( KG3DSceneEntity& Entity )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->UpdateTransformation();
	return S_OK;
}

HRESULT KG3DSEESFXTrack::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _tcscpy_s(pName, BufferSize, _T("轨道"));
	return nRet == 0? S_OK : E_FAIL;
}

HRESULT KG3DSEESFXTrack::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->GetAABBox(pOutBox);
	return S_OK;
}

HRESULT KG3DSEESFXTrack::FrameMove( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXTrack*>(Entity.m_pPointer)->FrameMove();
}
//BillBoard////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEESFXBillboard::Clone() const
{
	return new KG3DSEESFXBillboard;
}

HRESULT KG3DSEESFXBillboard::Clone( KG3DSceneEntity& Entity, KG3DSceneEntity *pOtherEntity, float fX, float fY, float fZ )
{
	KG3DSFXBillboard *pBillboard = static_cast<KG3DSFXBillboard*>(Entity.m_pPointer);
	KG3DSFXBillboard *pClone = NULL;
	pBillboard->Clone(&pClone);
	_ASSERTE(pClone);
	D3DXVECTOR3 vSrcPos;
	pBillboard->m_vPositionLine.GetData(&vSrcPos, 0);
	D3DXVECTOR3 vOffset = D3DXVECTOR3(fX, fY, fZ) - vSrcPos;
	int nFrameNum = (int)pClone->m_vPositionLine.GetSize();
	for (int i = 0; i < nFrameNum; ++i)
	{
		int nFrame;
		D3DXVECTOR3 vPos;
		pClone->m_vPositionLine.GetKeyFrameByIndex(&nFrame, &vPos, i);
		vPos += vOffset;
		pClone->m_vPositionLine.InsertKeyFrame(nFrame, vPos);
	}
	pClone->SetTranslation(&D3DXVECTOR3(fX, fY, fZ));

	_ASSERTE(pOtherEntity);
	*pOtherEntity = KG3DSceneEntity(SCENEENTITY_SFX_BILLBOARD, pClone);
	return S_OK;
}
HRESULT KG3DSEESFXBillboard::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->RenderSelectMark();
}

HRESULT KG3DSEESFXBillboard::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	_ASSERTE(false);
	return S_OK;
}


HRESULT KG3DSEESFXBillboard::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	KG3DSFXBillboard *pBillboard = static_cast<KG3DSFXBillboard*>(Entity.m_pPointer);
	pBillboard->SetHeight(pValue->y);
	pBillboard->SetWidht(pValue->x);
	return S_OK;
}

VOID KG3DSEESFXBillboard::SetScalingRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue )
{
	_ASSERTE(FALSE);
}

HRESULT KG3DSEESFXBillboard::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->GetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXBillboard::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	_ASSERTE(FALSE);
	return S_OK;
}

D3DXMATRIX KG3DSEESFXBillboard::GetWorldMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->m_matWorld;
}

D3DXMATRIX KG3DSEESFXBillboard::GetMatrix( KG3DSceneEntity& Entity )
{
	return static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->m_Matrix;
}

VOID KG3DSEESFXBillboard::SetLocalMatrix( KG3DSceneEntity& Entity, const D3DXMATRIX& mat )
{
	KG3DSFXBillboard* pBillboard = static_cast<KG3DSFXBillboard*>(Entity.m_pPointer);
	D3DXVECTOR3 t;
	D3DXVECTOR3 s;
	D3DXMATRIX  r;
	MatrixExract(s, t, r, const_cast<D3DXMATRIX*>(&mat));
	pBillboard->SetHeight(pBillboard->GetHeight() * s.y);
	pBillboard->SetWidht(pBillboard->GetWidht() * s.x);
}

HRESULT KG3DSEESFXBillboard::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->SetTranslation(pValue);
	return S_OK;
}

HRESULT KG3DSEESFXBillboard::GetName( KG3DSceneEntity& Entity, LPTSTR pName, size_t BufferSize )
{
	int nRet = _tcscpy_s(pName, BufferSize, _T("公告板"));
	return nRet == 0? S_OK : E_FAIL;
}

HRESULT KG3DSEESFXBillboard::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	static_cast<KG3DSFXBillboard*>(Entity.m_pPointer)->GetAABBox(pOutBox);
	return S_OK;
}

//在基类中就实现了，用BBOX的结果作为碰撞结果
//BOOL KG3DSEESFXBillboard::IsRayIntersect( KG3DSceneEntity& Entity, D3DXVECTOR3&Inter, D3DXVECTOR3 Origin , D3DXVECTOR3 Direction, std::vector<KG3DSceneEntity>* vecDarkEntitys /*= NULL*/ )
//{
//	AABBOX Box;
//	if (FAILED(GetBBox(Entity, &Box)))
//		return FALSE;
//
//	float K = 0.5F;
//	float fDistance = PointDistanceLine(&Box.GetCenter(),&Origin,&
//
//		(Origin+Direction));
//	float L = D3DXVec3Length(&(Box.A - Box.B));
//	float R = L * K;
//
//	if (fDistance > R)
//		return FALSE;
//
//	if (g_bClient)
//	{
//		if (vecDarkEntitys && fDistance < L * 0.58f)
//			vecDarkEntitys->push_back(Entity);
//	}
//
//	KG3DSFXBillboard *pBillboard = static_cast<KG3DSFXBillboard*>(Entity.m_pPointer);
//	return pBillboard->IsRayIntersect(Inter, Origin, Direction);
//}

//Position////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEEPosition::Clone() const
{
	return new KG3DSEEPosition;
}

HRESULT KG3DSEEPosition::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	return g_cGraphicsTool.DrawBoxAtPos(*static_cast<D3DXVECTOR3*>(Entity.m_pPointer), 5, 0xffffffff, FALSE);
}

HRESULT KG3DSEEPosition::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	return g_cGraphicsTool.DrawBoxAtPos(*static_cast<D3DXVECTOR3*>(Entity.m_pPointer), 10, dwColor, FALSE);
}

HRESULT KG3DSEEPosition::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	*static_cast<D3DXVECTOR3*>(Entity.m_pPointer) = *pValue;
	return S_OK;
}

HRESULT KG3DSEEPosition::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	*pValue = *static_cast<D3DXVECTOR3*>(Entity.m_pPointer);
	return S_OK;
}

HRESULT KG3DSEEPosition::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	D3DXVECTOR3* p = static_cast<D3DXVECTOR3*>(Entity.m_pPointer);
	*pOutBox = AABBOX(*p - D3DXVECTOR3(5,5,5), *p + D3DXVECTOR3(5,5,5));
	return S_OK;
}
//Clone////////////////////////////////////////////////////////////////////////
KG3DSceneEntityExecutor* KG3DSEESpeedTreeLeaf::Clone() const
{
	return new KG3DSEESpeedTreeLeaf;
}

HRESULT KG3DSEESpeedTreeLeaf::RenderOption( KG3DSceneEntity& Entity, DWORD dwOption )
{
	Leaf* pLeaf = static_cast<Leaf*>(Entity.m_pPointer);
	HRESULT hr = pLeaf->Render();
	pLeaf->bSelected = FALSE;
	return hr;
}

HRESULT KG3DSEESpeedTreeLeaf::RenderSelectMark( KG3DSceneEntity& Entity, DWORD dwColor )
{
	Leaf* pLeaf = static_cast<Leaf*>(Entity.m_pPointer);
	HRESULT hr = pLeaf->RenderSelectMark(dwColor);
	pLeaf->bSelected = TRUE;
	return hr;
}

HRESULT KG3DSEESpeedTreeLeaf::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	*pValue = static_cast<Leaf*>(Entity.m_pPointer)->Position;
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeaf::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	Leaf* pLeaf = static_cast<Leaf*>(Entity.m_pPointer);
	pLeaf->Size = pValue->x;
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeaf::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	Leaf* pLeaf = static_cast<Leaf*>(Entity.m_pPointer);
	D3DXVECTOR3 vec3Offset = *pValue - pLeaf->Position;
	pLeaf->Position = (*pValue);
	pLeaf->OriginPosition += vec3Offset;
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeaf::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	FLOAT fSize = static_cast<Leaf*>(Entity.m_pPointer)->Size;
	*pValue = D3DXVECTOR3(fSize, fSize, fSize);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeaf::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	HRESULT hResult = E_FAIL;
	Leaf* pLeaf = static_cast<Leaf*>(Entity.m_pPointer);
	
	KG_PROCESS_ERROR(pLeaf);
	KG_PROCESS_ERROR(pOutBox);

	pOutBox->AddPosition(pLeaf->Position - D3DXVECTOR3(5,5,5));
	pOutBox->AddPosition(pLeaf->Position + D3DXVECTOR3(5,5,5));

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DSEESpeedTreeLeaf::ProcessMessage( KG3DSceneEntity& Entity, UINT message, WPARAM wParam, LPARAM lParam )
{
	static_cast<Leaf*>(Entity.m_pPointer)->SetDirtyFlag();
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeaf::OnDelete( const KG3DSceneEntity& Entity )
{
	/*
	Leaf* pLeaf = (Leaf*) m_pPointer;
	HRESULT hr = pLeaf->OnDelete();
	m_pPointer = NULL;
	return hr;	*/
	//把m_pPointer改成NULL是不行的，因为这样子之后的list要remove掉这个Entity的时候，执行==算法会不准
	return static_cast<Leaf*>(Entity.m_pPointer)->OnDelete();
}

KG3DSceneEntityExecutor* KG3DSEESpeedTreeLeafGroup::Clone() const
{
	return new KG3DSEESpeedTreeLeafGroup;
}

HRESULT KG3DSEESpeedTreeLeafGroup::SetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	KG3DSpeedTreeLeafGroup *pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	pLeafGroup->OnRotation(*pValue);
	pLeafGroup->SetRotation(pValue);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::SetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer)->OnScale(*pValue);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::GetScaling( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer)->GetScaling(pValue);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::GetRotation( KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ )
{
	static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer)->GetRotation(pValue);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::GetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer)->GetCenter(*pValue);
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::UpdateTransformation( KG3DSceneEntity& Entity )
{
	static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer)->UpdateTransformation();
	return S_OK;
}

HRESULT KG3DSEESpeedTreeLeafGroup::SetTranslation( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	KG3DSpeedTreeLeafGroup* pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	D3DXVECTOR3 vec3Center;
	pLeafGroup->GetCenter(vec3Center);
	D3DXVECTOR3 vec3Offset = *pValue - vec3Center;
	pLeafGroup->OnMove(vec3Offset);	
	return S_OK;
}

VOID KG3DSEESpeedTreeLeafGroup::SetRotationCenter( KG3DSceneEntity& Entity, D3DXVECTOR3* pValue )
{
	D3DXVECTOR3 vec3Center;
	KG3DSpeedTreeLeafGroup *pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	pLeafGroup->GetCenter(vec3Center);
	pLeafGroup->SetRotationCenter(&vec3Center);	
}

HRESULT KG3DSEESpeedTreeLeafGroup::GetBBox( KG3DSceneEntity& Entity, AABBOX* pOutBox )
{
	KG3DSpeedTreeLeafGroup* pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	D3DXVECTOR3 vec3Center;
	pLeafGroup->GetCenter(vec3Center);
	D3DXVECTOR3 vec3A = vec3Center - D3DXVECTOR3(100.0f, 100.0f, 100.0f);
	D3DXVECTOR3 vec3B = vec3Center + D3DXVECTOR3(100.0f, 100.0f, 100.0f);
	pOutBox->AddPosition(vec3A);
	pOutBox->AddPosition(vec3B);
	return S_OK;
}


D3DXMATRIX KG3DSEESpeedTreeLeafGroup::GetWorldMatrix(KG3DSceneEntity& Entity)
{
	KG3DSpeedTreeLeafGroup *pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	return pLeafGroup->GetWorldMatrix();
}

D3DXMATRIX KG3DSEESpeedTreeLeafGroup::GetMatrix(KG3DSceneEntity& Entity)
{
	KG3DSpeedTreeLeafGroup *pLeafGroup = static_cast<KG3DSpeedTreeLeafGroup*>(Entity.m_pPointer);
	return pLeafGroup->GetMatrix();
}

VOID KG3DSEESpeedTreeLeafGroup::SetLocalMatrix(KG3DSceneEntity& Entity, const D3DXMATRIX& mat)
{
	KG3DTransformation t;
	t.UpdateByMatrix(mat);

	SetScaling(Entity, &t.m_Scaling);
	SetRotation(Entity, &t.m_Rotation, NULL);
	SetTranslation(Entity, &t.m_Translation);
}

//////////////////////////////////////////////////////////////////////////
EntityList::EntityList() 
{
}

EntityList::EntityList(const std::list<KG3DSceneEntity>& Entitys) : listEntitys(Entitys) 
{
}

size_t EntityList::GetSize() 
{
	return listEntitys.size();
}

void EntityList::Clear()
{
	return listEntitys.clear();
}

void EntityList::AddEntity(const KG3DSceneEntity& Entity)
{
	return listEntitys.push_back(Entity);
};

D3DXVECTOR3 EntityList::GetCenter()
{
	D3DXVECTOR3 vCenter = D3DXVECTOR3(0.f, 0.f, 0.f);

	if (listEntitys.size() < 0)
		return vCenter;

	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		AABBOX Box;
		(*i).GetBBox(&Box);
		vCenter += Box.GetCenter();
	}

	return vCenter / static_cast<FLOAT>(GetSize());
}

D3DXMATRIX EntityList::GetWorldMatrix()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	if (GetSize() <= 0)
		return matWorld;

	if (GetSize() == 1)
		return listEntitys.front().GetWorldMatrix();

	D3DXVECTOR3 vCenter = GetCenter();

	matWorld._41 = vCenter.x;
	matWorld._42 = vCenter.y;
	matWorld._43 = vCenter.z;

	return matWorld;
}

D3DXMATRIX EntityList::GetMatrix()
{
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);

	if (GetSize() <= 0)
		return matWorld;

	if (GetSize() == 1)
		return listEntitys.front().GetMatrix();

	return GetWorldMatrix();
}

void EntityList::SetLocalMatrix(const D3DXMATRIX& Mat)
{
	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		(*i).SetLocalMatrix(Mat);
		(*i).UpdateTransformation();
	}
}

void EntityList::Translation(const D3DXVECTOR3& Move)
{
	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		D3DXMATRIX matWorld = (*i).GetWorldMatrix();
		D3DXMATRIX matLocal = (*i).GetMatrix();
		D3DXVECTOR3 vMove = Move;

		if (matWorld != matLocal)
		{
			D3DXMATRIX matParent;
			D3DXMATRIX matLocalInv;
			D3DXMATRIX matParentInv;

			D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
			matParent = matLocalInv * matWorld;
			D3DXMatrixInverse(&matParentInv, NULL, &matParent);
			D3DXVec3TransformNormal(&vMove, &vMove, &matParentInv);
		}

		D3DXVECTOR3 vTrans;
		(*i).GetTranslation(&vTrans);
		vTrans += vMove;
		(*i).SetTranslation(&vTrans);
		(*i).UpdateTransformation();

	}
}

void EntityList::Scale(const D3DXVECTOR3& vScaling, DWORD dwMode)
{
	if (dwMode == KG3DCOORD_WORLD || GetSize() > 1)
	{
		for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
			i != listEntitys.end(); ++i)
		{
			D3DXVECTOR3 vTrans;
			D3DXMATRIX  mTrans;
			D3DXMATRIX  mTransInv;
			AABBOX box;
			D3DXMATRIX mat;
			D3DXVECTOR3 vScal = vScaling;

			D3DXMATRIX matWorld = (*i).GetWorldMatrix();
			D3DXMATRIX matLocal = (*i).GetMatrix();
			D3DXMATRIX matParentInv;
			D3DXMatrixIdentity(&matParentInv);

			if (matWorld != matLocal)
			{
				D3DXMATRIX matParent;
				D3DXMATRIX matLocalInv;
				D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
				matParent = matLocalInv * matWorld;
				D3DXMatrixInverse(&matParentInv, NULL, &matParent);
			}

			(*i).GetBBox(&box);

			vScal = D3DXVECTOR3(1.f, 1.f, 1.f) + vScal / box.GetDiameter();
			D3DXMatrixScaling(&mat, vScal.x, vScal.y, vScal.z);

			vTrans.x = matWorld._41;
			vTrans.y = matWorld._42;
			vTrans.z = matWorld._43;

			D3DXMatrixTranslation(&mTrans, vTrans.x, vTrans.y, vTrans.z);
			D3DXMatrixTranslation(&mTransInv, -vTrans.x, -vTrans.y, -vTrans.z);

			mat = matWorld * mTransInv * mat * mTrans * matParentInv;

			(*i).SetLocalMatrix(mat);
			(*i).UpdateTransformation();

		}
	}
	else
	{
		KG3DSceneEntity Entity = *listEntitys.begin();
		D3DXMATRIX matWorld = Entity.GetWorldMatrix();
		D3DXMATRIX matLocal = Entity.GetMatrix();
		D3DXMATRIX matParentInv;
		D3DXMatrixIdentity(&matParentInv);

		D3DXVECTOR3 vScal = vScaling;
		AABBOX box;
		D3DXMATRIX mat;

		Entity.GetBBox(&box);

		if (matWorld != matLocal)
		{
			D3DXMATRIX matParent;
			D3DXMATRIX matLocalInv;
			D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
			matParent = matLocalInv * matWorld;
			D3DXMatrixInverse(&matParentInv, NULL, &matParent);
		}

		D3DXMATRIX mWorldInv;
		D3DXMatrixInverse(&mWorldInv, NULL, &matWorld);
		float fLength = D3DXVec3Length(&vScal);
		D3DXVec3TransformNormal(&vScal, &vScal, &mWorldInv);
		D3DXVec3Normalize(&vScal, &vScal);
		vScal *= fLength;
		vScal = D3DXVECTOR3(1.f, 1.f, 1.f) + vScal / box.GetDiameter();
		D3DXMATRIX mScal;
		D3DXMatrixScaling(&mScal, vScal.x, vScal.y, vScal.z);
		mat = mScal * matWorld * matParentInv;

		Entity.SetLocalMatrix(mat);
		Entity.UpdateTransformation();

	}
}

void EntityList::Rotation(const D3DXMATRIX& Mat)
{
	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		D3DXMATRIX matWorld = (*i).GetWorldMatrix();
		D3DXMATRIX matLocal = (*i).GetMatrix();
		D3DXMATRIX matParentInv;
		D3DXMatrixIdentity(&matParentInv);

		D3DXMATRIX mat = Mat;

		if (matWorld != matLocal)
		{
			D3DXMATRIX matParent;
			D3DXMATRIX matLocalInv;
			D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
			matParent = matLocalInv * matWorld;
			D3DXMatrixInverse(&matParentInv, NULL, &matParent);
		}

		D3DXVECTOR3 vTrans;
		D3DXMATRIX  mTrans;
		D3DXMATRIX  mTransInv;

		vTrans.x = matWorld._41;
		vTrans.y = matWorld._42;
		vTrans.z = matWorld._43;

		D3DXMatrixTranslation(&mTrans, vTrans.x, vTrans.y, vTrans.z);
		D3DXMatrixTranslation(&mTransInv, -vTrans.x, -vTrans.y, -vTrans.z);

		mat = matWorld * mTransInv * mat * mTrans * matParentInv;

		(*i).SetLocalMatrix(mat);
		(*i).UpdateTransformation();

	}
}

void EntityList::Update() 
{
	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		(*i).FrameMove();
	}
}

void EntityList::GetBBox(AABBOX* Box)
{
	Box->Clear();
	for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)
	{
		D3DXVECTOR3 v[8];
		AABBOX _Box;
		(*i).GetBBox(&_Box);
		_Box.Extract(v);
		Box->AddPosition(v, sizeof(v) / sizeof(*v));
	}
}

