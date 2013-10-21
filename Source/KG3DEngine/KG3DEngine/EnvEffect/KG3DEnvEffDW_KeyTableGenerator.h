////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTableGenerator.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-25 16:11:36
/*	Comment     : 
主要是为了让KeyTable之间共享Alloc，这样就可以在不同KeyTable之间引用拷贝了。
另外Table要用Vector来保证和Effect之间的1，1对应关系，删除Effect的时候，删除的Table也是
对应的Table
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDW_KEYTABLEGENERATOR_H_
#define _INCLUDE_KG3DENVEFFDW_KEYTABLEGENERATOR_H_
#include "KG3DEnvEffDW_KeyTable.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	template<typename _TypeKeyTable>
	class TKG3DDWGC_KeyTableGenerator : public KG3DDWGC_KeyTableGeneratorBase
	{
		typedef _TypeKeyTable table_type;
		
		typedef typename table_type::key_alloc	key_alloc;
		typedef typename table_type::key_handle_type key_handle_type;
		typedef typename table_type::elem_type	elem_type;
		typedef std::vector<table_type*> tank_type;	//要用Vector来保证和Effect之间的1，1对应关系
		enum{DWGCKEY_TYPE = _TypeKeyTable::DWGCKEY_TYPE};

		tank_type	m_Tables;
		key_alloc	m_Alloc;

	protected:
		static const std::tstring&	GetSaveSectionName(){return table_type::GetSaveSectionName();}
	private:
		TKG3DDWGC_KeyTableGenerator(const TKG3DDWGC_KeyTableGenerator&);
		TKG3DDWGC_KeyTableGenerator& operator=(const TKG3DDWGC_KeyTableGenerator&);
		VOID DeleteAllTables()
		{
			for (tank_type::iterator it = m_Tables.begin(); it != m_Tables.end(); ++it)
			{
				_ASSERTE(*it);
				delete *it;
			}
			m_Tables.clear();
		}
	public:
		TKG3DDWGC_KeyTableGenerator(){}
		~TKG3DDWGC_KeyTableGenerator()
		{
			DeleteAllTables();
		}
		virtual VOID	SortTables(const std::vector<KG3DDWGC_KeyTableBase*>& VecOfAllTablesInEffects)
		{
			/*
			放在Effect的顺序可能和其对应的Table在TableGenerator中的顺序不一致
			调整TableGenerator，让其和Effect一致，这样读取的时候就可以11对应了
			*/
			_ASSERTE(VecOfAllTablesInEffects.size() <= m_Tables.size());
			for (size_t i = 0; i < VecOfAllTablesInEffects.size(); ++i)
			{
				table_type* pI = dynamic_cast<table_type*>(VecOfAllTablesInEffects[i]);
				_ASSERTE(pI);

				size_t PosOfTheTable = i;
				for (; PosOfTheTable < m_Tables.size(); ++PosOfTheTable)
				{
					if(m_Tables[PosOfTheTable] == pI)
						break;
				}

				_ASSERTE(PosOfTheTable < m_Tables.size());

				if (PosOfTheTable != i)
				{
					table_type* pTemp = m_Tables[i];
					m_Tables[i] = pI;
					m_Tables[PosOfTheTable] = pTemp;
				}		 
			}	
		}
		virtual bool ReleaseTable(KG3DDWGC_KeyTableBase* pTable)
		{
			table_type* p = dynamic_cast<table_type*>(pTable);
			if (p)
			{
				tank_type::iterator it = std::find(m_Tables.begin(), m_Tables.end(), p);
				if (it != m_Tables.end())
				{
					delete *it;
					m_Tables.erase(it);
					return true;
				}
			}
			return false;
		}
		virtual KG3DDWGC_KeyTableBase* RequestANewTable()
		{
			_ASSERTE(m_Tables.size() < KG3DENVEFF_MAX_SUBEFFECT);
			if(m_Tables.size() > KG3DENVEFF_MAX_SUBEFFECT - 1)
				return NULL;
			table_type* p = new table_type(m_Alloc);
			if (p)
				m_Tables.push_back(p);
			return p;
		}
		//用于遍历
		virtual size_t Size(){return m_Tables.size();}
		virtual KG3DDWGC_KeyTableBase& GetTable(size_t i){return *(m_Tables[i]);}
		virtual DWORD GetType(){return DWGCKEY_TYPE;}
		//读取
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR )
		{
			const std::tstring& tczSection = this->GetSaveSectionName();

			KG_CUSTOM_HELPERS::EraseSection(pIniFile, tczSection.c_str());
			TCHAR tczKeyName[MAX_PATH];
			ZeroMemory(tczKeyName, sizeof(tczKeyName));
			TCHAR tczKeyValue[MAX_PATH];
			ZeroMemory(tczKeyValue, sizeof(tczKeyValue));
			size_t NumInsertPos = 0;

			size_t AllocSavedElems = 0;

			//在KeyValue中预先填好前面的部分
			{
				const TCHAR* pKeyTypeName = g_cKG3DRTTITypeManager.GetName(DWGCKEY_TYPE);
				_ASSERTE(pKeyTypeName);
				_stprintf_s(tczKeyValue, "KG3DDWGC_Key_%s_", pKeyTypeName);
				size_t len = _tcslen(tczKeyValue);
				_ASSERTE(len > 0 && len < _countof(tczKeyValue) - std::numeric_limits<INT>::digits10 - 1);
				NumInsertPos = len;
				_ASSERTE(tczKeyValue[len] == '\0');
			}

			//保存Alloc里面的Key
			key_handle_type keyHandle = m_Alloc.get_first_handle();
			while (m_Alloc.is_valid_handle(keyHandle))
			{
				//得到KeyName
				{
#if defined(DEBUG) | defined(_DEBUG)
					BOOL bRet = 
#endif
						KG_CUSTOM_HELPERS::TValueToString(static_cast<UINT>(keyHandle), tczKeyName);
					_ASSERTE(bRet);
				}
				//得到KeyValue
				{
#if defined(DEBUG) | defined(_DEBUG)
					errno_t eRet = 
#endif
						_tcscpy_s(tczKeyValue + NumInsertPos, _countof(tczKeyValue) - NumInsertPos -1, tczKeyName);
					_ASSERTE(eRet == 0);
				}

				elem_type& elem = m_Alloc.get(keyHandle);
				_ASSERTE(! m_Alloc.is_error_elem(elem));
				if (elem.m_Key.SaveMapSetting(pIniFile, tczKeyValue))
				{
					pIniFile->WriteString(tczSection.c_str(), tczKeyName, tczKeyValue);//即 3 = KG3DDWGC_Key_3
					++AllocSavedElems;
				}
#if defined(DEBUG) | defined(_DEBUG)
				else
				{
					_KG3D_DEBUG_REPORT("存在的Struct却不能正确保存，要检查发生了什么");
				}
#endif
				keyHandle = m_Alloc.get_next_handle(keyHandle);
			}//end of while
			if (AllocSavedElems != m_Alloc.size())
			{
				_KG3D_DEBUG_OUTPUT1("动态天气KeyTableGenerator保存不完全成功, 类型：%u", this->GetType());
				KGLogPrintf(KGLOG_WARNING, "动态天气KeyTableGenerator保存不完全成功, 类型：%u", this->GetType());
			}
			
			//记录Alloc的大小和内含Table数
			KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tczSection.c_str(), _T("Alloc_Size"), (UINT)m_Alloc.size());
			KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tczSection.c_str(), _T("Table_Count"), (UINT)this->Size());
			
			//记录Table
			TCHAR TableSeperationBegin[MAX_PATH] = _T("");
			TCHAR TableSeperationEnd[MAX_PATH] = _T("");
			
			for (size_t i = 0; i < Size(); i++)
			{
				_stprintf_s(TableSeperationBegin, "TB%u_", i);
				KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tczSection.c_str(), TableSeperationBegin, _T(""));
				GetTable(i).SaveMapSetting(pIniFile, TableSeperationBegin);
				_stprintf_s(TableSeperationEnd, "E_TB%u_", i);
				KG_CUSTOM_HELPERS::SaveINIFileData(pIniFile, tczSection.c_str(), TableSeperationEnd, _T(""));
			}

			return S_OK;
		}
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection)
		{
			DeleteAllTables();

			UINT AllocSaveSize = 0;
			UINT TableCount = 0;
			_ASSERTE(this->Size() == 0);

			TCHAR tczKeyName[MAX_PATH];
			ZeroMemory(tczKeyName, sizeof(tczKeyName));
			TCHAR tczKeyValue[MAX_PATH];
			ZeroMemory(tczKeyValue, sizeof(tczKeyValue));

			typedef std::map<key_handle_type, key_handle_type> HandleHandleMap;
			HandleHandleMap handleMap;
			const std::tstring& tczSection = this->GetSaveSectionName();

			//读Alloc大小和内含的Table数
			bool bRet = KG_CUSTOM_HELPERS::LoadINIFileData<UINT>(pIniFile, tczSection.c_str(), _T("Alloc_Size"), 0, &AllocSaveSize);
			KG_PROCESS_ERROR(bRet);
			bRet = KG_CUSTOM_HELPERS::LoadINIFileData<UINT>(pIniFile, tczSection.c_str(), _T("Table_Count"), 0, &TableCount);
			KG_PROCESS_ERROR(bRet);
			_ASSERTE(TableCount <= KG3DENVEFF_MAX_SUBEFFECT);
			KG_PROCESS_ERROR(TableCount <= KG3DENVEFF_MAX_SUBEFFECT);
			for (UINT i = 0; i < TableCount; ++i)
			{
				this->RequestANewTable();
			}
			if(m_Tables.size() != TableCount)
			{
				DeleteAllTables();
				goto Exit0;
			}

			//读入Alloc
			*tczKeyName = '\0';
			while(KG_CUSTOM_HELPERS::GetNextKey(pIniFile, tczSection.c_str(), tczKeyName, tczKeyName))
			{
				if(0 == _tcscmp(tczKeyName, _T("Alloc_Size")))//用于分割
					break;
				if (! KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, tczSection.c_str(), tczKeyName, _T(""), tczKeyValue))
					continue;

				UINT KeyHandleOfTheKeyName = 0;
				BOOL bRet = KG_CUSTOM_HELPERS::TStringToValue(tczKeyName, KeyHandleOfTheKeyName);
				if(! bRet)
					continue;

				elem_type elem;
				elem.m_Ref = 0;//暂时还没有Ref
				if (! (elem.m_Key.LoadMapSetting(pIniFile, tczKeyValue)))
					continue;

				key_handle_type KeyHandle = m_Alloc.push(elem);
				if(! m_Alloc.is_valid_handle(KeyHandle))
					continue;				

				handleMap[KeyHandleOfTheKeyName] = KeyHandle;
			}

			KG_PROCESS_ERROR(AllocSaveSize == (UINT)m_Alloc.size());

			{
				//读入Table
				TCHAR TableSeperationBegin[MAX_PATH] = _T("");
				TCHAR TableSeperationEnd[MAX_PATH] = _T("");

				for (UINT i = 0; i <m_Tables.size(); i++)
				{
					_stprintf_s(TableSeperationBegin, "TB%u_", i);
					_stprintf_s(TableSeperationEnd, "E_TB%u_", i);
					table_type::load_struct LoadStruct;
					LoadStruct.pPreface = TableSeperationBegin;
					LoadStruct.pEnd = TableSeperationEnd;
					LoadStruct.pMapHandleHandle = &handleMap;
					
					GetTable(i).LoadMapSetting(pIniFile, reinterpret_cast<LPCTSTR>(&LoadStruct));
				}
			}


			return S_OK;
Exit0:
			_KG3D_DEBUG_OUTPUT1("动态天气KeyTableGenerator读取不完全成功, 类型：%d", this->GetType());
			KGLogPrintf(KGLOG_WARNING, "动态天气KeyTableGenerator读取不完全成功, 类型：%d", this->GetType());
			return E_FAIL;
		}
	};
	
};
#endif //_INCLUDE_KG3DENVEFFDW_KEYTABLEGENERATOR_H_
