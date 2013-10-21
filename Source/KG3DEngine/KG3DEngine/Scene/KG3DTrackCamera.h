////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTrackCamera.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-18 15:24:45
//  Comment     : 
//  Update      : 2006-12-12 14:17:59 (Chiyer)
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DTRACKCAMERA_H_
#define _INCLUDE_KG3DTRACKCAMERA_H_

#include "KG3DCamera.h"
#include "IEOther.h"	//by dengzhihui Refactor 2006年11月6日
#include "KG3DGraphicsStructEx.h"

class KG3DTrackCamera : public KG3DCamera, public IEKG3DTrackCamera //by dengzhihui Refactor 2006年11月6日 谨慎添加指针成员变量，因为用了默认赋值函数 by liumingod
{
public :
	virtual HRESULT GetStation(IEKG3DCameraStation** pStation);//by dengzhihui Refactor 2006年11月6日

    // 绑定到场景(在做遮档检测时需要从当前场景中取得地表信息)
    virtual HRESULT BindScene(KG3DScene* lpCurScene);

    // 做遮档检测和更新Camera信息(View Matrix)
    virtual HRESULT FrameMove();

     // 对外接口 : 更新相机位置, 需要更新相机位置时调用
    virtual HRESULT UpdateCamera(                  
        D3DXVECTOR3 vecPosition,             // -- 你期待的更新位置
        D3DXVECTOR3 vecTargetPos,              // -- 轴心位置
        float       fLookAtOffset = 100.0f,  // -- LookAt点到轴心位置的偏移值
        float       fTargetSpeed  = 800.f    // -- 目标点移动速度
        );

    // 设置 / 得到 Camera 的当前属性 
    virtual HRESULT SetTrackInfo(TrackCameraInfo  trackInfo);
    virtual HRESULT GetTrackInfo(TrackCameraInfo* pTrackInof);

    // 得到当前 Camera 的状态　查看 ENUM_CAMERA_STATE
    virtual DWORD GetCurrentState();

	// 模拟客户端操作
	HRESULT UpdateAsClient(D3DXVECTOR3 vecTargetPos, D3DXVECTOR3 vecTargetDir
		, float fTargerSpeed, DWORD dwTargetMoveType);
	
	virtual IKG3DCamera* GetNew();
	virtual HRESULT Copy(IKG3DCamera* pCamera);
	
	virtual KG3DTrackCamera* GetTrackCameraInterface(){return this;}	
	LRESULT	ProcessMouseWeel(WPARAM wParam, LPARAM lParam);	

    KG3DTrackCamera(void);
    virtual ~KG3DTrackCamera(void);
private:
	// 遮档避免
	HRESULT ObstructdAvert(D3DXVECTOR3* pVecCameraPos, D3DXVECTOR3* pVecTargerPos);

	/*
	为了方便引擎使用才做这个函数的，引擎内，碰撞用::RayIntersection
    求从VS到VD的线段与场景相交的交点中最靠近VS的一个
	*/
    //BOOL GetCrossPosition(const D3DXVECTOR3& VS, const D3DXVECTOR3& VD, D3DXVECTOR3* pVecCrossPos);

    virtual BOOL GetCrossPosition(
        const D3DXVECTOR3 vSrc[], 
        const D3DXVECTOR3 vDst[],
        int   nCount,
        D3DXVECTOR3* pVecCrossPos
        ); //最后的结果会用第一组的Src和vDes和最近的交点距离Src的距离得到

	LRESULT	ProcessMouseMove(POINT& prevPoint, POINT& currPoint);		
	//HRESULT ProcessLButtonDn(WPARAM wParam, LPARAM lParam);
	//HRESULT ProcessLButtonUp(WPARAM wParam, LPARAM lParam);
	HRESULT SetDistance(float fDistance);
	HRESULT SetAngleHor(float fAngleHor);
	HRESULT SetAngleVel(float fAngleVel);

	float   GetDistance();
	float   GetAngleHor();
	float   GetAngleVel();

	HRESULT AddDistance(float fDistance);
	HRESULT AddAngleHor(float fAngleHor);
	HRESULT AddAngleVel(float fAngleVel);
	HRESULT ZoomDistance(float fMultiple);
private:
	D3DXVECTOR3		m_vecCurPostion;        // 当前位置
	D3DXVECTOR3		m_vecPerfectPos;        // 理想位置  
	D3DXVECTOR3		m_vecSavedPePos;        // 前一帧的理想位置

	D3DXVECTOR3		m_vecTagPostion;        // 轴心位置
	D3DXVECTOR3     m_vecLookAtPos;         // 观察位置
	D3DXVECTOR3		m_vecPrevTagPos;        // 前一帧的轴心位置

