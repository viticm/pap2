////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DAnimationWarper.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-1 14:10:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DANIMATIONWARPER_H_
#define _INCLUDE_IEKG3DANIMATIONWARPER_H_


////////////////////////////////////////////////////////////////////////////////
interface IEKG3DAnimationComposer
{
    virtual HRESULT SetBoneInfo(
        unsigned int nAnmation,
        unsigned int nBone,
        DWORD dwType,
        float fWeight
    ) = 0;

    virtual HRESULT GetBoneInfo(unsigned int nAnmation,
        unsigned int nBone,
        DWORD& dwType,
        float& fWeight) = 0;

    virtual HRESULT GetAnimationCount(DWORD *pdwRetAnimationCount) = 0;

};

interface IEKG3DAnimation;
interface IEKG3DAnimationWarper
{
    virtual HRESULT AddAnimation(IEKG3DClip *pClip, int nComposerIndex) = 0;
    virtual HRESULT Enable(BOOL bEnable) = 0;

    virtual HRESULT AddComposer() = 0;
    virtual HRESULT GetComposerCount(DWORD *pnRetComposerCount) = 0;
    virtual HRESULT GetComposer(int nIndex, IEKG3DAnimationComposer **ppiRetComposer) = 0;

    virtual HRESULT GetWeight(int nComposer, int nBoneIndex, FLOAT *pfRetWeight) = 0;
    virtual HRESULT SetWeight(int nComposer, int nBoneIndex, float fWeight, BOOL bInherit) = 0;

	virtual HRESULT ChangeAnimation(unsigned int nComposer,  
		unsigned int nAnimation, 
		//IEKG3DAnimation* pAnimation, 
		IEKG3DClip* pClip,
		IKG3DAnimationController* pStart,
		DWORD dwTweenTime) = 0;

    virtual HRESULT Save(const TCHAR *strFileName) = 0;
    virtual HRESULT Load(const TCHAR *strFileName) = 0;


};

#endif //_INCLUDE_IEKG3DANIMATIONWARPER_H_
