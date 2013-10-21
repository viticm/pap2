////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DClip.h
//  Version     : 1.0
//  Creator     : huang shan
//  Create Date : 2006-11-2 13:47:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCLIP_H_
#define _INCLUDE_KG3DCLIP_H_

////////////////////////////////////////////////////////////////////////////////
#include "IEKG3DClip.h"
#include "KG3DAnimationStruct.h"
#include "KG3DAnimationController.h"


class KG3DClip : public IEKG3DClip
{
public: // for IEKG3DClip interface
    virtual HRESULT GetFrameLength(FLOAT *fRetLength);
    virtual HRESULT SetFrameLength(FLOAT fFrameLength);
    virtual HRESULT GetNumFrames(DWORD *pdwRetNum);
    virtual HRESULT GetName(LPCSTR *ppszName);
    virtual HRESULT Rename(const TCHAR* strNewName);
    virtual HRESULT GetPathName(LPCTSTR *ppszPath);
    virtual HRESULT SetPathName(const TCHAR *strPath);

public:
    KG3DClip(void);
    ~KG3DClip(void);

    //////////////////////////////////////////////////////////////////////////
    //IUnkown methods
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid, 
        void __RPC_FAR *__RPC_FAR *ppvObject
    );
    //////////////////////////////////////////////////////////////////////////

    std::string m_scName;

    DWORD GetMemSize()          { return m_uTotalMemSize;       }
    DWORD GetLength()           { return m_dwAniLen;            }
    DWORD GetLastUseTime()      { return m_dwLastUseTime;       }
    DWORD GetID()               { return m_dwID;                }
    DWORD GetType()             { return m_dwType;              }
    DWORD GetNumAnimatedBone()  { return m_dwNumAnimatedBone;   }
    void SetID(DWORD dwID) { m_dwID = dwID; }
    void SetType(DWORD dwType) { m_dwType = dwType; }
    int FindBone(LPCTSTR strBoneName);
    void SwapBoneAnimationData(int nBone1, int nBone2);
    DWORD ConvertFrameToTime(UINT uFrameIndex);
    HRESULT SaveToFile(const char cszFileName[], unsigned uOption);
    HRESULT LoadFromFile(const char cszFileName[], unsigned uLoadOption);
    HRESULT LoadFromFileMultiThread();
    HRESULT PreInterpolate();
    HRESULT UpdateClip(
        UpdateAnimationInfo *pAnimationInfo,
        KG3DAnimationUpdateExtraInfoCollector *pExtraInfo,
        DWORD dwFrame,
        DWORD dwFrameTo,
        float fWeight
    );

private:
    enum
    {
        MAX_CLIPDESC_LEN = 30,
        MAX_BONENAME_LEN = 30,
    };
#pragma pack(push,1)
    struct _ANI_FILE_HEADER
    {
        DWORD dwMask;
        DWORD dwBlockLength;
        DWORD dwNumAnimations;
        DWORD dwType;
        TCHAR strDesc[MAX_CLIPDESC_LEN];
    };
    struct _BONE_ANI
    {
        DWORD dwNumBones;
        DWORD dwNumFrames;
        float fFrameLength;
    };
    struct _VERTEX_ANI
    {
        DWORD dwNumVertices;
        DWORD dwNumAnimatedVertices;
        DWORD dwNumFrames;
        float fFrameLength;
    };
#pragma pack(pop)

    ULONG m_ulRefCount;
    DWORD m_dwType;
    DWORD m_dwID;
    DWORD m_dwOptions;
    BYTE *m_pbyFileData;
    unsigned m_uFileDataSize;
    std::string     m_strClipName;      //动画名称
    DWORD           m_dwNumFrames;      //动画包含的桢数
    float           m_fFrameLength;     //动画桢之间的时间间隔
    DWORD           m_dwAniLen;

    //顶点动画
    DWORD        m_dwNumVertices;
    DWORD        m_dwNumAnimatedVertice;     //动画顶点的数目
    DWORD*       m_pAnimatedVerticesIndex;   //动画顶点索引
    D3DXVECTOR3* m_pAnimatedVerticesPosition;//动画顶点的位置信息
    D3DXVECTOR3* m_pAnimationUV;

    //骨骼动画
    DWORD   m_dwNumAnimatedBone;         //动画骨骼的数目
    char **m_ppszBoneName;
    RTS  **m_ppBoneRTS;

    //for Bone Animation Pre-Computation
    D3DXMATRIX **m_ppBoneMatrixPreCompute;
    RTS **m_ppBoneRTSPreCompute;
    BOOL m_bPreInterpolateFlag;
    DWORD m_dwInterpolateFrames;
    DWORD m_dwExpectFrames;

    DWORD m_dwLastUseTime;
    unsigned m_uTotalMemSize;

private:
    HRESULT Clear();

    //for Bone Animation Pre-Computation
    HRESULT PreInterpolateBoneAnimation(float fInterpolateFrameLength);

    HRESULT AllocateMemforPreInterpolate(float fInterpolateFrameLength);

    //Update RTS bone animation (for precomputation)
    HRESULT UpdateRTSPre(UpdateAnimationInfo *pAnimationInfo,
        DWORD dwFrame,
        DWORD dwFrameTo,
        float fWeight,
        KG3DAnimationUpdateExtraInfoCollector *pExtraInfo);

    //Update RTS bone animation (for real-time computation)
    HRESULT UpdateRTSRealTime(UpdateAnimationInfo *pAnimationInfo,
        DWORD dwFrame,
        DWORD dwFrameTo,
        float fWeight,
        KG3DAnimationUpdateExtraInfoCollector *pExtraInfo);

    //Update vertices animation
    HRESULT UpdateVertices(UpdateAnimationInfo *pAnimationInfo, 
        DWORD dwFrame,
        DWORD dwFrameTo,
        float fWeight);

public:
    HRESULT GetCurFrame(
        DWORD dwAniPlayLen, DWORD dwPlayType, 
        DWORD *pdwFrame, DWORD *pdwFrameTo, float *pfWeight
    );

    int GetMatrixsRange(vector<D3DXMATRIX>& vecMatrixs,
        INT     nBoneIndex,
        FLOAT   fBeginFrame,
        FLOAT   fEndFrame,
        FLOAT   fPrecision
    );

public:
    //static members
    static void InterpolateRTSKeyFrame(RTS& FrameResult, const RTS& FrameA, const RTS& FrameB, float fWeight);
    static void RTS2Matrix(D3DXMATRIX& matResult, const RTS& RtsSource);
    static void ComposeRTS(RTS &rtsResult, RTS **ppRTS, DWORD *pFlags, DWORD dwNumRTS);

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

    BOOL IsLoad() { return (m_eDynamicLoadState == STATE_READY); }

    enuLoadState m_eDynamicLoadState;
    BOOL  m_bInMultiThreadLoadList;
};

#endif //_INCLUDE_KG3DCLIP_H_
