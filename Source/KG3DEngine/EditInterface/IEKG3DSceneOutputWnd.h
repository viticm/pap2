////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneOutputWnd.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2006-11-09 11:30:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENEOUTPUTWND_H_
#define _INCLUDE_IEKG3DSCENEOUTPUTWND_H_
interface IEKG3DCamera;
interface IEKG3DCameraStation;
////////////////////////////////////////////////////////////////////////////////

enum {
    CHANNEL_RED = 1,
    CHANNEL_GREEN = 1 << 1,
    CHANNEL_BLUE  = 1 << 2
};


interface IEKG3DSceneOutputWnd
{
    virtual HRESULT ResetDevice(BOOL bEnable, DWORD dwWidth, DWORD dwHeight) = 0;
    virtual HRESULT EnablePostEffect(PostRenderEffectType Type, BOOL bEnable) = 0;
    virtual HRESULT ReloadFX() = 0;
    virtual IEKG3DCamera* GetIECamera() = 0;

    // ---------- bloom parameter ------------
    virtual int SetBloomGate(float fValue)= 0;
    virtual int SetBloomExposure1(float fValue)= 0;
    virtual int SetBloomExposure2(float fValue)= 0;
    virtual int SetBloomGaussCoefficient(float fValue)= 0;
    virtual int SetfBloomSampleNum(int nValue)= 0;
    virtual int SetBloomDelaye(float fValue)= 0;

    virtual int SetDetailPower(float fValue) = 0;     
    virtual int SetBlurPower(float fValue) = 0;   
    virtual int SetBlurDistance(float fValue) = 0;        


    // ---------- hsi parameter --------------
    virtual int SetHSIHueScale(float fValue)= 0;
    virtual int SetHSIHueBias(float fValue)= 0;
    virtual int SetHSISaturationScale(float fValue)= 0;
    virtual int SetHSISaturationBias(float fValue)= 0;
    virtual int SetHSIIntensityScale(float fValue)= 0;
    virtual int SetHSIIntensityBias(float fValue)= 0;

    // ---------- color curve CMYK ------------
    virtual int SetActiveCurveRedPoints(D3DXVECTOR2* pts, int num) = 0;
    virtual int SetActiveCurveGreedPoints(D3DXVECTOR2* pts, int num) = 0;
    virtual int SetActiveCurveBluePoints(D3DXVECTOR2* pts, int num) = 0;

                   
    virtual D3DXVECTOR2 GetSplinePoint(int stype, int channel, int index) = 0;  // channel, 0 red, 1 green 2 blue
    virtual int GetSplinePointCount(int stype, int channel) = 0;
    virtual int GetStypeCount() = 0;
    virtual int GetStypeName(int index, char szName[]) = 0;
    virtual int SetStypeName(int index, char szName[]) = 0;
    virtual int SetActiveStype(int index, int bFlush) = 0;
    virtual int GetActiveStype() = 0;;
    virtual int CloneActiveStype() = 0; // if success return index, else return - 1
    virtual int DelActiveStype() = 0;   // if success return next index, else return -1
    virtual int ResetActiveStype(DWORD channel) = 0;

    virtual int SetCMYK_KC(float k) = 0;
    virtual int SetCMYK_KM(float k) = 0;
    virtual int SetCMYK_KY(float k) = 0;

    // ---------- shock wave -----------------
    virtual int SetShockWavePower(float fValue)= 0;

    virtual int SetGodRaySampleNum(int nValue) = 0;
    virtual int SetGodRayDensity(float fValue) = 0;
    virtual int SetGodRayWeight(float fValue) = 0;
    virtual int SetGodRayDecay(float fValue) = 0;
    virtual int SetGodRayExposure(float fValue) = 0;
    virtual int SetGodRayAngle(float fValue) = 0;

    virtual int SetSSAOLevel(int nLevel) = 0;  
    virtual int SetSSAOSampRadius(float radius) = 0;   
    virtual int SetSSAODarkFact(float dark) = 0;
    virtual int SetSSAOBlurWeight(float w) = 0;

