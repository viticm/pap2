////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEOther.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-27 16:39:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEOTHER_H_
#define _INCLUDE_IEOTHER_H_

////////////////////////////////////////////////////////////////////////////////


#include "../../../Include/kg3dengine/kg3dinterface.h" //by dengzhihui Refactor 2006年11月2日
#include "IEKG3DScene.h"
#include "IEKG3DCamera.h"

#define PlayError -2
#define NoChannelToPlay -1
#define MethodTakePlaceError -3
#define RunOk 1
#define PlayLooping 0
#define PlayNormal  1

interface IEKG3DGraphicsTool : public IKG3DGraphicsTool
{
    virtual double CubicSplineIntl(double x[], double y[], int n, int k, double t, double s[5]) = 0;
    virtual void DrawFocusRect(const D3DVIEWPORT9& ViewPort, FLOAT weight, DWORD color) = 0;
	virtual IEKG3DScene* GetScene() = 0;
	virtual IEKG3DCamera* GetCamera() = 0;
	virtual HRESULT DrawScreenRect(const D3DXVECTOR2* pA,
		const D3DXVECTOR2* pC,
		float Z,
		float ScaleU,
		float ScaleV,
		D3DCOLOR Diffuse,
		DWORD TextureID) = 0;
	virtual HRESULT DrawScreenRect(const D3DXVECTOR2 *pPointA,
		const D3DXVECTOR2 *pPointC,
		float fZ,
		DWORD dwColor) = 0;
	virtual HRESULT GetSceneMainCamera(IEKG3DCamera** pRet) = 0;
	virtual HRESULT IEDrawDebugText(LPCTSTR lpText, DWORD dwTimeStay, DWORD* pdwHandle) = 0;
	virtual HRESULT DeleteDebugText(DWORD dwHandle) = 0;
	virtual void RefreshShader() = 0;
	virtual float GetTinyTime() = 0;
	virtual DWORD GetNowTime() = 0;
};

interface IEKG3DTextureTable;
interface IEKG3DModelTable;
interface IEKG3DAnimationTable;
interface IEKG3DMaterial;
interface IEKG3DModel;
typedef struct IEKG3DEnvEffLight IEKG3DLight;//即IEKG3DEnvEffLight
struct OBJECT_PROPERTY;
struct NPC_PROPERTY;
class KG3DFileFinder;
interface IEKG3DRepresentObject;
interface IEKG3DRepresentObjectSet;
interface IEKG3DRegionInfoManagerTable;
interface IEKG3DRepresentObjectSoundBall;
interface IEKG3DMesh;
interface IEKG3DClipTable;


interface IEKG3DEngineManager : public IKG3DEngineManager
{
	virtual HRESULT Reset() = 0;//释放所有资源,恢复为刚启动状态
	virtual HRESULT SetFocusScene(IKG3DScene* pScene) = 0;
	virtual HRESULT SetMaxViewScene(IKG3DScene* pScene) = 0;
	virtual HRESULT GetUseSO3Logical(BOOL* pUseLogic) = 0;
	virtual HRESULT GetUseLogicObject(BOOL* pUseLogicObject) = 0;
	virtual HRESULT GetGraphicsTool(IEKG3DGraphicsTool** pTool) = 0;
    virtual HRESULT GetRepresentObjectTable(IKG3DResourceManager** ppObjectTable) = 0;
    virtual HRESULT GetIEKG3DTextureTable(IEKG3DTextureTable **ppTexture) = 0;
    virtual HRESULT GetIEKG3DModelTable(IEKG3DModelTable **ppModelTable) = 0;
	virtual HRESULT GetIEKG3DClipTable(IEKG3DClipTable **ppClipTable) = 0;
    virtual HRESULT GetNewIEKG3DMaterial(IEKG3DMaterial **ppMat) = 0;
    virtual HRESULT DestroyIEKG3DMaterial(IEKG3DMaterial *pMat) = 0;
    virtual HRESULT ExchangeModelSubsetID(IEKG3DModel* pIModel, int nSubset1, int nSubset2) = 0;
    virtual HRESULT GetNewIEKG3DLight(IEKG3DLight **ppiLight) = 0;
	virtual HRESULT GetDefWorkDirectory(LPTSTR pszBuf, DWORD dwBufSizeInCharCount) = 0;
    virtual HRESULT GetD3DDevice(LPDIRECT3DDEVICE9* ppDevice) = 0;
    virtual HRESULT GetBaseWnd(HWND *phBaseWnd) = 0; //取全局变量g_hBaseWnd

