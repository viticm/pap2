#include "KGMaterialTestCase.h"

bool KGMaterialTestCase::m_MtlCanBeLoaded = false;

KGMaterialTestCase::KGMaterialTestCase()
{
	m_pMtlTable = NULL;

}
KGMaterialTestCase::~KGMaterialTestCase()
{

}

int KGMaterialTestCase::Init(IKG3DEngineManager*	 pEngineMgr,
							 IKG3DAutoTestInterface* pTestInterface,
							 const char				 cszClientPath[],
							 const char				 cszResultPath[])
{
	int nResult  = false;
	int nRetCode = false;
	int nBaseInitFlag = false;

	nRetCode = KGBaseResourceTestCase::Init(pEngineMgr, pTestInterface, cszClientPath, cszResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);
	nBaseInitFlag = true;

	m_pMtlTable = m_pTestInterface->GetMaterialTable();
	KGLOG_PROCESS_ERROR(m_pMtlTable);

	nResult = true;
Exit0:
	if (!nResult && nBaseInitFlag)
	{
		UnInit();
	}
	return nResult;
}

void KGMaterialTestCase::UnInit()
{
	KGBaseResourceTestCase::UnInit();
}
int KGMaterialTestCase::LoadMaterialByNewFunc(const char cszFileName[], IEKG3DMaterial** ppMaterial)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	KG3DMaterial* pMaterial = NULL;

	_ASSERTE(cszFileName);
	KG_ASSERT_EXIT(ppMaterial);

	hrRetCode = m_pMtlTable->LoadResourceFromFile(cszFileName, 0, 0, (IKG3DResourceBase**)&pMaterial);
	KG_COM_PROCESS_ERROR(hrRetCode);

	*ppMaterial = pMaterial;
	nResult = true;
Exit0:
	if (!nResult && pMaterial)
	{
		ReleaseMtl(pMaterial);
	}
	return nResult;
}

