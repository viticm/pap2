// KRepNPC.h: interface for the KRepNPC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KREPNPC_H__853752D9_02F9_46A0_9971_0DC394D1A46A__INCLUDED_)
#define AFX_KREPNPC_H__853752D9_02F9_46A0_9971_0DC394D1A46A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataObject.h"

class KNpcTemplate;

class KRepNPC : public KComponentObject  
{
public:
	struct _AnimationInfo
	{
		int      nAniIndex;
		string   scName;
		string   scSoundFile;
		float    fRatio;

		_AnimationInfo()
		{
			nAniIndex = -1;
			scName = "";
			scSoundFile = "";
			fRatio = 1;
		}
	};

	vector<_AnimationInfo>m_vecAniStand;

	LPMODEL  m_lpModelBody;//身体

	LPMODEL  m_lpModelRHand;//右手武器
	LPMODEL  m_lpModelLHand;//左手武器

	string   m_scIniFileName;


	KNpcTemplate* m_pNpcTemplate;
	DWORD	m_dwLogicalScriptID;
	int           m_nLogicalKind;
	int           m_nLogicalLevel;
	string        m_szLogicalName;
	string        m_szLogicalScriptFile;
public:
	virtual BOOL    SelectByPickLine(float* Distance,D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir);

	virtual HRESULT Copy(KComponentObject** pNewObject);
	virtual HRESULT FrameMove();
	virtual HRESULT Render();
	virtual HRESULT LoadByIni(IIniFile* pIniFile,LPSTR pFileName);

	KRepNPC();
	virtual ~KRepNPC();

public:
	HRESULT GetLogicalPosition(int* pX,int* pZ,AABBOX* pMapBox);
	HRESULT GetLogicalKind(int* pKind);
	HRESULT GetLogicalTemplateID(DWORD* pID);
	HRESULT GetLogicalName(char* pName);
	HRESULT GetLogicalLevel(int* pLevel);
	HRESULT GetLogicalScriptID(DWORD* pdwScriptID);

	HRESULT SetLogicalLevel(int Level);
	HRESULT SetLogicalName(char* pName);
	HRESULT SetLogicalScript(char* pFileName);
};

#endif // !defined(AFX_KREPNPC_H__853752D9_02F9_46A0_9971_0DC394D1A46A__INCLUDED_)
