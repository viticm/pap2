#include "StdAfx.h"
#include ".\kg3drepresentobjecttable.h"
#include "KG3DRepresentNPC.h"
#include "KG3DRepresentObjectSet.h"
#include "KG3DRepresentObjectVisibleEntitySet.h"
#include "KG3DRepresentObjectSoundBall.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DRepresentObjectDataFlowArc.h"
#include "KG3DRepresentObjectDataFlowProcessor.h"
#include "KG3DRepresentObjectWall.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentObjectTable& g_GetObjectTable()
{
	static KG3DRepresentObjectTable stTable;
	return stTable;
}

KG3DRepresentObjectTable::KG3DRepresentObjectTable(void)
	:m_vecBuilder(REPRESENTOBJECT_TYPE_COUNT, NULL)
{
	_ASSERTE(REPRESENTOBJECT_TYPE_COUNT > REPRESENTOBJECT_TEST);
	m_pDefaultType = &g_GloableDefaultResourceType;//不要去掉这句，测试程序Release一个Object要用
}

KG3DRepresentObjectTable::~KG3DRepresentObjectTable(void)
{
}

HRESULT KG3DRepresentObjectTable::Get1NewResourcePointer(KG3DRepresentObject** ppT,
	DWORD dwType,
	DWORD_PTR )
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(ppT);

	*ppT = NULL;
	switch(dwType)
    {
    case REPRESENTOBJECT_DEFAULT:
        (*ppT) = new KG3DRepresentObject();
        break;
    case REPRESENTOBJECT_NPC:
        (*ppT) = new KG3DRepresentNPC();
        break;
	case REPRESENTOBJECT_DOODAD:
		(*ppT) = new KG3DRepresentDoodad();
		break;
	case REPRESENTOBJECT_WAYPOINT:
		(*ppT) = new KG3DRepresentWayPoint();
		break;
	case REPRESENTOBJECT_REFRESHPOINT:
		(*ppT) = new KG3DRepresentRefreshPoint();
		break;
	case REPRESENTOBJECT_SET:
		(*ppT) = new KG3DRepresentObjectSet();
		break;
    case REPRESENTOBJECT_PROP:
    case REPRESENTOBJECT_MAGIC:
    case REPRESENTOBJECT_EFFECT:
    case REPRESENTOBJECT_SKILL:
        (*ppT) = new KG3DRepresentObject();
        break;
	case REPRESENTOBJECT_VISIBLESET:
		(*ppT) = new KG3DRepresentObjectVisibleEntitySet();
		break;
	case REPRESENTOBJECT_POLY:
		(*ppT) = new KG3DRepresentPoly();
		break;
	case REPRESENTOBJECT_SOUNDBALL:
		(*ppT) = new KG3DRepresentObjectSoundBall();
		break;
	case REPRESENTOBJECT_PVS:
		(*ppT) = new KG3DRepresentObjectPVS();
		break;
	case REPRESENTOBJECT_ANTIPORTAL:
		(*ppT) = new KG3DRepresentObjectAntiPortal();
		break;
	case REPRESENTOBJECT_NODE:
		(*ppT) = new KG3DRepresentObjectNode();
		break;
	case REPRESENTOBJECT_TERRAINBLOCK:
		(*ppT) = new KG3DRepresentTerrainBlock();
		break;
	case REPRESENTOBJECT_DATAFLOWDATA:
		break;
	case REPRESENTOBJECT_DATAFLOWPROCESSOR:
		(*ppT) = new KG3DRepresentObjectDataFlowProcessor();
		break;
	case REPRESENTOBJECT_DATAFLOWARC:
		(*ppT) = new KG3DRepresentObjectDataFlowArc();
		break;
	case REPRESENTOBJECT_TRAFFICPOINT:
		(*ppT) = new KG3DRepresentTrafficPoint();
		break;
	case REPRESENTOBJECT_WALL:
		(*ppT) = new KG3DRepresentObjectWall();
		break;
    default:
		{
			//用注册的创建函数New出Object
			_ASSERTE(dwType < REPRESENTOBJECT_TYPE_COUNT);
			KG_PROCESS_ERROR(dwType < REPRESENTOBJECT_TYPE_COUNT);
			TypeFuncRepresentObjBuilder Builder = m_vecBuilder[dwType];
			KG_PROCESS_ERROR(Builder);
			(*ppT) = (*Builder)(NULL, NULL);
		}
    }

	KG_PROCESS_ERROR(*ppT);

	(*ppT)->SetType(dwType);//这句话应该在前面
	(*ppT)->Init();
	

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DRepresentObjectTable::GetCopy(
    KG3DRepresentObject** ppOut,KG3DRepresentObject* pIn
)
{
	if(!pIn)
		return E_FAIL;

	KG3DRepresentObject* pObject = NULL;
	HRESULT hr = Get1NewResource(&pObject,pIn->GetType(),0);
	KG_COM_PROCESS_ERROR(hr);
	hr = pObject->Copy(pIn);
	KG_COM_PROCESS_ERROR(hr);
//////////add by huangjinshou 2008-03-18//////////////////////////////////////////////////
	if (pIn->GetType() == REPRESENTOBJECT_NODE)
	{
		DWORD dwNodeType = static_cast<KG3DRepresentObjectNode*>(pIn)->GetNodeType();
		static_cast<KG3DRepresentObjectNode*>(pObject)->SetNodeType(dwNodeType);
	}
/////////////////////////////////////////////////////////////////////////////////////////
	(*ppOut) = pObject;
	return S_OK;
Exit0:
	SAFE_RELEASE(pObject);
	return E_FAIL;
}

