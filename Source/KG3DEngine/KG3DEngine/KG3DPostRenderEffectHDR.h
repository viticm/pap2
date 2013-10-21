////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DPostRenderEffectHDR.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-12-8 10:52:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DPOSTRENDEREFFECTHDR_H_
#define _INCLUDE_KG3DPOSTRENDEREFFECTHDR_H_

#include "KG3DPostRenderEffectBase.h"


// ----------- Constants and custom types ---------------

#define HDR_FX_FILE           TEXT("\\Data\\public\\shader\\HDR.fx")

namespace PostRender
{
    class KG3DPostRenderEffectManager;


		//----------------------------------------------------------
	// Star generation

	// Define each line of the star.
	typedef struct STARLINE
	{
		int     nPasses ;
		float   fSampleLength ;
		float   fAttenuation ;
		float   fInclination ;

	} *LPSTARLINE ;


	// Simple definition of the star.
	typedef struct STARDEF
	{
		TCHAR   *szStarName ;
		int     nStarLines ;
		int     nPasses ;
		float   fSampleLength ;
		float   fAttenuation ;
		float   fInclination ;
		bool    bRotation ;

	} *LPSTARDEF ;


	// Simple definition of the sunny cross filter
	typedef struct STARDEF_SUNNYCROSS
	{
		TCHAR   *szStarName ;
		float   fSampleLength ;
		float   fAttenuation ;
		float   fInclination ;

	} *LPSTARDEF_SUNNYCROSS ;


	// Star form library
	enum ESTARLIBTYPE
	{
		STLT_DISABLE            = 0,

		STLT_CROSS,
		STLT_CROSSFILTER,
		STLT_SNOWCROSS,
		STLT_VERTICAL,
		NUM_BASESTARLIBTYPES,

		STLT_SUNNYCROSS         = NUM_BASESTARLIBTYPES,

		NUM_STARLIBTYPES,
	} ;


	//----------------------------------------------------------
	// Star generation object

	class CStarDef
	{
	public:
		TCHAR           m_strStarName[256] ;

		int             m_nStarLines ;
		LPSTARLINE      m_pStarLine ;   // [m_nStarLines]
		float           m_fInclination ;
		bool            m_bRotation ;   // Rotation is available from outside ?

	// Static library
	public:
		static CStarDef     *ms_pStarLib ;
		static D3DXCOLOR    ms_avChromaticAberrationColor[8] ;

	// Public method
	public:
		CStarDef() ;
		CStarDef(const CStarDef& src) ;
		~CStarDef() ;

		CStarDef& operator =(const CStarDef& src) {
			Initialize(src) ;
			return *this ;
		}

		HRESULT Construct() ;
		void Destruct() ;
		void Release() ;

		HRESULT Initialize(const CStarDef& src) ;

		HRESULT Initialize(ESTARLIBTYPE eType) {
			return Initialize(ms_pStarLib[eType]) ;
		}

		/// Generic simple star generation
		HRESULT Initialize(const TCHAR *szStarName,
						   int nStarLines,
						   int nPasses,
						   float fSampleLength,
						   float fAttenuation,
						   float fInclination,
						   bool bRotation) ;

		HRESULT Initialize(const STARDEF& starDef)
		{
			return Initialize(starDef.szStarName,
							  starDef.nStarLines,
							  starDef.nPasses,
							  starDef.fSampleLength,
							  starDef.fAttenuation,
							  starDef.fInclination,
							  starDef.bRotation) ;
		}

		/// Specific star generation
		//  Sunny cross filter
		HRESULT Initialize_SunnyCrossFilter(const TCHAR *szStarName = TEXT("SunnyCross"),
											float fSampleLength = 1.0f,
											float fAttenuation = 0.88f,
											float fLongAttenuation = 0.95f,
											float fInclination = D3DXToRadian(0.0f)) ;


	// Public static method
	public:
		/// Create star library
		static HRESULT InitializeStaticStarLibs() ;
		static HRESULT DeleteStaticStarLibs() ;

		/// Access to the star library
		static const CStarDef& GetLib(DWORD dwType) {
			return ms_pStarLib[dwType] ;
		}

		static const D3DXCOLOR& GetChromaticAberrationColor(DWORD dwID) {
			return ms_avChromaticAberrationColor[dwID] ;
		}
	} ;



	//----------------------------------------------------------
	// Clare definition

	// Glare form library
	enum EGLARELIBTYPE
	{
		GLT_DISABLE                 = 0,

