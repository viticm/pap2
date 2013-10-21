////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DAnimationController.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-1 9:25:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DANIMATIONCONTROLLER_H_
#define _INCLUDE_IEKG3DANIMATIONCONTROLLER_H_

////////////////////////////////////////////////////////////////////////////////
//class IEKG3DAnimation;
struct IEKG3DClip;

interface IEKG3DAnimationController
{
    virtual HRESULT Detach() = 0;
    virtual HRESULT SetAnimation(IEKG3DClip *pClip) = 0;
    virtual HRESULT GetCurAnimation(IEKG3DClip **ppClip) = 0;
    virtual VOID PauseAnimation(BOOL bPause) = 0;
};

#endif //_INCLUDE_IEKG3DANIMATIONCONTROLLER_H_