HRESULT KG3DRepresentObjectTable::RegisterType( DWORD dwType, TypeFuncRepresentObjBuilder Builder )
{
	_ASSERTE(REPRESENTOBJECT_TYPE_COUNT == m_vecBuilder.size());
	KG_PROCESS_ERROR(dwType < REPRESENTOBJECT_TYPE_COUNT && Builder != NULL);
	{
		KG_PROCESS_ERROR(m_vecBuilder[dwType] == NULL 
			|| m_vecBuilder[dwType] == Builder);//如果已经有另外一个注册的生成器，那么必须先反注册
		m_vecBuilder[dwType] = Builder;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectTable::UnRegisterType( DWORD dwType, TypeFuncRepresentObjBuilder* pTheUnregisteredBuilder )
{
	KG_PROCESS_ERROR(dwType < REPRESENTOBJECT_TYPE_COUNT);
	if(pTheUnregisteredBuilder)
		*pTheUnregisteredBuilder = m_vecBuilder[dwType];
	m_vecBuilder[dwType] = NULL;
	return S_OK;
Exit0:
	return E_FAIL;
}



HRESULT KG3DRepresentObjectTable::Init()
{
	HRESULT hr = TypeBaseClass::Init();

	static TypeInfo stTypeInfo[] = 
	{
		{_T(".mesh"), REPRESENTOBJECT_DEFAULT, TRUE},
		{_T(".modelset"), REPRESENTOBJECT_SET, TRUE},
		{_T(".soundball"), REPRESENTOBJECT_SOUNDBALL, TRUE},
		{_T(".mdl"), REPRESENTOBJECT_DEFAULT, TRUE},
		{_T(".sfx"), REPRESENTOBJECT_DEFAULT, TRUE},		
		{_T(".walldefine"), REPRESENTOBJECT_WALL, TRUE},
        {_T(".pvs"), REPRESENTOBJECT_PVS, TRUE},
	};
	m_RefTable = stTypeInfo;
	m_RefTableSize = _countof(stTypeInfo);

	//////////////////////////////////////////////////////////////////////////

	return hr;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DRepresentObjectTable)
{
	class KG3DRepresentObjectTest : public KG3DRepresentObject 
	{
	public:
		DWORD A;
		KG3DRepresentObjectTest():A(0x12345678){}
		virtual ULONG STDMETHODCALLTYPE Release(){delete this; return 0;}
		static KG3DRepresentObject* Build(DWORD_PTR, DWORD_PTR){return new KG3DRepresentObjectTest();}
	};

	HRESULT hr = g_GetObjectTable().RegisterType(REPRESENTOBJECT_TEST, &KG3DRepresentObjectTest::Build);
	_ASSERTE(SUCCEEDED(hr));
	KG3DRepresentObject* pObj = NULL;
	hr = g_GetObjectTable().Get1NewResource(&pObj, REPRESENTOBJECT_TEST, NULL);
	_ASSERTE(SUCCEEDED(hr));
	KG3DRepresentObjectTest* pTest = dynamic_cast<KG3DRepresentObjectTest*>(pObj);
	_ASSERTE(pTest);
	_ASSERTE(pTest->A == 0x12345678);
	SAFE_RELEASE(pTest);
	hr = g_GetObjectTable().UnRegisterType(REPRESENTOBJECT_TEST, NULL);
	_ASSERTE(SUCCEEDED(hr));
	hr = g_GetObjectTable().Get1NewResource(&pObj, REPRESENTOBJECT_TEST, NULL);
	_ASSERTE(FAILED(hr));

	//_KG3D_DEBUG_REPORT("KG3DRepresentObjectTable Runtime Test Passed\n");
}
KG_TEST_END(KG3DRepresentObjectTable)
#endif