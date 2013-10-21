#pragma once
#include "KG3DAnimationTag.h"

class KG3DAnimationTagContainer: public IKG3DAnimationControllerUpdateNotifier, public IEKG3DAnimationTagContainer
{
    const static UINT INDEX_INVALIDATE = 0xffffffff;
    struct Item
    {
        enuTagType Type;
        IKG3DAnimationTag* pTag;
    };
    
public:
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();
	void Clear();
    KG3DAnimationTagContainer();
    ~KG3DAnimationTagContainer();
    virtual BOOL IsLoaded();
	HRESULT AttachToModel(IKG3DModel* pModel);
    virtual IKG3DModel* GetAttachedModel();
    virtual LPCSTR GetAnimationFilePath();
    virtual HRESULT Start(float fSpeed, 
        int   nOffsetTime, 
		DWORD dwType, 
		enuAnimationControllerPriority Priority);
    virtual HRESULT SetAnimation(LPCSTR strFileName);

	virtual IEKG3DAnimationTag* GetTag(enuTagType Type);
    virtual HRESULT FrameMove(IKG3DAnimationController* pController);
    
	HRESULT _Load(LPCSTR strFileName);
	virtual HRESULT Load(LPCSTR strFileName);
    virtual BOOL GetModifyState();
    virtual void SetModifyState();
    virtual HRESULT Save(LPCSTR strFileName);
	HRESULT _Save(LPCSTR strFileName);
	virtual HRESULT ExportTani(LPCSTR strFileName,DWORD dwMaxFrames);//把当前的标签数据导出到另外一个动作文件。
    enuTagRunState GetState();
    void OnExit();
    void Reset();
    HRESULT GetCurrenetAnimation(IEKG3DClip** ppClip);

	HRESULT Clone(KG3DAnimationTagContainer* pSrc);
	HRESULT LoadFromFileMultiThread();
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize);

private:
    HRESULT SkipBlock(AnimationTagBlockHeader *pBlockHeader, IFile *piFile);

#pragma pack(push, 1)
	struct FileHeader
    {
        const static DWORD s_dwMask = 0x41544147;//ASCII for "ATAG"
        const static DWORD s_dwCurrentVersion = 0x0;
        DWORD dwMask;
        DWORD dwVersion;
        TCHAR strAnimationFileName[MAX_PATH];
        DWORD dwNumBlock;
        DWORD dwReserved[8];
        FileHeader();
    };
#pragma pack(pop)
    char m_szAnimationName[MAX_PATH];
    IKG3DAnimationTag* GetNewInstance(enuTagType nType);
    std::vector<Item> m_vecTags;
    KG3DClip* m_pClip;
    KG3DModel* m_pModel;
    UINT m_uMotionTagIndex;
    BOOL m_bModifyFlag;
	BOOL m_IsLoaded;
	ULONG m_ulRefCount;

	string m_scFileName;
public:
	BOOL  m_bInMultiThreadLoadList;
};

class KG3DAnimationTagContainerTable
{
public:
	HRESULT Init();
	HRESULT UnInit();
	KG3DAnimationTagContainer* LoadTag(LPCSTR strFileName);

	KG3DAnimationTagContainerTable();
    virtual ~KG3DAnimationTagContainerTable();

private:
    typedef map<DWORD, KG3DAnimationTagContainer *> TAG_MAP;
    TAG_MAP m_mapTagCache;
};

extern KG3DAnimationTagContainerTable g_tagConTable;