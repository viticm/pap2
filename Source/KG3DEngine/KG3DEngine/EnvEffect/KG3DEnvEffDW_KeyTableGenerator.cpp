////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTableGenerator.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-25 16:11:52
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DEnvEffDW_KeyTableGenerator.h"
#include "KG3DEnvEffDW_KeyBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DEnvEffDW_KeyTableGenerator)
		{
			static INT LocalSTCount;
			struct TypeKey : public KG3DDWGC_KeyDummy
			{
				enum{DWGCKEY_TYPE = KG3DTYPE_ENVEFF_RAIN,};
				DWORD m_dwModificationOption;
				INT m_Region;
				INT m_Hour;

				INT m_NextHour;

				TypeKey():m_Region(0),m_Hour(0),m_NextHour(0){}
				~TypeKey(){}
				TypeKey(const TypeKey& Another):m_Region(Another.m_Region),m_Hour(Another.m_Hour),m_NextHour(Another.m_NextHour){}

				virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent)
				{
					const TypeKey* pPrev = dynamic_cast<const TypeKey*>(&BaseKeyBefore);
					const TypeKey* pNext = dynamic_cast<const TypeKey*>(&BaseKeyAfter);
					_ASSERTE(pPrev && pNext);
					_ASSERTE(pPrev->m_Region == pNext->m_Region);
					m_Region = pPrev->m_Region;
					m_Hour = pPrev->m_Hour;
					m_NextHour = pNext->m_Hour;
					return true;
				}

			};

			struct TypeTempTable : public  TKG3DDWGC_KeyTable<TypeKey>
			{
				TypeTempTable(TKG3DDWGC_KeyTable<TypeKey>::key_alloc& Alloc):TKG3DDWGC_KeyTable<TypeKey>(Alloc){++LocalSTCount;}
				~TypeTempTable(){--LocalSTCount;}
				//无拷贝
			};

			typedef TKG3DDWGC_KeyTableGenerator<TypeTempTable> TypeTableGen;
			KG3D_CUSTOM_HELPERS::TKG3DLifeTimeDependedPointer<TypeTableGen*> LifeP;
			_ASSERTE(! LifeP);
			{
				LocalSTCount = 0;
				TypeTableGen TempTableGenerator;
				std::vector<KG3DDWGC_KeyTableBase*> vecTemp;
				//new
				{
					vecTemp.push_back(TempTableGenerator.RequestANewTable());
					vecTemp.push_back(TempTableGenerator.RequestANewTable());
					vecTemp.push_back(TempTableGenerator.RequestANewTable());
					vecTemp.push_back(TempTableGenerator.RequestANewTable());
					vecTemp.push_back(TempTableGenerator.RequestANewTable());
					_ASSERTE(TempTableGenerator.Size() == 5);
				}
				_ASSERTE(LocalSTCount == 5);
				//Release
				{
					_ASSERTE(5 == vecTemp.size());
					TempTableGenerator.ReleaseTable(vecTemp[3]);
					TempTableGenerator.ReleaseTable(vecTemp[4]);

					vecTemp.erase(vecTemp.begin() + 3);
					vecTemp.erase(vecTemp.begin() + 3);

					_ASSERTE(TempTableGenerator.Size() == 3);
					_ASSERTE(LocalSTCount == 3);
				}
				//Traverse
				{
					for (size_t i = 0; i < TempTableGenerator.Size(); ++i)
					{
						_ASSERTE(TempTableGenerator.GetTable(i).GetType() == TypeKey::DWGCKEY_TYPE);
					}
				}
				//LifDependent
				{
					LifeP = &TempTableGenerator;
					_ASSERTE(LifeP);
				}
				//Sort
				
				{
					//先打乱VecTemp中的顺序
					_ASSERTE(3 == vecTemp.size());
					_ASSERTE(TempTableGenerator.Size() == 3);

					KG3DDWGC_KeyTableBase* pTemp = vecTemp[0];
					vecTemp[0] = vecTemp[2];
					vecTemp[2] = pTemp;

					TempTableGenerator.SortTables(vecTemp);

					for (size_t i = 0; i < TempTableGenerator.Size(); ++i)
					{
						_ASSERTE(&TempTableGenerator.GetTable(i) == vecTemp[i]);
					}
				}
			}
			_ASSERTE(! LifeP);
			//_KG3D_DEBUG_REPORT("KG3DDWGC_KeyTableGenerator RunTime Tests Passed, Congratulations!");
		}
KG_TEST_END(KG3DEnvEffDW_KeyTableGenerator)
#endif
};
////////////////////////////////////////////////////////////////////////////////

