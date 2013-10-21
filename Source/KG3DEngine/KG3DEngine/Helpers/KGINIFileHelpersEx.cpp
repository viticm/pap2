////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGINIFileHelpersEx.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-12-6 14:25:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGINIFileHelpersEx.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define FALSE_ASSERT_RETURN(condition)	\
{	\
	if(!(condition))	\
	return false;	\
}NULL

////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	//D3DCOLORVALUE
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, const D3DCOLORVALUE& DataRet )
	{
		FALSE_ASSERT_RETURN( pIniFile && szSection && szKeyName);
		D3DCOLOR dwColorToWrite = D3DCOLOR_COLORVALUE(DataRet.r, DataRet.g, DataRet.b, DataRet.a);
		return SaveINIFileHelper(pIniFile, szSection, szKeyName, dwColorToWrite);
	}
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DCOLORVALUE* DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile && szSection && szKeyName && DataRet);
		D3DCOLOR dwGetted = 0;
		bool bRet = LoadINIFileHelper(pIniFile, szSection, szKeyName, 0, &dwGetted);
		if (bRet)
		{
			D3DCOLORVALUE nColor;
			nColor.a = ((float)(dwGetted>>24))/255.0f;
			nColor.r = ((float)((dwGetted&0x00ff0000)>>16))/255.0f;
			nColor.g = ((float)((dwGetted&0x0000ff00)>>8))/255.0f;
			nColor.b = ((float)((dwGetted&0x000000ff)))/255.0f;
			*DataRet = nColor;
		}
		return bRet;
	}
	//D3DVECTOR
	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, const D3DVECTOR& DataRet )
	{
		FALSE_ASSERT_RETURN( pIniFile && szSection && szKeyName);
		_KG3D_DEBUG_STATIC_CHECK_T(sizeof(D3DVECTOR) == sizeof(float)*3, SizeNotMatch);
		return pIniFile->WriteMultiFloat(szSection, szKeyName, (float*)&DataRet, 3) ? true : false;
	}
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DVECTOR* DataRet )
	{
		FALSE_ASSERT_RETURN(pIniFile && szSection && szKeyName && DataRet);
		_KG3D_DEBUG_STATIC_CHECK_T(sizeof(D3DVECTOR) == sizeof(float)*3, SizeNotMatch);
		return pIniFile->GetMultiFloat(szSection, szKeyName, (float*)DataRet, 3) ? true : false;
	}

	bool Private::SaveINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, const D3DLIGHT9& DataRet )
	{
		/*D3DLIGHT9
		D3DLIGHTTYPE    Type;            / * Type of light source * /
				D3DCOLORVALUE   Diffuse;         / * Diffuse color of light * /
				D3DCOLORVALUE   Specular;        / * Specular color of light * /
				D3DCOLORVALUE   Ambient;         / * Ambient color of light * /
				D3DVECTOR       Position;         / * Position in world space * /
				D3DVECTOR       Direction;        / * Direction in world space * /
				float           Range;            / * Cutoff range * /
				float           Falloff;          / * Falloff * /
				float           Attenuation0;     / * Constant attenuation * /
				float           Attenuation1;     / * Linear attenuation * /
				float           Attenuation2;     / * Quadratic attenuation * /
				float           Theta;            / * Inner angle of spotlight cone * /
				float           Phi;              / * Outer angle of spotlight cone * /*/
		

		int nRet = 0;
		bool bRet = false;
		TCHAR tczTemp[MAX_PATH];
		FALSE_ASSERT_RETURN( pIniFile && szSection && szKeyName);
		
		bRet = SaveINIFileHelper(pIniFile, szSection, _T("Type"), (INT)DataRet.Type);
		KG_PROCESS_ERROR(bRet);
		
		//用D3DCOLORVALUE的重载版本
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Diffuse"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = SaveINIFileHelper(pIniFile, szSection, tczTemp, DataRet.Diffuse);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Specular"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = SaveINIFileHelper(pIniFile, szSection, tczTemp, DataRet.Specular);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Ambient"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = SaveINIFileHelper(pIniFile, szSection, tczTemp, DataRet.Ambient);
			KG_PROCESS_ERROR(bRet);
		}
		//用D3DVECTOR的重载版本
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Position"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = SaveINIFileHelper(pIniFile, szSection, tczTemp, DataRet.Position);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Direction"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = SaveINIFileHelper(pIniFile, szSection, tczTemp, DataRet.Direction);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Rang_FallOff_A012_Theta_Phi"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			_ASSERTE((&DataRet.Phi) - (&DataRet.Range) == 6);
			nRet = pIniFile->WriteMultiFloat(szSection, tczTemp, const_cast<float*>(&DataRet.Range), 7);//一次写入剩下的
			KG_PROCESS_ERROR(nRet > 0);
		}
		
		
		return true;
Exit0:
		return false;
	}

	
	bool Private::LoadINIFileHelper( IIniFile* pIniFile, const char* szSection, const char* szKeyName, D3DLIGHT9* pDataRet )
	{
		int nRet = 0;
		bool bRet = false;
		TCHAR tczTemp[MAX_PATH];
		FALSE_ASSERT_RETURN( pIniFile && szSection && szKeyName && pDataRet);

		{
			INT nType = 0;
			bRet = LoadINIFileHelper(pIniFile, szSection, _T("Type"), 0, &nType);
			KG_PROCESS_ERROR(bRet);
			KG_PROCESS_ERROR(nType >= D3DLIGHT_POINT && nType <= D3DLIGHT_DIRECTIONAL);
			pDataRet->Type = (D3DLIGHTTYPE)nType;
		}
		

		//用D3DCOLORVALUE的重载版本
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Diffuse"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = LoadINIFileHelper(pIniFile, szSection, tczTemp, &pDataRet->Diffuse);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Specular"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = LoadINIFileHelper(pIniFile, szSection, tczTemp, &pDataRet->Specular);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Ambient"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = LoadINIFileHelper(pIniFile, szSection, tczTemp, &pDataRet->Ambient);
			KG_PROCESS_ERROR(bRet);
		}
		//用D3DVECTOR的重载版本
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Position"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = LoadINIFileHelper(pIniFile, szSection, tczTemp, &pDataRet->Position);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Direction"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			bRet = LoadINIFileHelper(pIniFile, szSection, tczTemp, &pDataRet->Direction);
			KG_PROCESS_ERROR(bRet);
		}
		{
			nRet = _stprintf_s(tczTemp, _T("%s_Rang_FallOff_A012_Theta_Phi"), szKeyName);
			KG_PROCESS_ERROR(nRet);
			_ASSERTE((&pDataRet->Phi) - (&pDataRet->Range) == 6);
			nRet = pIniFile->GetMultiFloat(szSection, tczTemp, &pDataRet->Range, 7);//一次写入剩下的
		}
		return true;
Exit0:
		return false;
	}

	
};

