////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXTRACK_H_
#define _INCLUDE_KG3DSFXTRACK_H_

#include "../../../Include/kg3dengine/KG3DInterface.h"
#include "IEKG3DSFX.h"
#include "KG3DTemplates.h"
#include "Scene/KG3DTransformation.h"

const DWORD SFX_NOT_BIND_TO_TRACK = 0xffffffff;

class KG3DSFX;

class KG3DSFXTrackData : public IKG3DResourceBase
{
public :
    HRESULT Init();
    HRESULT UnInit();
    HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
    HRESULT SaveToFile(const char cszFileName[],   unsigned uOption);
	
    virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	
    HRESULT RefreshOption(unsigned uOption);
    unsigned GetType();
    HRESULT PrepareRender(unsigned uOption); 
    HRESULT PostRender(); 
    HRESULT GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem);
    unsigned GetLastUseTime();
    unsigned GetUseFrequency();

protected :
    DWORD m_dwType;
    DWORD m_dwID;

    HRESULT OnReferenceZero();

public :
    ULONG m_ulRefCount;
    string m_scName;

    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject);

    void SetID(DWORD dwID);
    DWORD GetID();

	void SetType(DWORD dwType)
    {
        m_dwType = dwType;
    }

public :
    KG3DSFXTrackData();
    virtual ~KG3DSFXTrackData();

    D3DXMATRIX  *m_pKeyFrame;
    DWORD       m_dwFrameNum;
    DWORD       m_dwInterval;
};

class KG3DSFXTrack : public KG3DTransformation, public IEKG3DSFXTrack
{
public:
    KG3DSFXTrack(KG3DSFX *pSFX);
    ~KG3DSFXTrack();

    HRESULT LoadFromFile(const char cszFileName[]);

    void SetInterval(DWORD dwInterval);
    void GetTransform(float fCurrentFrame, D3DXMATRIX* pMatrix, float* pTrackFrame = NULL);

    HRESULT FrameMove();
    HRESULT Render();
    HRESULT RenderSelectMark();

    void GetAABBox(AABBOX *pBox);

    DWORD GetFrameNum() const;
    DWORD GetInterval() const;

    std::string GetName() const;

    // editor
    virtual BOOL IsHide()               { return m_nHide;            }
    virtual void Show(BOOL enable)      { m_nHide = enable;          }
    virtual LPCTSTR GetName();
    virtual void SetName(LPCTSTR szName){ m_scName = szName;         } 
 
    string GetDataName()                 { return m_pData->m_scName; };


	virtual void SetTranslation(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::SetTranslation(pValue);
	}

	virtual void SetRotation(D3DXQUATERNION* pValue)
	{
		KG3DTransformation::SetRotation(pValue);
	}

	virtual void SetRotationCenter(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::SetRotationCenter(pValue);
	}

	virtual void SetScaling(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::SetScaling(pValue);
	}

	virtual void SetScalingRotation(D3DXQUATERNION* pValue)
	{
		KG3DTransformation::SetScalingRotation(pValue);
	}

	virtual void SetScalingCenter(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::SetRotationCenter(pValue);
	}

	virtual void GetTranslation(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::GetTranslation(pValue);
	}

	virtual void GetRotation(D3DXQUATERNION* pValue)
	{
		KG3DTransformation::GetRotation(pValue);
	}

	virtual void GetRotationCenter(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::GetRotationCenter(pValue);
	}

	virtual void GetScaling(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::GetScaling(pValue);
	}

	virtual void GetScalingRotation(D3DXQUATERNION* pValue)
	{
		KG3DTransformation::GetScalingRotation(pValue);
	}

	virtual void GetScalingCenter(D3DXVECTOR3* pValue)
	{
		KG3DTransformation::GetScalingCenter(pValue);
	}

private :
    KG3DSFXTrackData*    m_pData;

    DWORD               m_dwFrameNum;
    DWORD               m_dwInterval;

    KG3DSFX             *m_pSFX;

    D3DXMATRIX          m_matWorld;

    // editor
    BOOL    m_nHide;
    std::string m_scName;
};

class KG3DSFXTrackTable : public TKG3DResourceManagerBase<KG3DSFXTrackData>
{
public:
    KG3DSFXTrackTable(void);
    virtual ~KG3DSFXTrackTable(void);

    HRESULT Init();
    HRESULT UnInit();
};

extern KG3DSFXTrackTable    g_cTrackTable;

#endif //_INCLUDE_KG3DSFXTRACK_H_