		GLT_CAMERA,
		GLT_NATURAL,
		GLT_CHEAPLENS,
		//GLT_AFTERIMAGE,
		GLT_FILTER_CROSSSCREEN,
		GLT_FILTER_CROSSSCREEN_SPECTRAL,
		GLT_FILTER_SNOWCROSS,
		GLT_FILTER_SNOWCROSS_SPECTRAL,
		GLT_FILTER_SUNNYCROSS,
		GLT_FILTER_SUNNYCROSS_SPECTRAL,
		GLT_CINECAM_VERTICALSLITS,
		GLT_CINECAM_HORIZONTALSLITS,

		NUM_GLARELIBTYPES,
		GLT_USERDEF                 = -1,
		GLT_DEFAULT                 = GLT_FILTER_CROSSSCREEN,
	} ;


	// Simple glare definition
	typedef struct GLAREDEF
	{
		TCHAR           *szGlareName ;
		float           fGlareLuminance ;

		float           fBloomLuminance ;
		float           fGhostLuminance ;
		float           fGhostDistortion ;
		float           fStarLuminance ;
		ESTARLIBTYPE    eStarType ;
		float           fStarInclination ;

		float           fChromaticAberration ;

		float           fAfterimageSensitivity ;    // Current weight
		float           fAfterimageRatio ;          // Afterimage weight
		float           fAfterimageLuminance ;

	} *LPGLAREDEF ;


	//----------------------------------------------------------
	// Glare definition

	class CGlareDef
	{
	public:
		TCHAR       m_strGlareName[256] ;

		float       m_fGlareLuminance ;     // Total glare intensity (not effect to "after image")
		float       m_fBloomLuminance ;
		float       m_fGhostLuminance ;
		float       m_fGhostDistortion ;
		float       m_fStarLuminance ;
		float       m_fStarInclination ;

		float       m_fChromaticAberration ;

		float       m_fAfterimageSensitivity ;  // Current weight
		float       m_fAfterimageRatio ;        // Afterimage weight
		float       m_fAfterimageLuminance ;

		CStarDef    m_starDef ;

	// Static library
	public:
		static CGlareDef    *ms_pGlareLib ;

	// Public method
	public:
		CGlareDef() ;
		CGlareDef(const CGlareDef& src) ;
		~CGlareDef() ;

		CGlareDef& operator =(const CGlareDef& src) {
			Initialize(src) ;
			return *this ;
		}

		HRESULT Construct() ;
		void Destruct() ;
		void Release() ;

		HRESULT Initialize(const CGlareDef& src) ;

		HRESULT Initialize(const TCHAR *szStarName,
						   float fGlareLuminance,
						   float fBloomLuminance,
						   float fGhostLuminance,
						   float fGhostDistortion,
						   float fStarLuminance,
						   ESTARLIBTYPE eStarType,
						   float fStarInclination,
						   float fChromaticAberration,
						   float fAfterimageSensitivity,    // Current weight
						   float fAfterimageRatio,          // After Image weight
						   float fAfterimageLuminance) ;

		HRESULT Initialize(const GLAREDEF& glareDef)
		{
			return Initialize(glareDef.szGlareName,
							  glareDef.fGlareLuminance,
							  glareDef.fBloomLuminance,
							  glareDef.fGhostLuminance,
							  glareDef.fGhostDistortion,
							  glareDef.fStarLuminance,
							  glareDef.eStarType,
							  glareDef.fStarInclination,
							  glareDef.fChromaticAberration,
							  glareDef.fAfterimageSensitivity,
							  glareDef.fAfterimageRatio,
							  glareDef.fAfterimageLuminance) ;
		}

		HRESULT Initialize(EGLARELIBTYPE eType) {
			return Initialize(ms_pGlareLib[eType]) ;
		}


	// Public static method
	public:
		/// Create glare library
		static HRESULT InitializeStaticGlareLibs() ;
		static HRESULT DeleteStaticGlareLibs() ;

		/// Access to the glare library
		static const CGlareDef& GetLib(DWORD dwType) {
			return ms_pGlareLib[dwType] ;
		}
	} ;


	//----------------------------------------------------------
	// Dummy to generate static object of glare
	class __CGlare_GenerateStaticObjects
	{
	public:
		__CGlare_GenerateStaticObjects() {
			CStarDef::InitializeStaticStarLibs() ;
			CGlareDef::InitializeStaticGlareLibs() ;
		}

		~__CGlare_GenerateStaticObjects() {
			CGlareDef::DeleteStaticGlareLibs() ;
			CStarDef::DeleteStaticStarLibs() ;
		}

