////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _TKG3DTIMELINE_H_
#define _TKG3DTIMELINE_H_ 


#include "../../../Include/KG3DEngine/KG3DInterface.h"
#include "..\scene\kg3dtransformation.h"
#include "..\KG3DSFXFileDefine.h"


using namespace std;

#define SET_TRANS 0x01
#define SET_ROTAN 0x04
#define SET_SCALG 0x08


class TKG3DTimeLinebase : public IKG3DTimeLine
{

};

template <class TFormat> class TKG3DTimeLine : public TKG3DTimeLinebase
{
protected :
	map<int,TFormat> m_mapData;

public:
    TKG3DTimeLine<TFormat>& operator = (const TKG3DTimeLine<TFormat>& line)
    {
        m_mapData = line.m_mapData;
        return *this;
    }

    void Scanl(float scanl) {
        map<int, TFormat> mapTemp = m_mapData;
        m_mapData.clear();
        for (map<int,TFormat>::iterator i = mapTemp.begin(); 
            i != mapTemp.end(); ++i) 
        {
            int key = static_cast<int>(i->first * scanl);
            TFormat data = i->second;
            m_mapData[key] = data;
        }
    }

    void Clear()
    {
       m_mapData.clear();
    }

    size_t size()
    {
        return m_mapData.size();
    }

    TFormat* find(int nFrame)
    {
        map<int,TFormat>::iterator ifind = m_mapData.find(nFrame);

        if (ifind == m_mapData.end())
            return NULL;

        return &(ifind->second);
    }

    TFormat* GetDataByIndex(DWORD dwIndex, int& nFrame)
    {
        if( dwIndex < 0 || dwIndex >= m_mapData.size() )
            return NULL;

        map<int,TFormat>::iterator i = m_mapData.begin();
        advance( i, dwIndex );

        nFrame = i->first;
        return &(i->second);
        
    }


    HRESULT GetKeyFrameByIndex( int *pnFrame, TFormat *pData, DWORD dwIndex )
    {
        if( dwIndex >=0 && dwIndex < m_mapData.size() )
        {
            map<int,TFormat>::iterator i = m_mapData.begin();
            advance( i, dwIndex );
            (*pnFrame) = i->first;
            (*pData) = i->second;
            return S_OK;
        }
        else
        {
            return E_FAIL;
        }
    }

	HRESULT InsertKeyFrame(int nFrame,TFormat Data)
	{
		m_mapData[nFrame] = Data;
		return S_OK;
	}

	HRESULT RemoveKeyFrame(int nFrame)
	{
		map<int,TFormat>::iterator i =
			m_mapData.find(nFrame);
		if(i!=m_mapData.end())
		{
			m_mapData.erase(i);
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}

    HRESULT IsKeyFrame(int nFrame)
    {
        map<int, TFormat>::iterator i = m_mapData.find(nFrame);
        if (i != m_mapData.end())
            return S_OK;
        else
            return E_FAIL;
    }

    TFormat Interpolation(const TFormat& low, const TFormat& up, float factor) {
        return (TFormat)(low + (up - low) * factor);
    }

    /*int Interpolation(const int& low, const int& up, float factor) {
        return (int)(low + (up - low) * factor);
    }*/

	HRESULT GetData(TFormat* pValue,float fFrame) {
		int nFrame = 0;
		if(fFrame>=0)
			nFrame = (int) fFrame;
		else
			nFrame = (int) fFrame - 1;

		map<int,TFormat>::iterator i_Down;
		map<int,TFormat>::iterator i_Up;
		TFormat A,B;
		float TA,TB;

		if(m_mapData.size()==0)
			return E_FAIL;
		if(m_mapData.size()==1)
		{
			i_Up = m_mapData.begin();
			(*pValue) = (TFormat)i_Up->second;
			return S_OK;
		}
		else
		{
			i_Down = m_mapData.upper_bound(nFrame);
			if(i_Down==m_mapData.end())
			{
				i_Up = i_Down;
				i_Up--;

				(*pValue) = (TFormat)i_Up->second;
				return S_OK;
			}
			else if(i_Down==m_mapData.begin())
			{
				(*pValue) = (TFormat)i_Down->second;
				return S_OK;
			}
			else
			{
				i_Up = i_Down;
				i_Up--;

				A = i_Up->second;
				B = i_Down->second;
				TA = (float) i_Up->first;
				TB =  (float)i_Down->first;

                ASSERT(TB - TA > 0.1f);
				//if(abs(TA-TB)<0.00001)
				//{
				//	return E_FAIL;
				//}
                (*pValue) = Interpolation(A, B, (fFrame - TA) / (TB - TA));
				//(*pValue) = (TFormat)(A+(B-A)/(TB - TA) * (nFrame - TA));
				return S_OK;
			}
		}

       
	}


    HRESULT GetPrevFrameData(TFormat* pValue, int nFrame) {

        if(m_mapData.size()==0)
            return E_FAIL;

        map<int,TFormat>::iterator i_Up = m_mapData.upper_bound(nFrame);
        i_Up--;
        if (i_Up == m_mapData.end()) {
            return E_FAIL;
        }
        (*pValue) = (TFormat)i_Up->second;
        return S_OK;



    }

    int equal(TKG3DTimeLine<TFormat>* othr) {
        
        if (this->m_mapData.size() != othr->m_mapData.size())
            return false;

        map<int,TFormat>::iterator self_iter = this->m_mapData.begin();
        map<int,TFormat>::iterator othr_iter = othr->m_mapData.begin();

        while (self_iter != this->m_mapData.end()) {

            if (self_iter->first != othr_iter->first)
                return false;

            if (self_iter->second != othr_iter->second)
                return false;

            self_iter++;
            othr_iter++;
        }

        return true;
    }

	TKG3DTimeLine()
	{

	}

	virtual ~TKG3DTimeLine()
	{
		m_mapData.clear();
	}


public :

    // interface

    virtual HRESULT Insert(int nFrame, void* pValue)
    {
        TFormat data = *(TFormat*)pValue;
        return InsertKeyFrame(nFrame, data);
    }

    virtual HRESULT Remove(int nFrame) 
    {
        return RemoveKeyFrame(nFrame);
    }

    virtual HRESULT GetData(float fFrame, void* pValue)
    {
        return GetData((TFormat*)pValue, fFrame);
    }

    virtual size_t GetSize()
    {
        return m_mapData.size();
    }

    virtual HRESULT QueryByIndex(size_t uIndex, int* pFrame, void* pValue)
    {
        return GetKeyFrameByIndex(pFrame, (TFormat*)pValue, (DWORD)uIndex);
    }

	virtual HRESULT QueryByFrame(size_t uFrame, TFormat** ppData)
	{
		HRESULT hResult = E_FAIL;
		map<int,TFormat>::iterator it = m_mapData.find(static_cast<int>(uFrame));
		KG_PROCESS_ERROR(it != m_mapData.end());
		KG_PROCESS_ERROR(ppData);

		*ppData = &(it->second);

		hResult = S_OK;
Exit0:
		return hResult;
	}
};

D3DXQUATERNION TKG3DTimeLine<D3DXQUATERNION>::Interpolation(const D3DXQUATERNION& low, const D3DXQUATERNION& up, float factor) {
    D3DXQUATERNION q;
    D3DXQuaternionSlerp(&q, &low, &up, factor);
    return q;
}

struct KGRTSTimeLine {
    TKG3DTimeLine<D3DXVECTOR3>          m_TranslationTimeLine;
    TKG3DTimeLine<D3DXQUATERNION>       m_RotationTimeLine;
    TKG3DTimeLine<D3DXVECTOR3>          m_ScalingTimeLine;
    TKG3DTimeLine<D3DXQUATERNION>       m_ScalRotationTimLine;

    void InsertKeyFrame(const KG3DTransformation& t, int nFrame) {
        m_TranslationTimeLine.InsertKeyFrame(nFrame, t.m_Translation);
        m_RotationTimeLine.InsertKeyFrame(nFrame, t.m_Rotation);
        m_ScalRotationTimLine.InsertKeyFrame(nFrame, t.m_ScalingRotation);
        m_ScalingTimeLine.InsertKeyFrame(nFrame, t.m_Scaling);
    }

    void InsertKeyFrameSmark(const KG3DTransformation& tf, int nFrame, DWORD dwFlag = 0x0) {
        
        ASSERT(nFrame >= 0);

        D3DXQUATERNION q;
        D3DXQUATERNION u;
        D3DXVECTOR3 t;
        D3DXVECTOR3 s;

        if (dwFlag & SET_TRANS) {
            m_TranslationTimeLine.InsertKeyFrame(nFrame, tf.m_Translation);
        } else if (SUCCEEDED(m_TranslationTimeLine.GetPrevFrameData(&t, nFrame))) {
            if (t != tf.m_Translation) {
                 m_TranslationTimeLine.InsertKeyFrame(nFrame, tf.m_Translation);
            }
        } else {
            m_TranslationTimeLine.InsertKeyFrame(nFrame, tf.m_Translation);
        }

        if (dwFlag & SET_ROTAN) {
            m_RotationTimeLine.InsertKeyFrame(nFrame, tf.m_Rotation);
        } else if (SUCCEEDED(m_RotationTimeLine.GetPrevFrameData(&q, nFrame))) {
            if (q != tf.m_Rotation) {
                m_RotationTimeLine.InsertKeyFrame(nFrame, tf.m_Rotation);
            }
        } else {
            m_RotationTimeLine.InsertKeyFrame(nFrame, tf.m_Rotation);
        }
        
        if (dwFlag & SET_SCALG) {
            m_ScalingTimeLine.InsertKeyFrame(nFrame, tf.m_Scaling);
        } else if (SUCCEEDED(m_ScalingTimeLine.GetPrevFrameData(&s, nFrame))) {
            if (s != tf.m_Scaling) {
                m_ScalingTimeLine.InsertKeyFrame(nFrame, tf.m_Scaling);
            }
        } else {
            m_ScalingTimeLine.InsertKeyFrame(nFrame, tf.m_Scaling);
        }
        
        if (dwFlag & SET_SCALG) {
            m_ScalRotationTimLine.InsertKeyFrame(nFrame, tf.m_ScalingRotation);
        } else if (SUCCEEDED(m_ScalRotationTimLine.GetPrevFrameData(&u, nFrame))) {
            if (u != tf.m_ScalingRotation) {
                m_ScalRotationTimLine.InsertKeyFrame(nFrame, tf.m_ScalingRotation);
            }
        } else {
            m_ScalRotationTimLine.InsertKeyFrame(nFrame, tf.m_ScalingRotation);
        }
    }

    void Scanl(float fScal) {
        m_TranslationTimeLine.Scanl(fScal);
        m_RotationTimeLine.Scanl(fScal);
        m_ScalRotationTimLine.Scanl(fScal);
        m_ScalingTimeLine.Scanl(fScal);
    }
};

#endif  