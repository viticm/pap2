#include "stdafx.h"
#include "SceneEditor.h"
#include "KGCommonList.h"
//#include "KGCommonWidgets.h"
#include "KSceneSceneHelpers.h"
//#include "KSceneInterfaceGetters.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KSCENEEDITOR_HELPERCLASSES
{
	namespace COMMON_LIST
	{
		IMPLEMENT_DYNAMIC(KGCommonList, CListBox)

		BEGIN_MESSAGE_MAP(KGCommonList, CListBox)
		END_MESSAGE_MAP()

		VOID KGCommonList::SetManagerGetterParam( DWORD_PTR WParam, DWORD_PTR LParam )
		{
			if (m_pHelper)
			{
				m_pHelper->SetManagerGetterParam(WParam, LParam);
			}
		}
		KGCommonList::KGCommonList(const KGCommonListHelperBase& Helper)
			:m_pHelper(Helper.Clone())
		{

		}

		KGCommonList::~KGCommonList()
		{
			SAFE_DELETE(m_pHelper);
		}

		VOID KGCommonList::ReloadFuncBody()
		{
			if (!m_pHelper)
			{
				return;
			}
			this->ResetContent();
			INT nCount;
			CString TempString;
			
			BOOL bRet = m_pHelper->Reset();
			KG_PROCESS_ERROR(bRet);
			
			nCount = m_pHelper->GetCount();			
			KG_PROCESS_ERROR(nCount>=0);
			for (int i = 0; i < nCount; i++)
			{
				BOOL bRet = m_pHelper->GetName(i, TempString);
				
				if (!bRet)
				{				
					KG_CUSTOM_HELPERS::TValueToString(i, TempString);
				}
				
				int nAddedIndex = this->AddString((LPCTSTR)TempString);
				if (nAddedIndex >= 0)
				{
					if (bRet)
					{
						this->SetItemData(nAddedIndex, i);///只存放Index，以后随时可以用GetMemberData来重新取得数据
					}
				}
			}
			if (this->GetCount() > 0)
			{
				this->SetCurSel(0);
			}
Exit0:
			return;
		}
		
		/*
		REload()
				{
					VOID* pManager = m_Helper.GetManager();
					KG3DENVEFF_INFO EnvEffInfoArray[KG3DENVEFF_MAX];
					KG_PROCESS_ERROR(pEnv);
		
					{
						INT nElementCount = 0;
						KG_COM_PROCESS_ERROR(pEnv->GetALLExistEffect(EnvEffInfoArray, _countof(EnvEffInfoArray), &nElementCount));
		
						m_EffList.ResetContent();
						CString TempString;
						INT nAddedEnvEff = 0;
						for (int i = 0; i < nElementCount; i++)
						{
							/ *
							IKG3DRTTIType* pTempRTTIType = NULL;
							IKG3DRTTITypeDescStruct TempRTTIInfo; 
							if (SUCCEEDED(g_pKG3DRTTITypeManager->GetRTTIType(EnvEffInfoArray[i].m_dwType, &pTempRTTIType)))
							{
							_ASSERTE(pTempRTTIType);
							pTempRTTIType->GetInfo(&TempRTTIInfo);* /
							LPCTSTR tczTemp = KSCENEEDITOR_HELPERCLASSES::GetRTTITypeName(EnvEffInfoArray[i].m_dwType);
							if (!tczTemp)
							{
								::OutputDebugString(_STRINGER(KSceneSettingPageEnvironment::ReloadList)":无法得到Effect名字\n");
								continue;
							}
							for (int nInnerLoop = 0; nInnerLoop < EnvEffInfoArray[i].m_nCount && nAddedEnvEff <= _countof(m_EnvEffDescArray); nInnerLoop++)
							{
								TempString.Format(_T("%s\t%d"), tczTemp, nInnerLoop);
								int nAddedIndex = m_EffList.AddString((LPCTSTR)TempString);
								if (nAddedIndex >= 0)
								{
									m_EnvEffDescArray[nAddedEnvEff].m_dwType = EnvEffInfoArray[i].m_dwType;
									m_EnvEffDescArray[nAddedEnvEff].m_nIndex = nInnerLoop;
									m_EffList.SetItemData(nAddedIndex, reinterpret_cast<DWORD_PTR>(&m_EnvEffDescArray[nAddedEnvEff]));
									nAddedEnvEff++;
								}
							}
		
							/ *}* /
		
						}
						m_EffList.SetCurSel(0);
						UpdataInterface();
					}
				}*/
		
	};///COMMON_LIST
};///KSCENEEDITOR_HELPERCLASSES