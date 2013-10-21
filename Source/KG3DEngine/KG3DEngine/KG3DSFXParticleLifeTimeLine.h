////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXParticleLifeTimeLine.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 14:39:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXPARTICLELIFETIMELINE_H_
#define _INCLUDE_KG3DSFXPARTICLELIFETIMELINE_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DKeyFrameDataBase.h"
#include "KG3DStructs.h"
#include "iekg3dsfx.h"

class KG3DSFXParticleLifeTimeLine : public IEKG3DSFXParticleLifeTimeLine
{
public:
    KG3DSFXParticleLifeTimeLine();
    virtual ~KG3DSFXParticleLifeTimeLine(void);

public:
    TKG3DTimeLine<D3DXCOLOR>    m_ColorTimeLineL;
    TKG3DTimeLine<D3DXCOLOR>    m_ColorTimeLineR;
    TKG3DTimeLine<float>        m_fWidthTimeLine;
    TKG3DTimeLine<float>        m_fHeightTimeLine;

	TKG3DTimeLine<float>		m_fRotationTimeLineMin;
	TKG3DTimeLine<float>		m_fRotationTimeLineMax;

    // NOTE: If the texture frame index time line is zero, 
    // then we use random texture frame index.
    TKG3DTimeLine<int>          m_nTextureFrameTimeLine;

    UINT    m_uTextureIndex;
    float   m_fRotationFactor;

    WORD    m_wType;
    WORD    m_wFlag;
    DWORD   m_dwSmooth;
    


    void ScanlTimeLine(float scanl) {
        m_ColorTimeLineL.Scanl(scanl);
        m_ColorTimeLineR.Scanl(scanl);
        m_fWidthTimeLine.Scanl(scanl);
        m_fHeightTimeLine.Scanl(scanl);
        m_fRotationTimeLineMin.Scanl(scanl);
        m_fRotationTimeLineMax.Scanl(scanl);
        m_nTextureFrameTimeLine.Scanl(scanl);
    }

    int equal(KG3DSFXParticleLifeTimeLine* othr) {
        if (!this->m_ColorTimeLineL.equal(&othr->m_ColorTimeLineL))
            return false;
        if (!this->m_ColorTimeLineR.equal(&othr->m_ColorTimeLineR))
            return false;
        if (!this->m_fWidthTimeLine.equal(&othr->m_fWidthTimeLine))
            return false;
        if (!this->m_fHeightTimeLine.equal(&othr->m_fHeightTimeLine))
            return false;
        if (!this->m_fRotationTimeLineMin.equal(&othr->m_fRotationTimeLineMin))
            return false;
        if (!this->m_fRotationTimeLineMax.equal(&othr->m_fRotationTimeLineMax))
            return false;
        if (!this->m_nTextureFrameTimeLine.equal(&othr->m_nTextureFrameTimeLine))
            return false;
        if (this->m_uTextureIndex != othr->m_uTextureIndex)
            return false;
        if (this->m_fRotationFactor != othr->m_fRotationFactor)
            return false;
        if (this->m_wType != othr->m_wType)
            return false;
        if (this->m_wFlag != othr->m_wFlag)
            return false;
        return true;
    }

    KG3DSFXParticleLifeTimeLine* clone() {
        KG3DSFXParticleLifeTimeLine* pClone = new KG3DSFXParticleLifeTimeLine();
        pClone->m_ColorTimeLineL = this->m_ColorTimeLineL;
        pClone->m_ColorTimeLineR = this->m_ColorTimeLineR;
        pClone->m_fWidthTimeLine = this->m_fWidthTimeLine;
        pClone->m_fHeightTimeLine = this->m_fHeightTimeLine;
        pClone->m_fRotationTimeLineMin = this->m_fRotationTimeLineMin;
        pClone->m_fRotationTimeLineMax = this->m_fRotationTimeLineMax;
        pClone->m_nTextureFrameTimeLine = this->m_nTextureFrameTimeLine;
        pClone->m_uTextureIndex = this->m_uTextureIndex;
        pClone->m_fRotationFactor = this->m_fRotationFactor;
        pClone->m_wType = this->m_wType;
        pClone->m_wFlag = this->m_wFlag;
        return pClone;
    }

    virtual WORD GetParticleType()  { return m_wType; }
    virtual WORD GetParticleFlag()  { return m_wFlag; }

    virtual void SetParticleType(const WORD wType);
    virtual void SetParticleFlag(const WORD wFlag) { m_wFlag = wFlag; }
    virtual DWORD GetSmooth() { return m_dwSmooth; }
    virtual void  SetSmooth(DWORD dw) { m_dwSmooth = dw; }


	virtual UINT GetTextureIndex()
	{
		return m_uTextureIndex;
	}
	virtual void SetTextureIndex(UINT uValue)
	{
		m_uTextureIndex = uValue;
	}

	virtual float GetRotationFactor()
	{
		return m_fRotationFactor;
	}
	virtual void  SetRotationFactor(float fValue)
	{
		m_fRotationFactor = fValue;
	}
	/*virtual HRESULT WidthTimeLineGetData(float* pValue,float fFrame)
	{
		return m_fWidthTimeLine.GetData(pValue,fFrame);
	}
	virtual HRESULT HeightTimeLineGetData(float* pValue,float fFrame)
	{
		return m_fHeightTimeLine.GetData(pValue,fFrame);
	}
	virtual HRESULT RotationTimeLineGetData(float* pValue,float fFrame)
	{
		return m_fRotationTimeLine.GetData(pValue,fFrame);
	}
	virtual HRESULT ColorTimeLineGetData(D3DXCOLOR* pValue,float fFrame)
	{
		return m_ColorTimeLine.GetData(pValue,fFrame);
	}
	virtual HRESULT TextureFrameTimeLineGetData(int* pValue,float fFrame)
	{
		return m_nTextureFrameTimeLine.GetData(pValue,fFrame);
	}*/

	virtual TKG3DTimeLinebase* GetColorTimeLineL()
	{
		return &m_ColorTimeLineL;
	}
    virtual TKG3DTimeLinebase* GetColorTimeLineR()
    {
        return &m_ColorTimeLineR;
    }
	virtual TKG3DTimeLinebase* GetWidthTimeLine()
	{
		return &m_fWidthTimeLine;
	}
	virtual TKG3DTimeLinebase* GetHeightTimeLine()
	{
		return &m_fHeightTimeLine;
	}
	virtual TKG3DTimeLinebase* GetRotationTimeLineMin()
	{
		return &m_fRotationTimeLineMin;
	}
    virtual TKG3DTimeLinebase* GetRotationTimeLineMax()
    {
        return &m_fRotationTimeLineMax;
    }
	virtual TKG3DTimeLinebase* GetTextureFrameTimeLine()
	{
		return &m_nTextureFrameTimeLine;
	}



};

#endif //_INCLUDE_KG3DSFXPARTICLELIFETIMELINE_H_
