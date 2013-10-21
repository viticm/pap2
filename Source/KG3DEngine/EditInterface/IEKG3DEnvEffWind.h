////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DEnvEffWind.h
//  Version     : 1.0
//  Creator     : Li Kan
//  Create Date : 2009-12-3 10:55:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

typedef enum _KG3DWIND_MDO
{
	KG3DWIND_MDO_DEFAULT = 0x00,			//什么也不改
	KG3DWIND_MDO_DIRX = 1 << 0,				//风向x
	KG3DWIND_MDO_DIRY = 1 << 1,				//风向y
	KG3DWIND_MDO_DIRZ = 1 << 2,				//风向z
	KG3DWIND_MDO_SWAY = 1 << 3,		        //单顶点摆动方向
	KG3DWIND_MDO_AMPLITUDE1 = 1 << 4,		//震幅1
	KG3DWIND_MDO_AMPLITUDE2 = 1 << 5,		//震幅2
	KG3DWIND_MDO_WAVELEN1   = 1 << 6,		//波长1
	KG3DWIND_MDO_WAVELEN2   = 1 << 7,		//波长2
	KG3DWIND_MDO_SPEED1 = 1 << 8,			//风速1
	KG3DWIND_MDO_SPEED2 = 1 << 9,			//风速2
	KG3DWIND_MDO_SCALEUP1 = 1 << 10,			//波峰放大1
	KG3DWIND_MDO_SCALEUP2 = 1 << 11,		//波峰放大2
	KG3DWIND_MDO_SCALEDOWN1 = 1 << 12,		//波谷放大1
	KG3DWIND_MDO_SCALEDOWN2 = 1 << 13,		//波谷放大2
	KG3DWIND_MDO_OFFSET1 = 1 << 14,			//顶点跳跃1
	KG3DWIND_MDO_OFFSET2 = 1 << 15,			//顶点跳跃2
	KG3DWIND_MDO_ENABLEWAVE1 = 1 << 16,		//开启波1
	KG3DWIND_MDO_ENABLEWAVE2 = 1 << 17,		//开启波2
	KG3DWIND_MDO_ENABLEDIRWIND = 1 << 18,   //开启方向风
	KG3DWIND_MDO_ALL = 0xffffffff,			
} KG3D3DWIND_MDO;







typedef struct _KG3DWIND_DESCSTRUCT
{
	DWORD m_dwMidificationOption;
	int m_id;
	float m_WindDirX;
	float m_WindDirY;
	float m_WindDirZ;
	int   m_Sway;
	float m_WindAmplitude1;
	float m_WindAmplitude2;
	float m_WindWaveLen1;
	float m_WindWaveLen2;
	float m_WindSpeed1;
	float m_WindSpeed2;
	float m_WindWaveScaleUp1;
	float m_WindWaveScaleUp2;
	float m_WindWaveScaleDown1;
	float m_WindWaveScaleDown2;
	float m_WindWaveOffset1;
	float m_WindWaveOffset2;
	bool  m_bEnableWave1;
	bool  m_bEnableWave2;
	bool  m_bEnableDirWind;

	void SetDefaultValue()
	{
		m_WindDirX = 1.0f;
		m_WindDirY = 1.0f;
		m_WindDirZ = 1.0f;
		m_Sway = 0;
		m_WindAmplitude1 = 0.5f;
		m_WindAmplitude2 = 0.8f;
		m_WindWaveLen1 = 1000.0f;
		m_WindWaveLen2 = 500.0f;
		m_WindSpeed1 = 1.0f;
		m_WindSpeed2 = 2.0f;
		m_WindWaveScaleUp1 = 1.0f;
		m_WindWaveScaleUp2 = 1.0f;
		m_WindWaveScaleDown1 = 0.1f;
		m_WindWaveScaleDown2 = 0.1f;
		m_WindWaveOffset1 = 0.0f;
		m_WindWaveOffset2 = 0.0f;
		m_bEnableWave1 = true;
		m_bEnableWave2 = true;
		m_bEnableDirWind = false;
	}

	_KG3DWIND_DESCSTRUCT()
	{
		m_id = -1;
		SetDefaultValue();
	}
}KG3DWIND_DESCSTRUCT, *LPKG3DWIND_DESCSTRUCT;

struct IEKG3DEnvWindVistor
{
	virtual void vist(KG3DWIND_DESCSTRUCT *) = 0;
};

struct IEKG3DEnvEffWind 
{	
	virtual HRESULT	RequestModification( const KG3DWIND_DESCSTRUCT* pStruct ) = 0;///可以用pStruct中的ModificationOption设定改什么东西
	virtual HRESULT	GetStruct( KG3DWIND_DESCSTRUCT* pResultStruct ) = 0;
	virtual void DoVist(IEKG3DEnvWindVistor &vistor) = 0;
	virtual void DelWind(int windid) = 0;
};