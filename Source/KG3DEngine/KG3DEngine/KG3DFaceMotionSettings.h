#pragma once
#include "KG3DInterface.h"
struct FaceMotionInfo
{
	TCHAR strName[MAX_PATH];//表情的名字
	TCHAR strSuffix[MAX_PATH];//表情的动画文件的后缀
};

struct DefaultFaceMotion
{
	TCHAR strSuffix[MAX_PATH];//后缀名字
	float fPlayRate;//播放几率
	int nSpan;//检测是否要播放的间隔时间
};

class KG3DModel;
struct DefaultFaceMotionMgrItem
{
	KG3DModel *pModel;
	DWORD dwLastCheck;
	DefaultFaceMotionMgrItem()
	{
		pModel = NULL;
		dwLastCheck = 0;
	}
};

class KG3DFaceMotionSettings : public IKG3DFaceMotionSettings
{
public:
	KG3DFaceMotionSettings();
	~KG3DFaceMotionSettings();

	HRESULT Init();
	HRESULT ApplyDefaultMotion(DefaultFaceMotionMgrItem& FaceMotionItem);
	HRESULT GetFaceMotion(DWORD dwType,
		LPCSTR strFaceMeshName,
		LPSTR strFaceMotionAnimationName);
	virtual LPCSTR GetFaceMotionTypeName(DWORD dwIndex);
	virtual LPCSTR GetFaceMotionSuffixName(DWORD dwIndex);
	virtual LPCSTR GetFaceMotionModifyPath();
	virtual void GetFaceMotionPathFromFacePath(LPSTR strFacePath);
	virtual DWORD GetNumFaceMotion();
	void RefreshIgnoreFile();
private:
	DWORD GetFaceMotionType(LPCSTR strTypeName);
	const static DWORD s_dwInvalidateType = 0xffffffff;
	std::vector<FaceMotionInfo> m_FaceMotion;
	std::vector<DefaultFaceMotion> m_DefaultMotion;
	//下面是根据脸的mesh文件名自动找动画文件需要的参数
	TCHAR m_strFaceMotionModifyPath[MAX_PATH];//需要添加的目录
	int m_nPathDepth;//需要回溯多少级目录
	std::set<DWORD> m_IgnoreFile;
};

class KG3DDefaultFaceMotionMgr
{
public:
	KG3DDefaultFaceMotionMgr();
	~KG3DDefaultFaceMotionMgr();
	HRESULT UnInit();
	void SetDefaultFaceMotion(KG3DModel *pModel);
	void FrameMove();
private:
	std::vector<DefaultFaceMotionMgrItem> m_FaceModels;
	std::vector<DefaultFaceMotionMgrItem> m_FaceModelsBack;
	std::vector<DefaultFaceMotionMgrItem>* m_pCurrentFaceModelList;
	std::vector<DefaultFaceMotionMgrItem>* m_pBackFaceModelList;
};

extern KG3DFaceMotionSettings g_FaceMotionSettings;
extern KG3DDefaultFaceMotionMgr g_DefaultFaceMotionMgr;