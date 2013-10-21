#pragma once
#include "kg3danimationtag.h"
class KG3DAnimationSoundTagInfo
{
public:
	KG3DAnimationSoundTagInfo(void);
	~KG3DAnimationSoundTagInfo(void);
	void Init();
	HRESULT GetSound(SoundType soundType,AnimationSoundTagInfo * pInfo);
	enum {
		COL_BLANK,
		COL_SOUNDTYPE,
		COL_FILENAME,
		COL_RATE,
		TABWIDE,
	};
	BOOL IsInit()
	{
		return m_bInit;
	}
    int IsSaveVersionData(){return m_nSaveVersion3Data;}
protected:
    AnimationSoundTagInfo m_Default;
	std::map<int,std::vector<AnimationSoundTagInfo> > m_AnimationSoundTagInfo;
	std::map<std::string,int> m_SoundType;
	typedef std::map<std::string,int>::iterator iter;
	typedef std::map<int,std::vector<AnimationSoundTagInfo> >::iterator iterInfo;
	void Adjust();
	BOOL m_bInit;
    int m_nSaveVersion3Data;
};
extern KG3DAnimationSoundTagInfo& g_GetAnimationSoundTagInfo();
