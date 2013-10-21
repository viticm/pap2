#pragma once
#include <vector>
#include <string>
#include <map>
#include "IEKG3DAnimationTag.h"
#include "IEEditor.h"
#include "KSceneModelEditorFrameWnd.h"
#include "MeshEditorDoc.h"
class KDlgResourceList;
class KGFileFind
{
public:
	KGFileFind(void);
	~KGFileFind(void);
	HRESULT Search(LPCSTR strStartPath, 
		LPCSTR *ppKeywords, 
		DWORD dwNumKeywords,
		LPCSTR *ppIngoreList,
		DWORD dwNumIgnoreList,
		BOOL bIncludeSubDir);
	std::vector<std::string> m_strResult;
private:
	HRESULT Helper_Search(LPCSTR strStartPath, 
		LPCSTR *ppKeywords, 
		DWORD dwNumKeywords,
		LPCSTR *ppIngoreList,
		DWORD dwNumIgnoreList,
		BOOL bIncludeSubDir);
};

class KDlgLogicSaveProgress;
class KDlgAnimationTagBatchTool;
struct MetorSelInfo;
class KGAnimationTagBatchTool
{
public:
	friend class KDlgAnimationTagBatchTool;
	KGAnimationTagBatchTool();
	~KGAnimationTagBatchTool();

	enum enuSouceType
	{
		TYPE_TANI,
		TYPE_SFX,
		TYPE_SOUND,
		TYPE_COUNT,
	};

	struct SrcToDest
	{
		DWORD dwType;
		std::string strSrc;//相对于exe的路径
		TCHAR strSrcName[MAX_PATH];//文件名
		std::vector<TagSourceInfo> vecDest;
		SrcToDest()
		{
			dwType = TYPE_COUNT;
		}
	};

	std::vector<SrcToDest> GetUsedTagFile(std::vector<std::string>& strSrcFiles);//strSrcFiles可以是声音文件或者特效文件
	HRESULT GetUsedTagFile(DWORD dwHash, SrcToDest &Info);
	HRESULT Init(LPCSTR strTagPath,
		IEKG3DAnimationTagContainer *pTagContainer, 
		BOOL bClear);
	
	void FillListBox(CListBox *pListBox, DWORD dwType);
	CString GetFullPath(DWORD dwType, DWORD dwHash);
	DWORD GetType(DWORD dwType, DWORD dwHash);
	CString GetFileName(DWORD dwType, DWORD dwHash);
	HRESULT ApplyProcess(DWORD dwProcessType,
		IEKG3DAnimationTagContainer *pContainer,
		DWORD dwType,
		DWORD dwHash, 
		LPCSTR strOld, 
		LPCSTR strNew, 
		LPCSTR strSuffix, 
		void *pExtraInfo,
		KDlgResourceList *pdlgResourceList);
	KDlgLogicSaveProgress *m_pProgressDlg;
	void ProcessTagInfo(LPCSTR strFilePath, 
		TagSourceInfo *pInfo, 
		DWORD dwNumInfo);
	void ClearTagInfo(DWORD dwHash);
	HRESULT Get_Model_Editor(OUT IEKG3DSceneModelEditor** ppEditor);
private:
	std::map<DWORD, SrcToDest> m_TagToSource;
	std::map<DWORD, SrcToDest> m_SourceToTag;
	std::set<DWORD> m_ProcessedSFX;
	// add by chenganlai
	HRESULT ModifyMetor(IEKG3DAnimationTagContainer *pContainer,
		map<DWORD,MetorSelInfo> MetroSelect,UINT uFrame);
	void OnNewMotionTag(long nFrame,IEKG3DAnimationTagContainer *pContainer);
	HRESULT MergeMetro(MotionTagBlockMeteor *&pMetro,MotionTagDataRuntime *pMotionTagData,int *nIndex);
	//
};

extern KGAnimationTagBatchTool g_AnimationTagBatchTool;
