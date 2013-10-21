////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffWind.cpp
//  Version     : 1.0
//  Creator     : Li Kan
//  Create Date : 2007-1-15 10:55:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DEnvEffWind.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	KG3DEnvEffWind::KG3DEnvEffWind()
		:KG3DEnvEffect(0,KG3DTYPE_ENVEFF_WIND)
	{

	}

	KG3DEnvEffWind::~KG3DEnvEffWind()
	{

	}

	HRESULT KG3DEnvEffWind::Render( const KG3DEnvEffRenderContext& renderContext )
	{
		return S_OK;
	}

	HRESULT KG3DEnvEffWind::FrameMove()
	{
		return S_OK;
	}

	HRESULT KG3DEnvEffWind::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{

		for(map<int, KG3DWIND_DESCSTRUCT>::iterator idd = m_Wind.begin(); idd != m_Wind.end(); idd ++)
		{
            const int &key = idd->first;
			const KG3DWIND_DESCSTRUCT &val = idd->second;
			TCHAR _key[128];
			sprintf_s(_key,"%d", key);
			TCHAR _val[128];
			sprintf_s(_val, "Wind_%d", key, val.m_id);
			pIniFile->WriteString("Wind", _key, _val);
		}

		for(map<int, KG3DWIND_DESCSTRUCT>::iterator idd = m_Wind.begin(); idd != m_Wind.end(); idd ++)
		{
			KG3DWIND_DESCSTRUCT &val = idd->second;
			TCHAR _key[128];
			sprintf_s(_key, _countof(_key), "Wind_%d",val.m_id);
			pIniFile->WriteInteger(_key, "WindId", val.m_id);
			pIniFile->WriteFloat(_key,"WindDirX", val.m_WindDirX);
			pIniFile->WriteFloat(_key,"WindDirY", val.m_WindDirY);
			pIniFile->WriteFloat(_key,"WindDirZ", val.m_WindDirZ);
			pIniFile->WriteInteger(_key,"VertSway", val.m_Sway);
			pIniFile->WriteFloat(_key,"WindAmplitude1", val.m_WindAmplitude1);
			pIniFile->WriteFloat(_key,"WindAmplitude2", val.m_WindAmplitude2);
			pIniFile->WriteFloat(_key,"WindWaveLen1", val.m_WindWaveLen1);
			pIniFile->WriteFloat(_key,"WindWaveLen2", val.m_WindWaveLen2);
			pIniFile->WriteFloat(_key,"WindSpeed1", val.m_WindSpeed1);
			pIniFile->WriteFloat(_key,"WindSpeed2", val.m_WindSpeed2);
			pIniFile->WriteFloat(_key,"WindWaveScaleUp1", val.m_WindWaveScaleUp1);
			pIniFile->WriteFloat(_key,"WindWaveScaleUp2", val.m_WindWaveScaleUp2);
			pIniFile->WriteFloat(_key,"WindWaveScaleDown1", val.m_WindWaveScaleDown1);
			pIniFile->WriteFloat(_key,"WindWaveScaleDown2", val.m_WindWaveScaleDown2);
			pIniFile->WriteFloat(_key,"WindWaveOffset1", val.m_WindWaveOffset1);
			pIniFile->WriteFloat(_key,"WindWaveOffset2", val.m_WindWaveOffset2);
			pIniFile->WriteInteger(_key,"bEnableWave1",  val.m_bEnableWave1?1:0);
			pIniFile->WriteInteger(_key,"bEnableWave2",  val.m_bEnableWave2?1:0);
			pIniFile->WriteInteger(_key,"bEnableDirWind",  val.m_bEnableDirWind?1:0);
		}
		return S_OK;
	}

	HRESULT KG3DEnvEffWind::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		if(pIniFile == NULL)
		{
			return S_OK;
		}

		TCHAR keybuf[128];
		TCHAR valbuf[128];
		//ZeroMemory(keybuf, _countof(keybuf));
		vector<string> sections;

		if(pIniFile->IsSectionExist("Wind"))
		{		
		    //读取first
			if(pIniFile->GetNextKey("Wind","", keybuf))
			{
			   pIniFile->GetString("Wind",keybuf,"",valbuf, _countof(valbuf));
			   sections.push_back(valbuf);
               while(pIniFile->GetNextKey("Wind", keybuf, keybuf))
			   {
				   pIniFile->GetString("Wind",keybuf,"",valbuf, _countof(valbuf));
				   sections.push_back(valbuf);                 
			   }
			}
		}

		

		for(vector<string>::iterator it = sections.begin(); it!= sections.end(); it ++)
		{
		  KG3DWIND_DESCSTRUCT wind;
		  wind.SetDefaultValue();
		  const char * section = it->c_str();
		  pIniFile->GetInteger(section,"WindId", 0, &wind.m_id);
		  pIniFile->GetFloat(section,"WindDirX", 1.0f, &wind.m_WindDirX);
		  pIniFile->GetFloat(section,"WindDirY", 1.0f, &wind.m_WindDirY);
		  pIniFile->GetFloat(section,"WindDirZ", 1.0f, &wind.m_WindDirZ);
		  pIniFile->GetInteger(section, "VertSway", 0,  &wind.m_Sway);
		  pIniFile->GetFloat(section,"WindAmplitude1", 0.5f, &wind.m_WindAmplitude1);
		  pIniFile->GetFloat(section,"WindAmplitude2", 0.8f, &wind.m_WindAmplitude2);
		  pIniFile->GetFloat(section,"WindWaveLen1", 1000.0f,&wind.m_WindWaveLen1);
		  pIniFile->GetFloat(section,"WindWaveLen2", 500.f, &wind.m_WindWaveLen2);
		  pIniFile->GetFloat(section,"WindSpeed1", 1.0f, &wind.m_WindSpeed1);
		  pIniFile->GetFloat(section,"WindSpeed2", 2.0f, &wind.m_WindSpeed2);
		  pIniFile->GetFloat(section,"WindWaveScaleUp1", 1.0f, &wind.m_WindWaveScaleUp1);
		  pIniFile->GetFloat(section,"WindWaveScaleUp2", 1.0f, &wind.m_WindWaveScaleUp2);
		  pIniFile->GetFloat(section,"WindWaveScaleDown1", 0.1f, &wind.m_WindWaveScaleDown1);
		  pIniFile->GetFloat(section,"WindWaveScaleDown2", 0.1f, &wind.m_WindWaveScaleDown2);
		  pIniFile->GetFloat(section,"WindWaveOffset1", 0.0f, &wind.m_WindWaveOffset1);
		  pIniFile->GetFloat(section,"WindWaveOffset2", 0.0f, &wind.m_WindWaveOffset2);
		  int _boolbuf = 0;
		  pIniFile->GetBool(section,"bEnableWave1",  &_boolbuf);
		  wind.m_bEnableWave1 = _boolbuf ? true:false;
		  pIniFile->GetBool(section,"bEnableWave2",  &_boolbuf);
		  wind.m_bEnableWave2 = _boolbuf ? true:false;
		  pIniFile->GetBool(section,"bEnableDirWind", &_boolbuf);
		  wind.m_bEnableDirWind = _boolbuf ? true:false;
		  RequestModification(&wind);
		}
		return S_OK;
	}

	HRESULT KG3DEnvEffWind::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (NULL != pEffectInterface)
		{
			IEKG3DEnvEffWind* p = this;
			*pEffectInterface = p;
			return S_OK;
		}
		return E_FAIL;
	}


	HRESULT KG3DEnvEffWind::RequestModification( const KG3DWIND_DESCSTRUCT* pStruct )
	{

	  if(!pStruct)
		  return E_FAIL;

	  DWORD id = pStruct->m_id;
	  map<int, KG3DWIND_DESCSTRUCT>::iterator it = m_Wind.find(id);
	  if(it == m_Wind.end())
	  {
		  //找不到就新增
		  if(pStruct->m_id != -1)
		  {
			  //已经有编号了
			  m_Wind[pStruct->m_id] = *pStruct; //新增
			  return S_OK;
		  }
		  else
		  {
			  //自动分配编号
			  int maxid = -1;
			  for(map<int, KG3DWIND_DESCSTRUCT>::iterator idd = m_Wind.begin(); idd != m_Wind.end(); idd ++)
			  {
				int id = idd->first;
				if(id > maxid)
				{
				   maxid = id;
				}
			  }
			  const_cast<KG3DWIND_DESCSTRUCT*>(pStruct)->m_id = maxid + 1;
			  m_Wind[pStruct->m_id] = *pStruct; //新增
			  return S_OK;
		  }
	  }

	  KG3DWIND_DESCSTRUCT *wind = &it->second;

	  if(pStruct->m_dwMidificationOption == KG3DWIND_MDO_DEFAULT)
		  return S_OK;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_DIRX)
		  wind->m_WindDirX = pStruct->m_WindDirX;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_DIRY)
		  wind->m_WindDirY = pStruct->m_WindDirY;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_DIRZ)
		  wind->m_WindDirZ = pStruct->m_WindDirZ;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SWAY)
		  wind->m_Sway = pStruct->m_Sway;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_AMPLITUDE1)
		  wind->m_WindAmplitude1 = pStruct->m_WindAmplitude1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_AMPLITUDE2)
		  wind->m_WindAmplitude2 = pStruct->m_WindAmplitude2;
	  
	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_WAVELEN1)
		  wind->m_WindWaveLen1 = pStruct->m_WindWaveLen1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_WAVELEN2)
		  wind->m_WindWaveLen2 = pStruct->m_WindWaveLen2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SPEED1)
		  wind->m_WindSpeed1 = pStruct->m_WindSpeed1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SPEED2)
		  wind->m_WindSpeed2 = pStruct->m_WindSpeed2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SCALEUP1)
		  wind->m_WindWaveScaleUp1 = pStruct->m_WindWaveScaleUp1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SCALEUP2)
		  wind->m_WindWaveScaleUp2 = pStruct->m_WindWaveScaleUp2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SCALEDOWN1)
		  wind->m_WindWaveScaleDown1 = pStruct->m_WindWaveScaleDown1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_SCALEDOWN2)
		  wind->m_WindWaveScaleDown2 = pStruct->m_WindWaveScaleDown2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_OFFSET1)
		  wind->m_WindWaveOffset1 = pStruct->m_WindWaveOffset1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_OFFSET2)
		  wind->m_WindWaveOffset2 = pStruct->m_WindWaveOffset2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_ENABLEWAVE1)
		  wind->m_bEnableWave1 = pStruct->m_bEnableWave1;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_ENABLEWAVE2)
		  wind->m_bEnableWave2 = pStruct->m_bEnableWave2;

	  if(pStruct->m_dwMidificationOption & KG3DWIND_MDO_ENABLEDIRWIND)
		  wind->m_bEnableDirWind = pStruct->m_bEnableDirWind;

	  return S_OK;
	}


	HRESULT KG3DEnvEffWind::GetStruct( KG3DWIND_DESCSTRUCT* pResultStruct )
	{

		if(!pResultStruct)
			return E_FAIL;

		DWORD id = pResultStruct->m_id;
		map<int, KG3DWIND_DESCSTRUCT>::iterator it = m_Wind.find(id);
		if(it == m_Wind.end())
		{
            return E_FAIL;
		}
		else
		{		
			KG3DWIND_DESCSTRUCT *wind = &it->second;
			*pResultStruct = *wind;
		}

	    return S_OK;
	}

	void KG3DEnvEffWind::DoVist(IEKG3DEnvWindVistor &vistor)
	{
		for(map<int, KG3DWIND_DESCSTRUCT>::iterator it = m_Wind.begin(); it != m_Wind.end(); it ++)
		{
          KG3DWIND_DESCSTRUCT &wind = it->second;
		  vistor.vist(&wind);
		}
	}

	void KG3DEnvEffWind::DelWind(int windid)
	{
       m_Wind.erase(windid);
	}

};