	D3DXVECTOR3     m_vecCurVelocity;       // 当前做弹簧运动的速度

	int             m_nUseFlexFlag;         // 是否要做弹簧运动(在出现遮档后，此标志设为True)
	int             m_nUpdateFlag;          // 需要更新相机位置标志(UpdateCamera函数被调用后设置为True)
	int				m_nObstrucFlag;         // 遮档标志(出现遮档后设置为True)

	float           m_fDisPreTarger;        // 理想位置到轴心位置的长度
	float           m_fFlexOffset;          // 相机到轴心的长度拉伸了的长度
	float			m_fStartVelocity;		// 启始速度

	DWORD           m_dwState;              // 当前的状态, 查看 ENUM_CAMERA_STATE

	float           m_fZNearSave;           

	//
	D3DXVECTOR3     m_vecPositionSave;               
	D3DXVECTOR3     m_vecAxesPosSave;           
	//

	TrackCameraInfo m_trackInfo;            


	KG3DCameraSphere            m_BoundSphere;
private :
    D3DXVECTOR3		m_vecTagDirectn;

	float			m_fDistance;
	float			m_fAngleHor;
	float			m_fAngleVel;
	float			m_fAnVarHor;
	float			m_fAnVarVel;

	float			m_fUpOffset;
    float           m_fVelGlide;

	float			m_fAnglVelocity;
    float           m_nFlexLength;  

	int				m_nRegressFlag;
	int				m_nObTerrainFg;

public:
	struct _Station
	{
        KG3DTrackCamera* pCamera;

        float       fFlexCoefficient;   // 弹性系数  
        float       fDampCoefficient;   // 阻尼系数

        int			bObstructdAvert;	// 是否遮档检测　
        int         bUseFlexibilitySys; // 使用弹簧系统

		int			nChaseType;		    // 追尾类型 0:从不 1:总是 2:智能
        float       fChaseRate;         // 追尾速度

		float		fMaxDistance;		// 与主角的最大距离
		float		fMinDistance;		// 与主角的最近距离

		float		fMaxAngelVel;		// 竖值旋转最大角度
		float		fMinAngelVel;		// 竖值旋转最小角度

		float		fAngelRateHor;		// 水平旋转敏捷度
		float		fAngelRateVel;		// 竖值旋转敏捷度

		float		fDisZoomRate;		// 拉身敏捷度
        float       fFlexRate;          // 
       
		_Station();
		_Station& operator=(_Station& temp);

		int IniByConfigFile(const char pcszFileName[]);
		int SaveToConfigFile(const char pcszFileName[]);
	};

	//(by dengzhihui Refactor 2006年11月6日
	class CStation : public IEKG3DCameraStation
	{
	public:
		void Init(_Station* pStation){m_station = pStation;}
	private:
		_Station* m_station;

		virtual HRESULT IniByConfigFile(LPCTSTR pcszFileName);
		virtual HRESULT SaveToConfigFile(LPCTSTR pcszFileName);

		virtual HRESULT GetChaseType(int* pChaseType);
		virtual HRESULT SetChaseType(int nChaseType);
		virtual HRESULT GetChaseRate(float* pChaseRate);
		virtual HRESULT SetChaseRate(float nChaseRate);
		virtual HRESULT GetObstructdAvert(int* pObstructdAvert);
		virtual HRESULT SetObstructdAvert(int nObstructdAvert);
		virtual HRESULT GetFlexCoefficient(float* FlexCoefficient);
		virtual HRESULT SetFlexCoefficient(float FlexCoefficient);
		virtual HRESULT GetDampCoefficient(float* DampCoefficient);
		virtual HRESULT SetDampCoefficient(float DampCoefficient);
		virtual HRESULT GetMaxAngelVel(float* MaxAngelVel);
		virtual HRESULT SetMaxAngelVel(float MaxAngelVel);
		virtual HRESULT GetMinAngelVel(float* MinAngelVel);
		virtual HRESULT SetMinAngelVel(float MinAngelVel);
		virtual HRESULT GetAngelRateHor(float* AngelRateHor);
		virtual HRESULT SetAngelRateHor(float AngelRateHor);
		virtual HRESULT GetAngelRateVel(float* AngelRateVel);
		virtual HRESULT SetAngelRateVel(float AngelRateVel);
		virtual HRESULT GetMaxDistance(float* MaxDistance);
		virtual HRESULT SetMaxDistance(float MaxDistance);
		virtual HRESULT GetDisZoomRate(float *fRate);
		virtual HRESULT SetDisZoomRate(float fRate);
	};
private:
	_Station m_cameraStation;
	CStation m_StationHelper;

};

#endif //_INCLUDE_KG3DTRACKCAMERA_H_
