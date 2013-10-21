#pragma once

#include "KG3DAnimationWarper.h"

class KG3DAnimationSplitter
{
public:
	const static TCHAR* s_strConfigFile;
	KG3DAnimationSplitter(void);
	~KG3DAnimationSplitter(void);
	void SetModel(KG3DModel* pModel);

	HRESULT PlayAnimation(LPCSTR strAnimationName, 
		unsigned int uPresetIndex,
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT FrameMove(D3DXMATRIX *pResult);

	HRESULT LoadConfig();
	HRESULT LoadPreset(unsigned int uIndex);
	//below is add for editor only, do not call outside the editor
	KG3DAnimationWarper* GetAnimationWarper();
	void EnableWarper(BOOL bEnable);

	BOOL GetEnable();
	void UnInit();
	void OnSecondaryAnimationFinish(KG3DAnimationController *pController);
	void SeekAnimation(DWORD dwSplitType, 
		enuAnimationControllerPriority nPrority,
		DWORD dwSeekType, 
		int nOffset);

private:
	HRESULT PlayAnimationChange(LPCSTR strAnimationName, 
		unsigned int uPresetIndex,
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
        int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT MoveCurrentToSecondaryAndPlay(LPCSTR strAnimationName, 
		unsigned int uPresetIndex,
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	HRESULT PlayAnimationSecondary(LPCSTR strAnimationName, 
		DWORD dwSplitType,
		DWORD dwPlayType,
		FLOAT fSpeed,
		int   nOffsetTime,
		PVOID pReserved,
		PVOID pUserdata,
		enuModelPlayAnimationType Type,
		KG3DAnimationTagContainer* pTagContainerNew,
		KG3DClip* pClip);

	static BOOL m_bConfigLoaded;
	static std::vector<string> m_PresetPathName;
	unsigned int m_uCurrentPreset;
	KG3DAnimationWarper m_Warper;
	KG3DModel* m_pModel;
};
