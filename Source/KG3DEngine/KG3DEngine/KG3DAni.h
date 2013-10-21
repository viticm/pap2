#ifndef _INCLUDE_KG3DANI_H_
#define _INCLUDE_KG3DANI_H_

////////////////////////////////////////////////////////////////////////////////
#include "IEKG3DAni.h"
#include "KG3DAnimationStruct.h"

class KG3DAnimation;
class KG3DAnimationController;


class KG3DAni : public IEKG3DAni
{

public: // for IEKG3DAni interface
	enum
	{
		MAX_CLIPDESC_LEN = 30,
		MAX_BONENAME_LEN = 30,
	};
#pragma pack(push,1)  
	

	struct HeaderInfo
	{
		DWORD dwMask;
		DWORD dwBlockLength;
		DWORD dwNumAnimations;
		DWORD dwType;
		TCHAR strDesc[MAX_CLIPDESC_LEN];
		union
		{
			struct BoneAnimation
			{
				DWORD dwNumBones;
				DWORD dwNumFrames;
				float fFrameLength;
			} Bone;
			struct VertexAnimation
			{
				DWORD dwNumVertices;
				DWORD dwNumAnimatedVertices;
				DWORD dwNumFrames;
				float fFrameLength;
			} Vertex;
		}FrameInfo;
	};
#pragma pack(pop)

	virtual HRESULT OnInvalidDevice(){return S_OK;};
	virtual HRESULT OnRestoreDevice(){return S_OK;};

	virtual HRESULT LoadHeaderInfo(LPCSTR strFileName, HeaderInfo* pInfo);
	virtual HRESULT GetFrameLength(FLOAT *fRetLength);
	virtual HRESULT SetFrameLength(FLOAT fFrameLength);

	virtual HRESULT GetNumFrames(DWORD *pdwRetNum);                       

	virtual HRESULT GetName(LPCSTR *ppszName);

	virtual HRESULT NormalizeClipData(IEKG3DMesh* pMesh, BOOL bSaveToFile);
	virtual HRESULT NormalizeClipData(IEKG3DAni* pAni, BOOL bSaveToFile);


public:
	KG3DAni(void);
	~KG3DAni(void);

	//////////////////////////////////////////////////////////////////////////
	//IKG3DResourceBase methods
	virtual HRESULT Init();
	virtual HRESULT UnInit();

	virtual HRESULT LoadFromFile(
		const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption
		);

	HRESULT LoadFromFileMultiThread();
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}

	virtual HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	virtual unsigned GetType();
	virtual DWORD GetID();
	void SetID(DWORD dwID) {m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}

	virtual HRESULT PrepareRender(unsigned uOption); 
	virtual HRESULT PostRender(); 
	virtual HRESULT GetDateInformation(
		int *pnMemory, 
		int *pnVideoMemory,
		int *pnCompressMem,
		int *pnCompressVideoMem
		);
	virtual unsigned GetLastUseTime();
	virtual unsigned GetUseFrequency();
	//////////////////////////////////////////////////////////////////////////


public:
	int m_nReference;//物品使用计数
	std::string m_scName;
	//////////////////////////////////////////////////////////////////////////
	//IUnkown methods
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		REFIID riid, 
		void __RPC_FAR *__RPC_FAR *ppvObject
		);
	//////////////////////////////////////////////////////////////////////////

public:

	void ExchangePercentage2Time(float fPercentage, float& fTime);//根据百分比获得时间

	void Rename(const TCHAR* strNewName);

	HRESULT GetNumAnimatedBone(unsigned *puRetNum);

	HRESULT GetAnimatedBoneName(char ***pppAnimatedBoneName);

	HRESULT GetRTSPoint(RTS ** ppRTS);

	//Find a bone by name
	int FindBone(LPCTSTR strBoneName);

	//Swap bone animation data
	void SwapBoneAnimationData(int nBone1, int nBone2);

	//Update clip animation
	HRESULT UpdateClip(KG3DAnimationController* pController,
		const std::vector<int>* pBoneLODList, 
		KG3DAnimationUpdateExtraInfoCollector *pExtraInfo);

	//Interpolate RTS key frame
	void InterpolateRTSBoneKeyFrame(D3DXMATRIX& OutRelativeMat, RTS& FrameA, RTS& FrameB, float fWeight);

	float ConvertFrameToTime(UINT uFrameIndex);
	UINT TimeToFrame(float fTime, 
		float fSpeed, 
		UINT* pRepeatTimes);