    virtual PostRenderParams* LockPostRenderParams(BOOL bReadOnly) = 0;
    virtual void UnlockPostRenderParams() = 0;

    virtual void GetHDFilePath(char szHD1Path[], char szHD2Path[], const char szRetPath[]) = 0;
    
    virtual HRESULT GetRelativePath(const TCHAR* strAbsPath11, const TCHAR* strAbsPath21, TCHAR* strRelativePath) = 0;
    virtual HRESULT GetSpeedTreeLeafScale(FLOAT *pfRetScale) = 0;
    virtual HRESULT SetSpeedTreeLeafScale(FLOAT fScale) = 0;

    virtual HRESULT GetRenderTreeFlag(BOOL *pbRetRenderTree) = 0;
    virtual HRESULT SetRenderTreeFlag(BOOL bRenderTree) = 0;

    virtual HRESULT ChangeMovingState() = 0;

    virtual HRESULT GetAutoReloadTexture(BOOL *pbAutoReloadTexture) = 0;
    virtual HRESULT SetAutoReloadTexture(BOOL bAutoReloadTexture) = 0;

    virtual HRESULT SetFullScreen(BOOL bFullScreen) = 0;

	virtual HRESULT SetRenderTerrain(BOOL bRenderTerrain) = 0;
	virtual BOOL GetRenderTerrain() = 0;

    // GrassPattern
    virtual HRESULT LoadPattern(LPCSTR strFileName, int& nIndex) = 0;
    virtual HRESULT FindPattern(LPCSTR strFileName, INT *pnRetIndex) = 0;

    // Represent object template
    virtual HRESULT GetRepresentObjectPropertyCount(DWORD *pdwRetCount) = 0;
    virtual HRESULT GetRepresentObjectProperty(DWORD dwIndex, OBJECT_PROPERTY **ppRetObjProperty) = 0;

    // Represent NPC template
    virtual HRESULT GetRepresentNPCPropertyCount(DWORD *pdwRetCount) = 0;
    virtual HRESULT GetRepresentNPCProperty(DWORD dwIndex, NPC_PROPERTY **ppRetNPCProperty) = 0;

    //virtual HRESULT GetCaps(D3DCAPS9 **ppRetCaps9) = 0;//用EngineManager的GetEngineCaps，不要用这个 Last modify by Chen TianHong:2008-9-25 21:25:29

	/////////////////////////////////////////////////////////////
	// 编辑器端修改选项 Add by FengBo 2007-9-5
	virtual HRESULT SetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption) = 0;
	virtual HRESULT GetEngineEditorOption(KG3DEngineEditorOption* pEngineEditorOption) = 0;

	////////////////////////////////////////////////////////////

    //MathTool
    //virtual void LPCTSTR2IntInRange(LPCTSTR czSource, INT* pnDes, INT nLowerBound, INT nUpperBound) = 0;
   // virtual D3DCOLOR ColorValue(D3DCOLORVALUE co) = 0;
   // virtual D3DCOLORVALUE Color2Value(D3DCOLOR Color) = 0;
    //virtual void Color2Ref(const D3DCOLOR &D3DColor, COLORREF &CRColor) = 0;

	virtual HRESULT GetUI(IKG3DUI** pUI) = 0;

	//<Object相关>
	virtual HRESULT BuildKG3DRepresentObject(LPCTSTR lpFileName, DWORD dwType, DWORD_PTR wParam
		, DWORD_PTR lParam, IEKG3DRepresentObject **ppObject) = 0;//调用ObjectTable创建出Obj之后会调用Obj的Build方法，一次创建出一个完整的Obj，不用创建完让外部又LOAD又Set。一般的Obj只用到了lpFileName,另外两个是万能扩展参数，OBJ的Build函数自己决定怎么用。如BuildKG3DRepresentObject(strFilePath, REPRESENTOBJECT_DEFAULT, 0, 0, &pObjSet);
	virtual HRESULT NewKG3DRepresentObject(DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, IEKG3DRepresentObject **ppObject) = 0;//传入类型，返回对应的新OBJ的指针，这样就不用N个类型的OBJ创建N个接口了
	//virtual HRESULT NewKG3DRepresentObjectInTable(IEKG3DRepresentObject **ppObject) = 0;//用上面的
    //virtual HRESULT NewKG3DRepresentObjectSoundBallInTable(IEKG3DRepresentObjectSoundBall **ppObject) = 0;
    //virtual HRESULT NewKG3DRepresentObjectSetInTable(IEKG3DRepresentObjectSet **ppObject) = 0;
	//<Object相关>

    virtual HRESULT GetEnableAddEntity(BOOL **ppbEnableAddEntity) = 0;//取全局变量g_bEnableAddEntity;

	virtual HRESULT GetKeyStateBuffer(BYTE** ppbyKeyStateBuf) = 0;

	virtual HRESULT GetKG3DRTTITypeManager(IKG3DRTTITypeManager** pRTTI) = 0;///<CTH><RTTI_INTERFACE>

	virtual BOOL GetAmibentShaderOnly() = 0;
	virtual HRESULT SetAmibentShaderOnly(BOOL bValue) = 0;
	virtual IEKG3DMesh* FindMeshBase(DWORD dwID) = 0;

	//区域设定///////////////////////////////////////////////////////////////////////
	virtual HRESULT GetIEKG3DRegionInfoManagerTable(IEKG3DRegionInfoManagerTable** ppTable) = 0;
	//相当于QueryInterface////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetIEInterface(DWORD dwType1, DWORD dwType2, DWORD_PTR wParam, DWORD_PTR lParam, LPVOID* ppInterface) = 0;
	virtual BOOL GetImportOldScene() = 0;	//获取是否加载旧地形的标志

    virtual void RegistPushAlertMessageFunction(void (*)(const char[])) = 0;
	virtual void ReloadAnimation() = 0;

	// 策划编辑器
	virtual void ManipulateSceneList(IEKG3DScene* pTargetScene, BOOL bAdd) = 0;
};