		static __CGlare_GenerateStaticObjects ms_staticObject ;
	} ;

    class KG3DPostRenderEffectHDR : public KG3DPostRenderEffectBase
    {
    public :
        KG3DPostRenderEffectHDR(KG3DPostRenderEffectManager *pMgr);
        virtual ~KG3DPostRenderEffectHDR();
    
        static HRESULT GetSampleOffsets_DownScale4x4( 
            DWORD dwWidth, DWORD dwHeight, D3DXVECTOR2 avSampleOffsets[] 
        );
        static  HRESULT GetSampleOffsets_GaussBlur5x5(
            DWORD        dwD3DTexWidth,
            DWORD        dwD3DTexHeight,
            D3DXVECTOR2* avTexCoordOffset,
            D3DXVECTOR4* avSampleWeight,
            FLOAT        fMultiplier = 1.0f 
            );
        static HRESULT GetSampleOffsets_DownScale2x2( 
            DWORD        dwWidth, 
            DWORD        dwHeight, 
            D3DXVECTOR2  avSampleOffsets[] 
        );
        static __forceinline float GaussianDistribution( float x, float y, float rho )
        {
            float g = 1.0f / sqrtf( 2.0f * D3DX_PI * rho * rho );
            g *= expf( -(x*x + y*y)/(2*rho*rho) );
            return g;
        }

    public :
        virtual HRESULT ApplyEffect(KG3DScene* pScene);
        virtual HRESULT Enable(BOOL bEnable);
        virtual HRESULT ReloadFX();

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();

    public :
        HRESULT GetBackBufferDesc(D3DSURFACE_DESC& desc);

        HRESULT Scene_To_SceneScaled(); 
        HRESULT MeasureLuminance();
        HRESULT CalculateAdaptation();
        HRESULT SceneScaled_To_BrightPass();
        HRESULT BrightPass_To_StarSource();
        HRESULT StarSource_To_BloomSource();
        HRESULT RenderBloom();
        HRESULT RenderStar();
		int GetHDRStarTypeCount();                   
		int GetHDRStarName(int index, char szName[]); 
		void UpdateStarType();
        
        static HRESULT DrawFullScreenQuad(CoordRect c);
        HRESULT ClearTexture(LPDIRECT3DTEXTURE9 pTexture);

        HRESULT GetTextureCoords(  
            PDIRECT3DTEXTURE9 pTexSrc,  
            RECT*             pRectSrc, 
            PDIRECT3DTEXTURE9 pTexDest, 
            RECT*             pRectDest, 
            CoordRect*        pCoords 
            );
        HRESULT GetTextureRect(
            PDIRECT3DTEXTURE9 pTexture, 
            RECT* pRect
            );
        static HRESULT DrawFullScreenQuad(
            float fLeftU, 
            float fTopV,
            float fRightU, 
            float fBottomV
            );
        void CleanUp();

    private:
        HRESULT InitTextures();
        HRESULT UnInitTextures();

    public :
        PDIRECT3DTEXTURE9  m_pTexSceneScaled;                      // Scaled copy of the HDR scene
        PDIRECT3DTEXTURE9  m_apTexToneMap[NUM_TONEMAP_TEXTURES];  // Log average luminance samples 
        PDIRECT3DTEXTURE9  m_pTexAdaptedLuminanceCur;             // The luminance that the user is currenly adapted to
        PDIRECT3DTEXTURE9  m_pTexAdaptedLuminanceLast;            // The luminance that the user is currenly adapted to
        PDIRECT3DTEXTURE9  m_pTexBrightPass; 
        PDIRECT3DTEXTURE9  m_pTexStarSource;       
        PDIRECT3DTEXTURE9  m_pTexBloomSource;
        PDIRECT3DTEXTURE9  m_apTexBloom[NUM_BLOOM_TEXTURES];        // Blooming effect working textures
        PDIRECT3DTEXTURE9  m_apTexStar[NUM_STAR_TEXTURES];          // Star effect working textures

        LPD3DXEFFECT       m_pHDREffect;

        DWORD m_dwCropWidth;
        DWORD m_dwCropHeight;

        FLOAT m_fGlareLuminance;
        FLOAT m_fBloomLuminance;
        FLOAT m_fStarLuminance;
		CGlareDef         m_GlareDef;         // Glare defintion
		EGLARELIBTYPE     m_eGlareType;       // Enumerated glare type
    };
}

#endif //_INCLUDE_KG3DPOSTRENDEREFFECTHDR_H_
