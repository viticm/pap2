#ifndef CASE_ACTION_OBJECT_KRLSCENEOBJECT_H
#define CASE_ACTION_OBJECT_KRLSCENEOBJECT_H

#include "KG3DEngine/KG3DInterface.h"

class KSceneObject;

class KRLSceneObject : public IKG3DAnimationEventHandler
{
public:
	enum { INVALID_OBJECT_ID = UINT_MAX };

    enum
    {
        FADE_NONE,
        FADE_IN,
        FADE_OUT,
		FADE_OUT_END,
    };

public:
	KRLSceneObject();
	virtual ~KRLSceneObject();

	BOOL IsFadeOut() const;
	BOOL IsFinished() const;

	DWORD GetObjectID() const;
	DWORD GetObjectType() const;

	HRESULT AttachObject(DWORD dwObject);
	HRESULT SetObjectType(DWORD dwObjectType);

	KSceneObject* GetGameWorldSceneObject() const;

	void SetPosition(D3DXVECTOR3 const& vPosition);
	void GetPosition(D3DXVECTOR3& vPosition) const;

	void SetDirectionLH(float fDirectionLH);
	float GetDirectionLH() const;

	void SetDirectionLH(D3DXQUATERNION const& qDirectionLH);
	
	HRESULT SetAlpha(float fAlpha);
    HRESULT GetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const;
    
	void FadeIn();
	void FadeOut();
    void SetFadeAlpha(float fAlpha);
	void UpdateFade();

protected:
	virtual HRESULT OnPositionChange(D3DXVECTOR3 const& vPosition) = 0; // TODO: remove
	virtual HRESULT OnRotationLHChange(D3DXQUATERNION const& qRotationLH) = 0; // TODO: remove
    virtual HRESULT OnAlphaChange(float fAlpha) = 0;
    virtual HRESULT OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const = 0;

private:
	DWORD			m_dwObject;
	DWORD			m_dwObjectType;

	float			m_fDirectionLH;

	D3DXVECTOR3		m_vPosition;		// 插值后的当前值

    float           m_fFadeAlpha;
    int				m_nFadeType;

	int				m_nRoadPath;
	int				m_nRoadPathPosition;
	static int		ms_nRoadPath;
};

#endif // CASE_ACTION_OBJECT_KRLSCENEOBJECT_H