	// ---------- shadow map -----------------
	virtual int SetEnabledAreaShadow(BOOL bEnabled)= 0;
	virtual int SetShadowSoftness(float fValue) = 0;
	virtual int SetShadowJitter(float fValue) = 0;

	//------------hdr----------------------------
	virtual int SetHDRBrightGate(float fValue) = 0;
	virtual int SetHDRRectify(float fValue) = 0;
	virtual int SetHDRGrayLevel(float fValue) = 0;
	virtual int SetHDRStarType(int fValue) = 0;
	virtual int SetHDREnableBlueShift(BOOL fValue) = 0;
	virtual int SetHDRBloomScale(float fValue) = 0;
	virtual int SetHDRStarScale(float fValue) = 0;
	virtual int GetHDRStarTypeCount() = 0;
	virtual int GetHDRStarName(int index, char szName[]) = 0;

    virtual const D3DVIEWPORT9* GetViewPort() = 0;
    ////////////////////////////////////////////////////////////////////////////////
    virtual HRESULT GetCameraStation(IEKG3DCameraStation **ppiStation) = 0;
    virtual HRESULT InitCameraStation(const char pcszFileName[]) = 0;
    virtual HRESULT GetViewPortWidthHeight(DWORD *pdwRetWidth, DWORD *pdwRetHeight) = 0;
    virtual HRESULT GetRect(RECT *pRetRect) = 0;

    virtual HRESULT GetPickRay(D3DXVECTOR3* pRaySrc,D3DXVECTOR3* pNormalizedDir, D3DXVECTOR2* pRaySrc2D) = 0;	//前两个参数可以获得隐式直线，最后一个参数获得窗口点坐标，参数都可选

    //virtual HRESULT SetMiscParam(INT nIndex, FLOAT fValue) = 0;
    //virtual HRESULT SetMiscTechnique(LPCSTR Name) = 0;
	virtual HRESULT Enable(BOOL bEnable) = 0;
	virtual HRESULT IsEnable(BOOL* bRetEnable) = 0;

	virtual HRESULT GetShadowMask(LPDIRECT3DTEXTURE9* ppTexture) = 0;
	virtual void GetDepthTexCoordScale(float* pU, float* pV) = 0;
};

struct IEKG3DSceneOutputWndDummy : public IEKG3DSceneOutputWnd 
{
	virtual HRESULT ResetDevice(BOOL bEnable, DWORD dwWidth, DWORD dwHeight) {return E_FAIL;}
	virtual HRESULT EnablePostEffect(PostRenderEffectType Type, BOOL bEnable) {return E_FAIL;}
	virtual HRESULT ReloadFX() {return E_FAIL;}
	virtual IEKG3DCamera* GetIECamera() {return NULL;}

	// ---------- bloom parameter ------------
	virtual int SetBloomGate(float fValue){return 0;}
	virtual int SetBloomExposure1(float fValue){return 0;}
	virtual int SetBloomExposure2(float fValue){return 0;}
	virtual int SetBloomGaussCoefficient(float fValue){return 0;}
	virtual int SetfBloomSampleNum(int nValue){return 0;}
	virtual int SetBloomDelaye(float fValue){return 0;}

	virtual int SetDetailPower(float fValue) {return 0;}     
	virtual int SetBlurPower(float fValue) {return 0;}   
	virtual int SetBlurDistance(float fValue) {return 0;}        


	// ---------- hsi parameter --------------
	virtual int SetHSIHueScale(float fValue){return 0;}
	virtual int SetHSIHueBias(float fValue){return 0;}
	virtual int SetHSISaturationScale(float fValue){return 0;}
	virtual int SetHSISaturationBias(float fValue){return 0;}
	virtual int SetHSIIntensityScale(float fValue){return 0;}
	virtual int SetHSIIntensityBias(float fValue){return 0;}

	// ---------- color curve CMYK ------------
	virtual int SetActiveCurveRedPoints(D3DXVECTOR2* pts, int num) {return 0;}
	virtual int SetActiveCurveGreedPoints(D3DXVECTOR2* pts, int num) {return 0;}
	virtual int SetActiveCurveBluePoints(D3DXVECTOR2* pts, int num) {return 0;}


