////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBaseCoordImp.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-27 14:19:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DBASECOORDIMP_H_
#define _INCLUDE_KG3DBASECOORDIMP_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DBaseCoord.h"
#include "KG3DMessenger.h"
#include "KG3DMessageBroadcasterGroup.h"
#include "KGStateMachine.h"
#include "IEEditorTypes.h"


class KG3DBaseCoordImp : public KG3DBaseCoord, public KG3DMessageReceiver
{
public:
	STDMETHOD_(ULONG, GetType)() = 0;
	STDMETHOD(Init)();	//子类要转调这个函数
	virtual	ULONG GetActivationState() = 0;	//什么状态下激活
	virtual D3DXMATRIX GetTrans() = 0;
	STDMETHOD(Render)() = 0;

	STDMETHOD(FrameMove)();
	

	virtual ULONG	GetCurStateID();
	virtual HRESULT SetPosition(const D3DXVECTOR3& Pos);
	virtual HRESULT	Zoom(FLOAT zoomFactor);//zoomFactor会加到Scale系数里面，而不是乘

	virtual void __stdcall OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageReceiverBase* TestGetReceiverInterface(){return this;}
#endif

	virtual ~KG3DBaseCoordImp();
protected:
	KG3DBaseCoordImp(KG3DMessageBroadcasterGroup& Sc);

	
	STDMETHOD(UnInit)();

	struct RepresentData;
	struct ResultData;
	struct InputData;

	static FLOAT GetSelectorRingRadius(const RepresentData& data);

	static VOID RenderIntersectMesh(RepresentData& RpData, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);

	static BOOL IntersectMeshAndFindPoint(const RepresentData& RpData, const InputData& IpData, D3DXVECTOR3& vInter, KG3DCOORD& IntersectedCoord);

	static BOOL IntersectPlaneAndFindPoint(KG3DCOORD WhichCoord, const D3DXVECTOR3& Center, const D3DXVECTOR3& vSrc
		, const D3DXVECTOR3& vDir, D3DXVECTOR3& vInter);

	static D3DXVECTOR3 RefineInterPoint(const RepresentData& rpData, const D3DXVECTOR3& vInter, const ResultData& retData);

protected:
	DWORD       m_dwMode;			//世界坐标还是局部坐标

	struct RepresentData			//资源全是Manager型的不需要管设备丢失
	{
		//全是Managed型的Mesh
		LPD3DXMESH MeshForAxis[3];
		LPD3DXMESH MeshForPlane[3];
		LPD3DXMESH MeshForIntegration;	//一致放缩之类用的面

		FLOAT	zoomFactor;//放缩整个坐标圈圈的表现
		D3DXVECTOR3	vecPos;	
		D3DXVECTOR3 CamPos;
		
		INT			bFilterInter;

		RepresentData();
		~RepresentData();
		VOID ReleaseMeshes();
	};

	RepresentData m_RepresentData;

	struct InputData
	{
		BOOL		IsActivated;
		D3DXVECTOR3 vMouseRaySrc;
		D3DXVECTOR3 vMouseRayNormalizedDir;
		InputData(){ZeroMemory(this,sizeof(*this));}
	};

	InputData m_InputData;

	struct ResultData 
	{
		KG3DCOORD	IntersectedCoord;
		D3DXVECTOR3	StartDir;
		D3DXVECTOR3 LastDir;
		FLOAT		Angle;

		D3DXVECTOR3 StartPos;
		D3DXVECTOR3 EndPos;

		ResultData()
			:StartDir(1,0,0)
			,LastDir(0,0,1)
			,IntersectedCoord(KG3DCOORD_INVALID)
			,Angle(0)
		{

		}
	};

	ResultData m_ResultData;

	enum
	{
		em_mesh_inner_radius = 10,
		em_mesh_range = 200,
		em_mesh_plane_range = 60,	//大约是em_mesh_range的1/3
		em_mesh_sides = 4,
		em_mesh_rings = 16,

		em_selplane_inner_edge = 100,
		em_selplane_outter_edge = 125,

		em_state_check_gap = 40,//毫秒

		em_filter_none = 0,	//控制碰撞过滤
		em_filter_front = 1,
		em_filter_back = 2,
	};

	KG3DMessage m_messageCur;

	//状态机驱动,原理和状态机图完全一致,根据一个输入,改变当前状态或者保持状态
protected:
	struct CustomStateMachineState : public KGStateMachineState
	{
		virtual ULONG STDMETHODCALLTYPE Release(){return 1;}//静态类，不需要Release
		virtual KGStateMachineState* Clone(DWORD_PTR){return this;}
		virtual DWORD CheckInput(KG3DBaseCoordImp& instance) = 0;
		virtual DWORD CheckInput(DWORD_PTR Input)
		{
			_ASSERTE(NULL != Input);
			return CheckInput(*reinterpret_cast<KG3DBaseCoordImp*>(Input));
		}
	};

	KGStateMachine*	m_pStateMachine;

	struct StateNotActivated : public CustomStateMachineState
	{
		virtual DWORD	GetStateID();
		virtual DWORD	CheckInput(KG3DBaseCoordImp& instance);
	};

	struct StateActivated : public CustomStateMachineState
	{
		virtual DWORD	GetStateID();
		virtual DWORD	CheckInput(KG3DBaseCoordImp& instance);
	};

	struct StateTransforming : public CustomStateMachineState
	{
		virtual DWORD	GetStateID();
		virtual DWORD	CheckInput(KG3DBaseCoordImp& instance);
	};

	static KGStateMachine::BuildStruct GetStateMachineStateDesc();
	static const D3DXPLANE& GetPlaneIntergration();//得到用于统一放缩的那个Normal为1,1,1的平面
	
};


#endif //_INCLUDE_KG3DBASECOORDIMP_H_