private:


	DWORD m_dwType;//类型             
	DWORD m_dwID;

	virtual HRESULT OnReferenceZero();	

	//////////////////////////////////////////////////////////////////////////
	//KG3DClip Data
private:
	std::string  m_strClipName;          //动画名称
	DWORD   m_dwNumFrames;     //动画包含的桢数
	float   m_fFrameLength;   //动画桢之间的时间间隔

	//顶点动画
	DWORD        m_dwNumAnimatedVertice;      //动画顶点的数目
	DWORD*       m_pAnimatedVerticesIndex;   //动画顶点索引
	D3DXVECTOR3* m_pAnimatedVerticesPosition;//动画顶点的位置信息
	D3DXVECTOR3* m_pAnimationUV;


	//骨骼动画
	DWORD        m_dwNumAnimatedBone;         //动画骨骼的数目
	LPTSTR*       m_ppAnimatedBoneName;        //动画骨骼的名称

	D3DXMATRIX*  m_pAnimatedBoneMatrix;      //动画骨骼的变换（4X4矩阵）
	RTS*         m_pAnimatedBoneRTS;//Bone's rotation, translation & Scale


	DWORD m_dwNumVertices;
	DWORD m_dwOptions;
	const static DWORD m_dwMask = 0x414E494D;


private:

	//Update RTS bone animation
	HRESULT UpdateRTS(KG3DAnimationController* pController,
		const std::vector<int>* pBoneLODList,
		KG3DAnimationUpdateExtraInfoCollector *pExtraInfo);

	//Update world bone animation
	HRESULT UpdateWorld(KG3DAnimationController* pController,
		const std::vector<int>* pBoneLODList);

	//Update relative bone animation
	HRESULT UpdateRelative(KG3DAnimationController* pController,
		const std::vector<int>* pBoneLODList);

	//Update vertices animation
	HRESULT UpdateVertices(KG3DAnimationController* pController);


	//Update vertices relative animation
	HRESULT UpdateVerticesRelative(KG3DAnimationController* pController);

public:
	//Get Current frame and next frame
	void    GetCurFrame(
		KG3DAnimationController* pController, 
		int& nFrame, 
		int& nFrameTo,
		float& fWeight
		);

	int GetMatrixsRange(
		vector<D3DXMATRIX>& vecMatrixs,
		INT          nBoneIndex,
		FLOAT        fBeginFrame,
		FLOAT        fEndFrame,
		FLOAT        fPrecision
		);

public:
	//static members
	static void InterpolateRTSKeyFrame(RTS& FrameResult, 
		const RTS& FrameA, 
		const RTS& FrameB, 
		float fWeight);

	static void RTS2Matrix(D3DXMATRIX& matResult,
		const RTS& RtsSrc,
		DWORD dwFlag);

	static void RTS2Matrix(D3DXMATRIX& matResult,
		const RTS& RtsSource);

	static void RTSWeighted(RTS& Frame, float fWeight);

	static void ComposeRTS(RTS &rtsResult,
		RTS *pRTS,
		DWORD *pFlags,
		DWORD dwNumRTS);

	static void ComposeRTS(RTS &rtsResult,
		RTS **ppRTS,
		DWORD *pFlags,
		DWORD dwNumRTS);

	enum enuLoadState
	{
		STATE_NONE,
		STATE_FAILED,
		STATE_READY,
	};

	enum enuLoadOption
	{
		LOAD_NORMAL,
		LOAD_MULTITHREAD,
	};

	virtual HRESULT CheckLoadState();

	enuLoadState m_eDynamicLoadState;

};


#endif //_INCLUDE_KG3DANI_H_