	virtual D3DXVECTOR2 GetSplinePoint(int stype, int channel, int index) {return D3DXVECTOR2(0,0);}  // channel, 0 red, 1 green 2 blue
	virtual int GetSplinePointCount(int stype, int channel) {return 0;}
	virtual int GetStypeCount() {return 0;}
	virtual int GetStypeName(int index, char szName[]) {return 0;}
	virtual int SetStypeName(int index, char szName[]) {return 0;}
	virtual int SetActiveStype(int index, int bFlush) {return 0;}
	virtual int GetActiveStype() {return 0;};
	virtual int CloneActiveStype() {return 0;} // if success return index, else return - 1
	virtual int DelActiveStype() {return 0;}   // if success return next index, else return -1
	virtual int ResetActiveStype(DWORD channel) {return 0;}

	virtual int SetCMYK_KC(float k) {return 0;}
	virtual int SetCMYK_KM(float k) {return 0;}
	virtual int SetCMYK_KY(float k) {return 0;}

	// ---------- shock wave -----------------
	virtual int SetShockWavePower(float fValue){return 0;}

	virtual int SetGodRaySampleNum(int nValue) {return 0;}
	virtual int SetGodRayDensity(float fValue) {return 0;}
	virtual int SetGodRayWeight(float fValue) {return 0;}
	virtual int SetGodRayDecay(float fValue) {return 0;}
	virtual int SetGodRayExposure(float fValue) {return 0;}
	virtual int SetGodRayAngle(float fValue) {return 0;}

	virtual int SetSSAOLevel(int nLevel) {return 0;}  
	virtual int SetSSAOSampRadius(float radius) {return 0;}   
	virtual int SetSSAODarkFact(float dark) {return 0;}
	virtual int SetSSAOBlurWeight(float w) {return 0;}

	// ---------- shadow map -----------------
	virtual int SetEnabledAreaShadow(BOOL bEnabled){return 0;}
	virtual int SetShadowSoftness(float fValue){return 0;}
	virtual int SetShadowJitter(float fValue){return 0;}

	//------------hdr----------------------------
	virtual int SetHDRBrightGate(float fValue){return 0;}
	virtual int SetHDRRectify(float fValue){return 0;}
	virtual int SetHDRGrayLevel(float fValue){return 0;}
	virtual int SetHDRStarType(int fValue){return 0;}
	virtual int SetHDREnableBlueShift(BOOL fValue){return 0;}
	virtual int SetHDRBloomScale(float fValue){return 0;}
	virtual int SetHDRStarScale(float fValue){return 0;}
	virtual int GetHDRStarTypeCount(){return 0;}
	virtual int GetHDRStarName(int index, char szName[]){return 0;}

	virtual const D3DVIEWPORT9* GetViewPort() {return 0;}
	////////////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetCameraStation(IEKG3DCameraStation **ppiStation) {return E_FAIL;}
	virtual HRESULT InitCameraStation(const char pcszFileName[]) {return E_FAIL;}
	virtual HRESULT GetViewPortWidthHeight(DWORD *pdwRetWidth, DWORD *pdwRetHeight) {return E_FAIL;}
	virtual HRESULT GetRect(RECT *pRetRect) {return E_FAIL;}

	virtual HRESULT GetPickRay(D3DXVECTOR3* pRaySrc,D3DXVECTOR3* pNormalizedDir, D3DXVECTOR2* pRaySrc2D) {return E_FAIL;}	//前两个参数可以获得隐式直线，最后一个参数获得窗口点坐标，参数都可选

	virtual HRESULT Enable(BOOL bEnable) {return E_FAIL;}
	virtual HRESULT IsEnable(BOOL* bRetEnable) {return E_FAIL;}
	virtual HRESULT GetShadowMask(LPDIRECT3DTEXTURE9* ppTexture){return E_FAIL;}
	virtual void GetDepthTexCoordScale(float* pU, float* pV){return;}
};

#endif //_INCLUDE_IEKG3DSCENEOUTPUTWND_H_