//将 struct KG3DTrackCamera::_Station 结构提取为独立的接口
interface IEKG3DCameraStation
{
	virtual HRESULT IniByConfigFile(LPCTSTR pcszFileName) = 0;
	virtual HRESULT SaveToConfigFile(LPCTSTR pcszFileName) = 0;

	virtual HRESULT GetChaseType(int* pChaseType) = 0;
	virtual HRESULT SetChaseType(int nChaseType) = 0;
	virtual HRESULT GetChaseRate(float* pChaseRate) = 0;
	virtual HRESULT SetChaseRate(float nChaseRate) = 0;
	virtual HRESULT GetObstructdAvert(int* pObstructdAvert) = 0;
	virtual HRESULT SetObstructdAvert(int nObstructdAvert) = 0;
	virtual HRESULT GetFlexCoefficient(float* FlexCoefficient) = 0;
	virtual HRESULT SetFlexCoefficient(float FlexCoefficient) = 0;
	virtual HRESULT GetDampCoefficient(float* DampCoefficient) = 0;
	virtual HRESULT SetDampCoefficient(float DampCoefficient) = 0;
	virtual HRESULT GetMaxAngelVel(float* MaxAngelVel) = 0;
	virtual HRESULT SetMaxAngelVel(float MaxAngelVel) = 0;
	virtual HRESULT GetMinAngelVel(float* MinAngelVel) = 0;
	virtual HRESULT SetMinAngelVel(float MinAngelVel) = 0;
	virtual HRESULT GetAngelRateHor(float* AngelRateHor) = 0;
	virtual HRESULT SetAngelRateHor(float AngelRateHor) = 0;
	virtual HRESULT GetAngelRateVel(float* AngelRateVel) = 0;
	virtual HRESULT SetAngelRateVel(float AngelRateVel) = 0;
	virtual HRESULT GetMaxDistance(float* MaxDistance) = 0;
	virtual HRESULT SetMaxDistance(float MaxDistance) = 0;
    virtual HRESULT GetDisZoomRate(float *fRate) = 0;
    virtual HRESULT SetDisZoomRate(float fRate) = 0;
};

interface IEKG3DTrackCamera
{
	//virtual HRESULT Update(D3DXVECTOR3 vecTargetPos, D3DXVECTOR3 vecTargetDir, float fTargerSpeed, DWORD dwTargetMoveType ) = 0;
	//virtual HRESULT GetDistance(float* fDistance) = 0;
	virtual HRESULT GetStation(IEKG3DCameraStation** pStation) = 0;
};

#endif //_INCLUDE_IEOTHER_H_
