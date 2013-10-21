////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTable.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-17 17:12:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KG3DEnvEffDW_KeyTable.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
#if defined(KG_ENABLE_TDD)
	static int LocalSTCount = -1;
	KG_TEST_BEGIN(KG3DEnvEffDW_KeyTable)
	{
		struct TypeKey : public KG3DDWGC_KeyDummy
		{
			enum{DWGCKEY_TYPE = KG3DTYPE_ENVEFF_RAIN,};
			DWORD m_dwModificationOption;
			INT m_Region;
			INT m_Hour;

			INT m_NextHour;

			TypeKey():m_Region(0),m_Hour(0),m_NextHour(0){++LocalSTCount;}
			~TypeKey(){--LocalSTCount;}
			TypeKey(const TypeKey& Another):m_Region(Another.m_Region),m_Hour(Another.m_Hour),m_NextHour(Another.m_NextHour){++LocalSTCount;}

			virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore
				, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent)
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
		struct RegionHour 
		{
			int Region;
			int Hour;
		};

		RegionHour RegionHourStruct[] = 
		{
			{1, 1}, {1, 2}, {1, 12},{1, 24},
			{127, 2}, {127, 3}, {127, 4},{127, 5},
			{255, 6}, {255, 7}, {255, 8},{255, 24},				
		};

		//BlackBox

		do
		{
			TKG3DDWGC_KeyTable<TypeKey>::key_alloc	Alloc;
			TKG3DDWGC_KeyTable<TypeKey>	TempTable(Alloc);
			_ASSERTE(LocalSTCount == 0);
			_ASSERTE(TempTable.Size() == 0);
			_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());
			DWORD dwTempTableType = TempTable.GetType();
			_ASSERTE(dwTempTableType == TKG3DDWGC_KeyTable<TypeKey>::DWGCKEY_TYPE);

			//case 0
			TempTable.TestPrivateFunctions();

			//case1	//New
			{
				TypeKey*  pKey = NULL;
				for (size_t i = 0; i < _countof(RegionHourStruct); ++i)
				{
					pKey = dynamic_cast<TypeKey*>(
						TempTable.New(
						KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)
						)
						);
					_ASSERTE(pKey);
					pKey->m_Region = RegionHourStruct[i].Region;
					pKey->m_Hour = RegionHourStruct[i].Hour;
				}
				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());
				_ASSERTE(TempTable.GetKeyCountOfRegion(1) == 4);
				_ASSERTE(TempTable.GetKeyCountOfRegion(127) == 4);
				_ASSERTE(TempTable.GetKeyCountOfRegion(255) == 4);
			}
			_ASSERTE(LocalSTCount == _countof(RegionHourStruct));
			//GetRegion,GetPrev
			{
				KG3DENVEFF_DWGCKEY_HANDLE KeyHandle;

				//检查第一行
				HRESULT hr = TempTable.GetFirstKeyHandleOfRegion(1, &KeyHandle, NULL);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(1, 1));

				hr = TempTable.GetFirstKeyHandleOfRegion(128, &KeyHandle, NULL);
				_ASSERTE(FAILED(hr));
				hr = TempTable.GetFirstKeyHandleOfRegion(256, &KeyHandle, NULL);
				_ASSERTE(FAILED(hr));

				hr = TempTable.GetNextKeyHandleOfRegion(1, KeyHandle, &KeyHandle, NULL);
				hr = TempTable.GetNextKeyHandleOfRegion(1, KeyHandle, &KeyHandle, NULL);
				hr = TempTable.GetNextKeyHandleOfRegion(1, KeyHandle, &KeyHandle, NULL);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(1, 24));
				hr = TempTable.GetNextKeyHandleOfRegion(1, KeyHandle, &KeyHandle, NULL);
				_ASSERTE(FAILED(hr));

				//检查最后行
				hr = TempTable.GetFirstKeyHandleOfRegion(255, &KeyHandle, NULL);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(255, 6));
				hr = TempTable.GetNextKeyHandleOfRegion(255, KeyHandle, &KeyHandle, NULL);
				hr = TempTable.GetNextKeyHandleOfRegion(255, KeyHandle, &KeyHandle, NULL);
				hr = TempTable.GetNextKeyHandleOfRegion(255, KeyHandle, &KeyHandle, NULL);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(255, 24));
				hr = TempTable.GetNextKeyHandleOfRegion(255, KeyHandle, &KeyHandle, NULL);
				_ASSERTE(FAILED(hr));

				//得到之前的
				/*
				RegionHour RegionHourStruct[] = 
				{
				{1, 1}, {1, 2}, {1, 12},{1, 24},
				{127, 2}, {127, 3}, {127, 4},{127, 5},
				{255, 6}, {255, 7}, {255, 8},{255, 24},				
				};
				*/

				hr = TempTable.GetPrevKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE(1,1), &KeyHandle);
				_ASSERTE(FAILED(hr));
				hr = TempTable.GetPrevKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE(1,2), &KeyHandle);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(1,1));
				hr = TempTable.GetPrevKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE(255,6), &KeyHandle);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(KeyHandle == KG3DENVEFF_DWGCKEY_HANDLE(127,5));
			}
			//Del
			{
				for (size_t i = _countof(RegionHourStruct)-1; i >= 4; --i)
				{
					HRESULT hr = TempTable.Del(
						KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)
						);
					_ASSERTE(SUCCEEDED(hr));
					_ASSERTE(TempTable.Size() == i);
				}
				_ASSERTE(TempTable.Size() == 4);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());
			}
			_ASSERTE(LocalSTCount == 4);
			//Get
			{
				for (size_t i = 0; i < 4; ++i)
				{
					TypeKey* pKey = dynamic_cast<TypeKey*>(TempTable.Get(
						KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)
						));
					_ASSERTE(pKey->m_Region == RegionHourStruct[i].Region);
					_ASSERTE(pKey->m_Hour == RegionHourStruct[i].Hour);
				}
			}
			//Clear
			{
				TempTable.Clear();
				_ASSERTE(TempTable.Size() == 0);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());
			}
			_ASSERTE(LocalSTCount == 0);
			//case5	//遍历
			{
				TempTable.Clear();
				TypeKey*  pKey = NULL;
				for (size_t i = 0; i < _countof(RegionHourStruct); ++i)
				{
					pKey = dynamic_cast<TypeKey*>(
						TempTable.New(
						KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)
						)
						);
					_ASSERTE(pKey);
					pKey->m_Region = RegionHourStruct[i].Region;
					pKey->m_Hour = RegionHourStruct[i].Hour;
				}
				_ASSERTE(LocalSTCount == _countof(RegionHourStruct));

				KG3DENVEFF_DWGCKEY_HANDLE KeyHandle;
				size_t	handleInAlloc  = 0;
				size_t	uElemGetted = 0;
				std::vector<unsigned>	TempVecOfHandleInAlloc;
				HRESULT hr = TempTable.GetFirstKeyHandle(&KeyHandle, (LPVOID)&handleInAlloc);
				_ASSERTE(SUCCEEDED(hr));
				++uElemGetted;
				TempVecOfHandleInAlloc.push_back((unsigned)handleInAlloc);

				while (SUCCEEDED(TempTable.GetNextKeyHandle(KeyHandle, &KeyHandle, (LPVOID)&handleInAlloc)))
				{
					++uElemGetted;
					TempVecOfHandleInAlloc.push_back((unsigned)handleInAlloc);
				}

				_ASSERTE(TempVecOfHandleInAlloc.size() == TempTable.Size());

			}
			_ASSERTE(LocalSTCount == _countof(RegionHourStruct));
			//case6 DelRegion IsAnyKeyInRegion
			{
				_ASSERTE(TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(1,1)));
				HRESULT hr = TempTable.DelRegion(1);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(! TempTable.IsAnyKeyInRegion(1));

				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(1,1)));
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(1,255)));
				_ASSERTE(TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(255,24)));
				_ASSERTE(TempTable.Size() + 4 == _countof(RegionHourStruct));
				_ASSERTE(LocalSTCount == (int)TempTable.Size());
				_ASSERTE(TempTable.Size() == TempTable.GetAllocElemCount_Plus_RefElemCount());

				hr = TempTable.DelRegion(255);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(255,24)));
				_ASSERTE(TempTable.Size() == 4);
				_ASSERTE(LocalSTCount == (int)TempTable.Size());
				_ASSERTE(TempTable.Size() == TempTable.GetAllocElemCount_Plus_RefElemCount());
				_ASSERTE(! TempTable.IsAnyKeyInRegion(255));

				hr = TempTable.DelRegion(125);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(TempTable.Size() == 4);
				_ASSERTE(TempTable.IsAnyKeyInRegion(127));
			}
			//Case 7 MoveRegion
			{
				_ASSERTE(TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(255,24)));
				_ASSERTE(LocalSTCount == 5);

				HRESULT hr = TempTable.MoveAllKeysInRegionToNewRegion(127, 128);
				_ASSERTE(TempTable.Size() == 5);
				_ASSERTE(TempTable.Size() == TempTable.GetAllocElemCount_Plus_RefElemCount());
				_ASSERTE(TempTable.GetKeyCountOfRegion(128) == 4);
				_ASSERTE(TempTable.GetKeyCountOfRegion(127) == 0);

				hr = TempTable.MoveAllKeysInRegionToNewRegion(128, 255);
				_ASSERTE(FAILED(hr));
				_ASSERTE(LocalSTCount == 5);

				hr = TempTable.MoveAllKeysInRegionToNewRegion(128, 127);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(LocalSTCount == 5);
				_ASSERTE(TempTable.Size() == 5);
				_ASSERTE(TempTable.Size() == TempTable.GetAllocElemCount_Plus_RefElemCount());
				_ASSERTE(TempTable.GetKeyCountOfRegion(127) == 4);
			}
			//Last Case Clear
			{
				TempTable.Clear();
				_ASSERTE(TempTable.Size() == 0);
				_ASSERTE(TempTable.Size() == TempTable.GetAllocElemCount_Plus_RefElemCount());
			}
		}while(false);
		_ASSERTE(LocalSTCount == 0);
		//UinitTest
		do
		{
			TKG3DDWGC_KeyTable<TypeKey>::key_alloc	Alloc;
			TKG3DDWGC_KeyTable<TypeKey>	TempTable(Alloc);
			//New
			{
				TypeKey*  pKey = NULL;
				for (int i = _countof(RegionHourStruct) - 1; i >= 0 ; --i)
				{
					pKey = dynamic_cast<TypeKey*>(
						TempTable.New(
						KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)
						)
						);
					_ASSERTE(pKey);
					pKey->m_Region = RegionHourStruct[i].Region;
					pKey->m_Hour = RegionHourStruct[i].Hour;
				}
				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());

				_ASSERTE(! TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(1, 25)));
				_ASSERTE(! TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(1, 0)));
				_ASSERTE(! TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(0, 20)));
				_ASSERTE(! TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(256, 20)));
				_ASSERTE(! TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(256, 0)));
			}
			_ASSERTE(LocalSTCount == _countof(RegionHourStruct));
			//Get
			{
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(0,0)));
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(127,127)));
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(255,254)));
			}
			//Del
			{
				size_t oldSize = TempTable.Size();
				HRESULT hr = TempTable.Del(
					KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[0].Region, RegionHourStruct[0].Hour)
					);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(! TempTable.Get(KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[0].Region, RegionHourStruct[0].Hour)));
				_ASSERTE(oldSize == TempTable.Size() + 1);
			}
		}while(false);
		//GetInterpKey
		do
		{
			RegionHour RegionHourStruct[] = 
			{
				{1, 2},
				{127, 24},
				{128, 6},{128,24},
				{255, 5}, {255, 7}, {255, 8},{255, 23},				
			};

			TKG3DDWGC_KeyTable<TypeKey>::key_alloc	Alloc;
			TKG3DDWGC_KeyTable<TypeKey>	TempTable(Alloc);
			for (size_t i = 0; i < _countof(RegionHourStruct); ++i)
			{
				TypeKey* p = dynamic_cast<TypeKey*>(TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)));
				_ASSERTE(p);
				p->m_Region = RegionHourStruct[i].Region;
				p->m_Hour = RegionHourStruct[i].Hour;
			}
			_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));

			{
				TypeKey TempKey;
				HRESULT hr = TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(126, 1), TempKey);
				_ASSERTE(FAILED(hr));
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(255, 0), TempKey);
				_ASSERTE(FAILED(hr));
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(1234, 12), TempKey);
				_ASSERTE(FAILED(hr));

				//对行1检查
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(1, 1), TempKey);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(TempKey.m_Region == 1 && TempKey.m_Hour == 2 && TempKey.m_NextHour == 0);
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(1, 24), TempKey);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(TempKey.m_Region == 1 && TempKey.m_Hour == 2 && TempKey.m_NextHour == 0);
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(1, 12), TempKey);
				_ASSERTE(SUCCEEDED(hr));
				_ASSERTE(TempKey.m_Region == 1 && TempKey.m_Hour == 2 && TempKey.m_NextHour == 0);

				//对行2检查
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(127, 12), TempKey);
				_ASSERTE(TempKey.m_Region ==127 && TempKey.m_Hour == 24 && TempKey.m_NextHour == 0);

				//对行3检查
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(128, 1), TempKey);
				_ASSERTE(TempKey.m_Region == 128 && TempKey.m_Hour == 24 && TempKey.m_NextHour == 6);
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(128, 11), TempKey);
				_ASSERTE(TempKey.m_Region == 128 && TempKey.m_Hour == 6 && TempKey.m_NextHour == 24);


				//对行4检查
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(255, 1), TempKey);
				_ASSERTE(TempKey.m_Region == 255 && TempKey.m_Hour == 23 && TempKey.m_NextHour == 5);
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(255, 24), TempKey);
				_ASSERTE(TempKey.m_Region == 255 && TempKey.m_Hour == 23 && TempKey.m_NextHour == 5);

				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(255, 6), TempKey);
				_ASSERTE(TempKey.m_Region == 255 && TempKey.m_Hour == 5 && TempKey.m_NextHour == 7);
				hr =  TempTable.GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE(255, 22), TempKey);
				_ASSERTE(TempKey.m_Region == 255 && TempKey.m_Hour == 8 && TempKey.m_NextHour == 23);


			}

		}while(false);
		//CopyTo
		do{
			RegionHour RegionHourStruct[] = 
			{
				{1, 2},
				{127, 24},
				{128, 6},{128,24},
				{255, 5}, {255, 7}, {255, 8},{255, 23},				
			};

			//创建两个不同的Table
			TKG3DDWGC_KeyTable<TypeKey>::key_alloc	Alloc;
			TKG3DDWGC_KeyTable<TypeKey>	TempTable(Alloc);
			for (size_t i = 0; i < _countof(RegionHourStruct); ++i)
			{
				TypeKey* p = dynamic_cast<TypeKey*>(TempTable.New(KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)));
				_ASSERTE(p);
				p->m_Region = RegionHourStruct[i].Region;
				p->m_Hour = RegionHourStruct[i].Hour;
			}
			_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));
			_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size());
			_ASSERTE(LocalSTCount == (int)TempTable.Size());

			TKG3DDWGC_KeyTable<TypeKey>	DescTable(Alloc);
			for (size_t i = 0; i < _countof(RegionHourStruct) - 4; ++i)//DescTable没有最后一行
			{
				TypeKey* p = dynamic_cast<TypeKey*>(DescTable.New(KG3DENVEFF_DWGCKEY_HANDLE(RegionHourStruct[i].Region, RegionHourStruct[i].Hour)));
				_ASSERTE(p);
				p->m_Region = RegionHourStruct[i].Region;
				p->m_Hour = RegionHourStruct[i].Hour;
			}
			_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4);
			_ASSERTE(DescTable.GetAllocElemCount_Plus_RefElemCount() == DescTable.Size() + TempTable.Size());
			_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());

			//CopyByValue

			{
				HRESULT hr = E_FAIL;
				//保证失败
				{
					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TRUE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(256, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 5),TRUE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(256, 5),TRUE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 25),TRUE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 0),TRUE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(128, 6),TRUE);
					_ASSERTE(FAILED(hr));
				}


				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 6),TRUE);
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());

				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 6), DescTable, KG3DENVEFF_DWGCKEY_HANDLE(128, 6),TRUE);
				_ASSERTE(FAILED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());

				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 6), DescTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 6),TRUE);
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4 + 1);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());
			}

			//CopyByRef
			//先还原
			{
				HRESULT hr = E_FAIL;
				hr = TempTable.Del(KG3DENVEFF_DWGCKEY_HANDLE(255, 6));
				_ASSERTE(SUCCEEDED(hr));
				hr = DescTable.Del(KG3DENVEFF_DWGCKEY_HANDLE(255, 6));
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());

				//保证失败
				{
					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 5), FALSE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(256, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 5),FALSE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(256, 5),FALSE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 25),FALSE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 0),FALSE);
					_ASSERTE(FAILED(hr));

					hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(128, 6),FALSE);
					_ASSERTE(FAILED(hr));
				}

				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 5), TempTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 6),FALSE);
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size() - 1);//注意这里，因为有一个是引用的

				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 6), DescTable, KG3DENVEFF_DWGCKEY_HANDLE(128, 6), FALSE);
				_ASSERTE(FAILED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size() - 1);

				hr = TempTable.CopyTo(KG3DENVEFF_DWGCKEY_HANDLE(255, 6), DescTable, KG3DENVEFF_DWGCKEY_HANDLE(255, 6),FALSE);
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct) + 1);
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4 + 1);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size() - 2);//现在应该有两个引用的


				//再次还原
				hr = TempTable.Del(KG3DENVEFF_DWGCKEY_HANDLE(255, 6));
				_ASSERTE(SUCCEEDED(hr));
				hr = DescTable.Del(KG3DENVEFF_DWGCKEY_HANDLE(255, 6));
				_ASSERTE(SUCCEEDED(hr));

				_ASSERTE(TempTable.Size() == _countof(RegionHourStruct));
				_ASSERTE(TempTable.GetAllocElemCount_Plus_RefElemCount() == TempTable.Size() + DescTable.Size());
				_ASSERTE(DescTable.Size() == _countof(RegionHourStruct) - 4);
				_ASSERTE(LocalSTCount == (int)TempTable.Size() + (int)DescTable.Size());
			}



		}while(false);
		//_KG3D_DEBUG_REPORT("Dynamic Weather KeyTable RunTime Test Complete! Congratulations!\n");
	}
	KG_TEST_END(KG3DEnvEffDW_KeyTable)
#endif


};

////////////////////////////////////////////////////////////////////////////////

