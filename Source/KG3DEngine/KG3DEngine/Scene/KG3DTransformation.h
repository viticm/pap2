#pragma once


class KG3DTransformation
{
public:
	D3DXVECTOR3          m_ScalingCenter;
	D3DXQUATERNION       m_ScalingRotation;
	D3DXVECTOR3          m_Scaling;
	D3DXVECTOR3          m_RotationCenter;
	D3DXQUATERNION       m_Rotation;
	D3DXVECTOR3          m_Translation;
public:
	D3DXMATRIX            m_Matrix;

    static const D3DXQUATERNION IdentiQuaternion;
	BOOL   m_bSleep;
public:
	const D3DXMATRIX& GetMatrix();
	VOID	SetMatrix(const D3DXMATRIX& mat);

    void SetTranslation(const D3DXVECTOR3* pValue);
    void SetRotation(const D3DXQUATERNION* pValue);
	void SetRotationCenter(const D3DXVECTOR3* pValue);
	void SetScaling(const D3DXVECTOR3* pValue);
	void SetScalingRotation(const D3DXQUATERNION* pValue);
	void SetScalingCenter(const D3DXVECTOR3* pValue);

	void GetTranslation(D3DXVECTOR3* pValue);
	D3DXVECTOR3 GetTranslation(){return m_Translation;}
	void GetRotation(D3DXQUATERNION* pValue);
	void GetRotationCenter(D3DXVECTOR3* pValue);
	void GetScaling(D3DXVECTOR3* pValue);
	void GetScalingRotation(D3DXQUATERNION* pValue);
	void GetScalingCenter(D3DXVECTOR3* pValue);

	void UpdateTransformation();

    void NormalizeRotation();

    void ReSet();

	//BOOL IsTransformated();
	//void SetTransformated(BOOL bTran);

    virtual void UpdateByMatrix(const D3DXMATRIX& mat);

	KG3DTransformation operator + ( CONST KG3DTransformation& A);
	KG3DTransformation operator - ( CONST KG3DTransformation& A);
	KG3DTransformation operator * ( FLOAT A);
	KG3DTransformation operator / ( FLOAT A);

    void Interpolation(KG3DTransformation &NextTrans, float fFactor);

	KG3DTransformation(void);
	virtual ~KG3DTransformation(void);



};