int KGMaterialTestCase::CompareOption(void* pvOption1, void* pvOption2)
{
	int nResult = false;

	KG_ASSERT_EXIT(pvOption1);
	KG_ASSERT_EXIT(pvOption2);

	KG3DMaterial::_MtlOption* pOldOption = static_cast<KG3DMaterial::_MtlOption*>(pvOption1);
	KG3DMaterial::_MtlOption* pNewOption = static_cast<KG3DMaterial::_MtlOption*>(pvOption2);

	KG_PROCESS_ERROR(pOldOption->Type == pNewOption->Type);

	switch(pOldOption->Type)
	{
	case OPTION_MATERIAL_CULLING:
		{
			KG3DMaterial::_MatCull* pOldData = static_cast<KG3DMaterial::_MatCull*>(pOldOption->pData);
			KG3DMaterial::_MatCull* pNewData = static_cast<KG3DMaterial::_MatCull*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->CullMode == pNewData->CullMode);
			KG_PROCESS_ERROR(pOldData->BakCullMode == pNewData->BakCullMode);
			break;
		}
	case OPTION_MATERIAL_ALPHABLEND:
		{
			KG3DMaterial::_MatBlend* pOldData = static_cast<KG3DMaterial::_MatBlend*>(pOldOption->pData);
			KG3DMaterial::_MatBlend* pNewData = static_cast<KG3DMaterial::_MatBlend*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->SrcBlend == pNewData->SrcBlend);
			KG_PROCESS_ERROR(pOldData->DestBlend == pNewData->DestBlend);
			KG_PROCESS_ERROR(pOldData->BakSrcBlend == pNewData->BakSrcBlend);
			KG_PROCESS_ERROR(pOldData->BakDestBlend == pNewData->BakDestBlend);
			break;
		}
	case OPTION_MATERIAL_ALPHAOPERATION:
		{
			_MtlAlphaOperation* pOldData = static_cast<_MtlAlphaOperation*>(pOldOption->pData);
			_MtlAlphaOperation* pNewData = static_cast<_MtlAlphaOperation*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->EnableAlphaBlend == pNewData->EnableAlphaBlend);
			KG_PROCESS_ERROR(pOldData->SrcBlend == pNewData->SrcBlend);
			KG_PROCESS_ERROR(pOldData->DestBlend == pNewData->DestBlend);
			KG_PROCESS_ERROR(pOldData->BlendFactor == pNewData->BlendFactor);
			KG_PROCESS_ERROR(pOldData->BlendOperator == pNewData->BlendOperator);

			KG_PROCESS_ERROR(pOldData->EnableSeparateAlphaBlend == pNewData->EnableSeparateAlphaBlend);
			KG_PROCESS_ERROR(pOldData->BlendOperatorAlpha == pNewData->BlendOperatorAlpha);
			KG_PROCESS_ERROR(pOldData->SrcBlendAlpha == pNewData->SrcBlendAlpha);
			KG_PROCESS_ERROR(pOldData->DestBlendAlpha == pNewData->DestBlendAlpha);

			KG_PROCESS_ERROR(pOldData->EnableAlphaTest == pNewData->EnableAlphaTest);
			KG_PROCESS_ERROR(pOldData->AlphaTestRef == pNewData->AlphaTestRef);
			KG_PROCESS_ERROR(pOldData->AlphaFunction == pNewData->AlphaFunction);

			KG_PROCESS_ERROR(pOldData->BackEnableAlphaBlend == pNewData->BackEnableAlphaBlend);
			KG_PROCESS_ERROR(pOldData->BackSrcBlend == pNewData->BackSrcBlend);
			KG_PROCESS_ERROR(pOldData->BackDestBlend == pNewData->BackDestBlend);
			KG_PROCESS_ERROR(pOldData->BackBlendFactor == pNewData->BackBlendFactor);
			KG_PROCESS_ERROR(pOldData->BackBlendOperator == pNewData->BackBlendOperator);

			KG_PROCESS_ERROR(pOldData->BackEnableSeparateAlphaBlend == pNewData->BackEnableSeparateAlphaBlend);
			KG_PROCESS_ERROR(pOldData->BackBlendOperatorAlpha == pNewData->BackBlendOperatorAlpha);
			KG_PROCESS_ERROR(pOldData->BackSrcBlendAlpha == pNewData->BackSrcBlendAlpha);
			KG_PROCESS_ERROR(pOldData->BackDestBlendAlpha == pNewData->BackDestBlendAlpha);

			KG_PROCESS_ERROR(pOldData->BackEnableAlphaTest == pNewData->BackEnableAlphaTest);
			KG_PROCESS_ERROR(pOldData->BackAlphaTestRef == pNewData->BackAlphaTestRef);
			KG_PROCESS_ERROR(pOldData->BackAlphaFunction == pNewData->BackAlphaFunction);

			break;
		}
	case OPTION_MATERIAL_ALPHARENDERTWICE:
		{			
			_MtlRenderTwiceAlphaOption* pOldData = static_cast<_MtlRenderTwiceAlphaOption*>(pOldOption->pData);
			_MtlRenderTwiceAlphaOption* pNewData = static_cast<_MtlRenderTwiceAlphaOption*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->bEnable == pNewData->bEnable);
			KG_PROCESS_ERROR(pOldData->dwAlphaRefPass0 == pNewData->dwAlphaRefPass0);
			KG_PROCESS_ERROR(pOldData->dwAlphaRefPass1 == pNewData->dwAlphaRefPass1);
			break;
		}
	case OPTION_MATERIAL_MSEFFECT:
		{
			//新的加载方式已经跳过加载特效相关的数据
			break;
		}

	case OPTION_TEXTURE_OPERATION:
		{
			KG3DMaterial::_TextureOp* pOldData = static_cast<KG3DMaterial::_TextureOp*>(pOldOption->pData);
			KG3DMaterial::_TextureOp* pNewData = static_cast<KG3DMaterial::_TextureOp*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->ColorOp == pNewData->ColorOp);
			KG_PROCESS_ERROR(pOldData->ColorArg1 == pNewData->ColorArg1);
			KG_PROCESS_ERROR(pOldData->ColorArg2 == pNewData->ColorArg2);

			KG_PROCESS_ERROR(pOldData->AlphaOp == pNewData->AlphaOp);
			KG_PROCESS_ERROR(pOldData->AlphaArg1 == pNewData->AlphaArg1);
			KG_PROCESS_ERROR(pOldData->AlphaArg2 == pNewData->AlphaArg2);

			KG_PROCESS_ERROR(pOldData->TFactor == pNewData->TFactor);

			KG_PROCESS_ERROR(pOldData->BakColorOp == pNewData->BakColorOp);
			KG_PROCESS_ERROR(pOldData->BakColorArg1 == pNewData->BakColorArg1);
			KG_PROCESS_ERROR(pOldData->BakColorArg2 == pNewData->BakColorArg2);

			KG_PROCESS_ERROR(pOldData->BakAlphaOp == pNewData->BakAlphaOp);
			KG_PROCESS_ERROR(pOldData->BakAlphaArg1 == pNewData->BakAlphaArg1);
			KG_PROCESS_ERROR(pOldData->BakAlphaArg2 == pNewData->BakAlphaArg2);
			KG_PROCESS_ERROR(pOldData->BakTFactor == pNewData->BakTFactor);

			break;
		}
	case OPTION_TEXTURE_OPERATIONEX:
		{
			KG3DMaterial::_TextureOpEx* pOldData = static_cast<KG3DMaterial::_TextureOpEx*>(pOldOption->pData);
			KG3DMaterial::_TextureOpEx* pNewData = static_cast<KG3DMaterial::_TextureOpEx*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->ColorOp == pNewData->ColorOp);
			KG_PROCESS_ERROR(pOldData->ColorArg1 == pNewData->ColorArg1);
			KG_PROCESS_ERROR(pOldData->ColorArg2 == pNewData->ColorArg2);
			KG_PROCESS_ERROR(pOldData->ColorArg0 == pNewData->ColorArg0);
			KG_PROCESS_ERROR(pOldData->Result == pNewData->Result);

			KG_PROCESS_ERROR(pOldData->AlphaOp == pNewData->AlphaOp);
			KG_PROCESS_ERROR(pOldData->AlphaArg1 == pNewData->AlphaArg1);
			KG_PROCESS_ERROR(pOldData->AlphaArg2 == pNewData->AlphaArg2);
			KG_PROCESS_ERROR(pOldData->AlphaArg0 == pNewData->AlphaArg0);
			KG_PROCESS_ERROR(pOldData->Result == pNewData->Result);

			KG_PROCESS_ERROR(pOldData->TFactor == pNewData->TFactor);
			KG_PROCESS_ERROR(pOldData->Constant == pNewData->Constant);

			KG_PROCESS_ERROR(pOldData->BakColorOp == pNewData->BakColorOp);
			KG_PROCESS_ERROR(pOldData->BakColorArg1 == pNewData->BakColorArg1);
			KG_PROCESS_ERROR(pOldData->BakColorArg2 == pNewData->BakColorArg2);
			KG_PROCESS_ERROR(pOldData->BakColorArg0 == pNewData->BakColorArg0);
			KG_PROCESS_ERROR(pOldData->BakResult == pNewData->BakResult);

			KG_PROCESS_ERROR(pOldData->BakAlphaOp == pNewData->BakAlphaOp);
			KG_PROCESS_ERROR(pOldData->BakAlphaArg1 == pNewData->BakAlphaArg1);
			KG_PROCESS_ERROR(pOldData->BakAlphaArg2 == pNewData->BakAlphaArg2);
			KG_PROCESS_ERROR(pOldData->BakAlphaArg0 == pNewData->BakAlphaArg0);
			KG_PROCESS_ERROR(pOldData->BakTFactor == pNewData->BakTFactor);
			KG_PROCESS_ERROR(pOldData->BakConstant == pNewData->BakConstant);

			break;
		}
	case OPTION_TEXTURE_MAPMETHOD:
		{
			KG3DMaterial::_TextureMap* pOldData = static_cast<KG3DMaterial::_TextureMap*>(pOldOption->pData);
			KG3DMaterial::_TextureMap* pNewData = static_cast<KG3DMaterial::_TextureMap*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->TextureMap == pNewData->TextureMap);
			KG_PROCESS_ERROR(pOldData->BakTextureMap == pNewData->BakTextureMap);

			break;
		}
	case OPTION_TEXTURE_TRANSFROM:
		{
			KG3DMaterial::_TextureTf* pOldData = static_cast<KG3DMaterial::_TextureTf*>(pOldOption->pData);
			KG3DMaterial::_TextureTf* pNewData = static_cast<KG3DMaterial::_TextureTf*>(pNewOption->pData);

			KG_PROCESS_ERROR(pOldData->Scaling == pNewData->Scaling);
			KG_PROCESS_ERROR(pOldData->Rotation == pNewData->Rotation);
			KG_PROCESS_ERROR(pOldData->Position == pNewData->Position);
			KG_PROCESS_ERROR(pOldData->Speed_Scaling == pNewData->Speed_Scaling);
			KG_PROCESS_ERROR(pOldData->Speed_Rotation == pNewData->Speed_Rotation);
			KG_PROCESS_ERROR(pOldData->Speed_Position == pNewData->Speed_Position);

			break;
		}
	case OPTION_MATERIAL_ZFUN:
		{
			DWORD* pOldData = static_cast<DWORD*>(pOldOption->pData);
			DWORD* pNewData = static_cast<DWORD*>(pNewOption->pData);

			KG_PROCESS_ERROR(*pOldData == *pNewData);

			break;
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

void KGMaterialTestCase::ReleaseMtl(IEKG3DMaterial* pMaterial)
{
	HRESULT hrRetCode = E_FAIL;

    if (pMaterial)
    {
		//这里要注意加参数RELEASE_ATONCE，以便立即释放资源，默认是放入释放队列（RELEASE_INQUEUE）
		//否则内存中会一直占用释一定数量的资源，数量等于释放资源队列的大小
		hrRetCode = m_pMtlTable->ReleaseResource((KG3DMaterial*)pMaterial, RELEASE_ATONCE);
		if (FAILED(hrRetCode))
		{
			KGLogPrintf(KGLOG_ERR, "Release Mtl Failed!");
		}
		else
		{
			pMaterial = NULL;
		}
    }
}

