////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DBillboardCloudCreater.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-7-31 16:41:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DBILLBOARDCLOUDCREATER_H_
#define _INCLUDE_IEKG3DBILLBOARDCLOUDCREATER_H_

#include "../../../Include/KG3DEngine/KG3DTypes.h"

////////////////////////////////////////////////////////////////////////////////
enum { EM_BILLBOARD_CREATE_STOP
, EM_BILLBOARD_CREATE_INPUT_MESH
, EM_BILLBOARD_CREATE_PLANE
, EM_BILLBOARD_CREATE_CALC_UV
, EM_BILLBOARD_RENDER_TEXTURE };

interface IEKG3DBillBoardCloudCreater//<BillboardCloudV1>
{
	//virtual HRESULT CreateBillBoardCloudForModel() = 0;
	virtual HRESULT ComputeUV() = 0;
	virtual HRESULT ComputeTexture() = 0;
	virtual float GetPrecision() = 0;
	virtual void SetPrecision(float Precision) = 0;
	virtual DWORD GetBillBoardCount() = 0;
	virtual DWORD GetState() = 0;
	virtual void StartBillBoardCreating() = 0;  
	virtual void StopBillBoardCreating() = 0;   
	virtual BOOL IsTwitterPlane() = 0;
	virtual void EnableTwitterPlane(BOOL bEnable) = 0;
	virtual HRESULT SaveBillboardCloud() = 0;

	virtual int GetTextureWidth() = 0;
	virtual void SetTextureWidth(int nWidth) = 0;
};
#include "IEClassMemberDescription.h"
#pragma pack(push,4)
struct KG3DBillboardCloudCreaterThresholdStruct 
{
	FLOAT m_fMaxNormalBias;							//最大Normal偏移，拒绝合并
	FLOAT m_fMaxNormalDistanceBiasPercent;			//在Normal方向上偏移和边的百分比超过，拒绝合并，0以上，1代表和边一样大
	FLOAT m_fMaxDistanceToPoint;			//到点的距离大于绝对值
	FLOAT m_fMinAreaInfluenceAtNormalCorrection;	//面积如果太小，不影响Normal
	FLOAT m_fMinAreaAccepted;						//面积太小的三角形，扔掉

	IEClassMemberDescriptionPack	GetMemberDescrption()const
	{
		typedef KG3DBillboardCloudCreaterThresholdStruct MyType;
		static IEClassMemberDescription Desc[] = 
		{
			{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxNormalBias), NULL, _T("最大Normal偏移/弧度")},
			{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxNormalDistanceBiasPercent), NULL, _T("Normal方向最大距离偏移百分比")},
			{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMaxDistanceToPoint), NULL, _T("点到点最大距离偏移百分比")},
			{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMinAreaInfluenceAtNormalCorrection), NULL, _T("最小面积权值")},
			{KG3DTYPE_FLOAT, sizeof(FLOAT), offsetof(MyType, m_fMinAreaAccepted), NULL, _T("绝对最小面积")},
		};
		static IEClassMemberDescriptionPack Pack(Desc, _countof(Desc), this);
		return Pack;
	}

	friend bool operator==(const KG3DBillboardCloudCreaterThresholdStruct& A1
		, const KG3DBillboardCloudCreaterThresholdStruct& A2)
	{
		return 0 == memcmp(&A1, &A2, sizeof(KG3DBillboardCloudCreaterThresholdStruct));
	}
	friend bool operator!=(const KG3DBillboardCloudCreaterThresholdStruct& A1
		, const KG3DBillboardCloudCreaterThresholdStruct& A2)
	{
		return ! (A1 == A2);
	}
};
#pragma pack(pop)

enum KG3D_BILLBOARD_CLOUD_CT_OP
{
	KG3D_BILLBOARD_CLOUD_CT_OP_SHOW_PROCESS = 1,
};
interface IEKG3DModel;
interface IEKG3DBillboardCloudCreater2
{
	virtual HRESULT BuildForModel(IEKG3DModel* pModel) = 0;
	virtual HRESULT SetThresholdStruct(const KG3DBillboardCloudCreaterThresholdStruct* pIn) = 0;
	virtual HRESULT GetThresholdStruct(KG3DBillboardCloudCreaterThresholdStruct* pOut) = 0;
	virtual HRESULT UpdateShader() = 0;
	virtual ULONG GetOption() = 0;
	virtual HRESULT SetOption(DWORD Option) = 0;
	virtual HRESULT	StepCommand() = 0;
};

#endif //_INCLUDE_IEKG3DBILLBOARDCLOUDCREATER_H_
