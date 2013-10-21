#ifndef _INCLUDE_IEKG3DANIMATION_H_
#define _INCLUDE_IEKG3DANIMATION_H_

class IEKG3DAni;

struct AnimationItem
{
    TCHAR szClipName[MAX_PATH];
    IEKG3DAni* piClip;
    INT nStartFrame;
    INT nEndFrame;
    FLOAT fSpeed;
    FLOAT fStartTimeOffset;
    FLOAT fEndTimeOffset;
    FLOAT fTweenTime;
};

enum enuSetBaseClipInfoFlag
{
    SETBASECLIPFLAG_NAME         = 0x1,
    SETBASECLIPFLAG_FRAMELENGTH  = 0x1<<1,
    SETBASECLIPFLAG_FORCE_DWORD  = 0xffffffff,
};

interface IEKG3DAnimation
{
	virtual HRESULT GetAniName(LPCTSTR *ppszName) = 0;
	virtual HRESULT SetAniName(LPCTSTR pszName) = 0;

    virtual HRESULT GetName(LPCTSTR *ppszName) = 0;
    virtual HRESULT SetName(LPCTSTR pszName) = 0;

	virtual HRESULT SaveToFile(LPCTSTR cszFileName, UINT uOption) = 0;
	virtual HRESULT SetClipData(INT nSlot, INT nStartFrame, INT nEndFrame, FLOAT fSpeed, FLOAT fTweenTime) = 0;
	virtual HRESULT RemoveClipFromAnimation(INT nIndex) = 0;

	
	virtual HRESULT GetClipData(INT nSlot, AnimationItem& Data) = 0;

	virtual HRESULT Rename(LPCTSTR strNewName) = 0;
	virtual HRESULT GetNumClips(INT *pnRet) = 0;
    virtual HRESULT InsertAnimation(const IEKG3DAnimation* pSrc, INT nSlot) = 0;

    virtual HRESULT GetBaseClipFrameLength(int nSlot, FLOAT *pfRetLength) = 0;
    virtual HRESULT SetBaseClipInfo(int nSlot, 
        enuSetBaseClipInfoFlag Flag, 
        float fFrameLength, 
        const TCHAR* strClipName) = 0;

    virtual ULONG STDMETHODCALLTYPE Release() = 0;

    virtual HRESULT GetNumFrame(DWORD* pNumFrame) = 0;
    
};
#endif 