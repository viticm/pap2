/*=============================================================================
	ocean_surface.cpp: Ocean wave simulator/renderer.

	Revision history:
		* Created by Lin Nan, 2003-6-19
=============================================================================*/

#include "stdafx.h"
#include "ocean_surface.h"
#include "KG3DGraphicsTool.h"
#include "KG3DCamera.h"
#include "KG3DTerrainRender.h"
#include <d3dx9effect.h>

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//<CTH>//080331//这个标志在2004Oct的SDK里面没有，是05版加入的。
#ifndef D3DXFX_NOT_CLONEABLE
#define D3DXFX_NOT_CLONEABLE (1 << 11)
#endif

#define HALF_SQRT_2	0.7071068f
#define GRAV_ACCEL	981.0f	// The acceleration of gravity, cm/s^2


#ifndef V
#define V(x)           { hr = x; }
#endif
#ifndef V_RETURN
#define V_RETURN(x)    { hr = x; if( FAILED(hr) ) { return hr; } }
#endif

// Generating gaussian random number with mean 0 and standard deviation 1.
float Gauss()
{
	float u1 = rand() / (float)RAND_MAX;
	float u2 = rand() / (float)RAND_MAX;
	if (u1 < 1e-6f)
		u1 = 1e-6f;
	return sqrtf(-2 * logf(u1)) * cosf(2*D3DX_PI * u2);
}

// Phillips Spectrum
// K: normalized wave vector, W: wind direction, v: wind velocity, a: constant affecting factor
float Phillips(D3DXVECTOR2 K, D3DXVECTOR2 W, float v, float a, float dir_depend)
{
	float l = v * v / GRAV_ACCEL;	// largest possible wave from constant wind of velocity v
	float w = l / 1000;		// damp out waves with very small length w << l

	float Ksqr = K.x * K.x + K.y * K.y;
	float Kcos = K.x * W.x + K.y * W.y;
	float phillips = a * expf(-1 / (l * l * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

	if (Kcos < 0)
		phillips *= dir_depend;  // filter out waves moving opposite to wind

	return phillips * expf(-Ksqr * w * w);	// damp out waves with very small length w << l
}

OceanSurface::OceanSurface()
{
	m_pOceanVB				= NULL;
	m_pOceanIB				= NULL;
	m_pHeightMap			= NULL;
	m_pGradientMap			= NULL;
	m_pBicolorMap			= NULL;
	m_pHeightMapBack		= NULL;

	m_pHeightField0	= NULL;
	// H(t)
	m_pHeightField	= NULL;
	// omega
	omega			= NULL;
	// Height field
	m_Dz			= NULL;
	//m_Dz2			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	// Choppy x
	m_Dx			= NULL;
	//m_Dx2			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	// Choppy y
	m_Dy			= NULL;

	m_pd3dDevice = NULL;
}

OceanSurface::~OceanSurface()
{
	fftwnd_destroy_plan(g_fft_plan);
	SAFE_RELEASE(m_pOceanIB);
	SAFE_RELEASE(m_pOceanVB);
	SAFE_RELEASE(m_pMeshDecl);
	SAFE_RELEASE(m_pOceanFX);

	SAFE_RELEASE(m_pBicolorMap);
	SAFE_RELEASE(m_pHeightMap);
	SAFE_RELEASE(m_pGradientMap);
	SAFE_RELEASE(m_pHeightMapBack);

	SAFE_DELETE_ARRAY(m_pHeightField0);
	SAFE_DELETE_ARRAY(m_pHeightField);
	SAFE_DELETE_ARRAY(omega);
	SAFE_DELETE_ARRAY(m_Dz);
	SAFE_DELETE_ARRAY(m_Dx);
	SAFE_DELETE_ARRAY(m_Dy);
	//SAFE_DELETE_ARRAY(m_Dx2);
	//SAFE_DELETE_ARRAY(m_Dy2);
	//SAFE_DELETE_ARRAY(m_Dz2);
}


HRESULT OceanSurface::init(OceanParameter& params)
{
	HRESULT hr = S_OK;

	m_params = params;
	int HeightMapSize = params.HeightMapSize;

	// H0
	m_pHeightField0	= new fftw_complex[(HeightMapSize+1) * (HeightMapSize+4) * sizeof(fftw_complex)];
	// H(t)
	m_pHeightField		= new fftw_complex[(HeightMapSize/2) * (HeightMapSize+4) * sizeof(fftw_complex)];
	// omega
	omega			= new float[(HeightMapSize+1) * (HeightMapSize+4) * sizeof(float)];
	// Height field
	m_Dz			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	//m_Dz2			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	// Choppy x
	m_Dx			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	//m_Dx2			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	// Choppy y
	m_Dy			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];
	//m_Dy2			= new float[(HeightMapSize+2) * HeightMapSize * sizeof(float)];

	// HLSL
#if defined( DEBUG ) || defined( _DEBUG )
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
	dwShaderFlags |= D3DXSHADER_DEBUG;
#endif
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
	V_RETURN(KG3DD3DXCreateEffectFromFile(m_pd3dDevice, TEXT("data\\public\\shader\\ocean_surface.fx"), NULL, NULL, 0, NULL, &m_pOceanFX, NULL));

	// Declaration
	D3DVERTEXELEMENT9 oceanDecl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	V_RETURN(m_pd3dDevice->CreateVertexDeclaration(oceanDecl, &m_pMeshDecl));

	// Textures
	V_RETURN(m_pd3dDevice->CreateTexture(HeightMapSize, HeightMapSize, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED, &m_pHeightMap, NULL));
	V_RETURN(m_pd3dDevice->CreateTexture(HeightMapSize, HeightMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, D3DPOOL_DEFAULT, &m_pGradientMap, NULL));
	V_RETURN(m_pd3dDevice->CreateTexture(HeightMapSize, HeightMapSize, 1, 0, D3DFMT_A32B32G32R32F, D3DPOOL_MANAGED, &m_pHeightMapBack, NULL));

	// Timer

	// Height map
	initHeightMap(params);

	// D3D buffers
	createSurfaceMesh();
	createBicolorMap();


	LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency( &qwTicksPerSec );
    fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;
    // Save the start time
    QueryPerformanceCounter(&qwTime);
    qwLastTime.QuadPart = qwTime.QuadPart;
    qwAppTime.QuadPart        = 0;
    qwElapsedTime.QuadPart    = 0;
    qwElapsedAppTime.QuadPart = 0;

    return S_OK;
}

// Initialize the vector field.
// wlen_x: width of wave tile, in meters
// wlen_y: length of wave tile, in meters
void OceanSurface::initHeightMap(OceanParameter& params)
{
	int i, j;
	D3DXVECTOR2 K, Kn;

	D3DXVECTOR2 WindDir;
	D3DXVec2Normalize(&WindDir, &params.WindDir);
	float a = params.WaveAmplitude * 1e-7f;	// It is too small. We must scale it for editing.
	float v = params.WindVelocity;
	float dir_depend = params.WindDependency;
	int HeightMapSize = params.HeightMapSize;
	float UnitLength = params.UnitLength;

	// initialize random generator.
	srand(0);

	for (i = 0; i <= HeightMapSize; i++)
	{
		// K is wave-vector, range [-|DX/W, |DX/W], [-|DY/H, |DY/H]
		K.x = (-HeightMapSize / 2.0f + i) * (2 * D3DX_PI / UnitLength);

		for (j = 0; j <= HeightMapSize; j++)
		{
			K.y = (-HeightMapSize / 2.0f + j) * (2 * D3DX_PI / UnitLength);

			float phil = (K.x == 0 && K.y == 0) ? 0 : sqrtf(Phillips(K, WindDir, v, a, dir_depend));

			float real  = float(phil * Gauss() * 0.7071068f);
			float image = float(phil * Gauss() * 0.7071068f);

			m_pHeightField0[i * (HeightMapSize + 4) + j].re = real;
			m_pHeightField0[i * (HeightMapSize + 4) + j].im = image;

			// The angular frequency is following the dispersion relation:
			//            |omega^2 = g*k
			// So the equation of Gerstner wave is:
			//            x = x0 - K/k * A * sin(dot(K, x0) - sqrt(g * k) * t), x is a 2D vector.
			//            z = A * cos(dot(K, x0) - sqrt(g * k) * t)
			// Gerstner wave means: a point on a simple sinusoid wave is doing a uniform circular motion. The
			// center is (x0, y0, z0), the radius is A, and the circle plane is parallel to K.
			omega[i * (HeightMapSize + 4) + j] = sqrtf(GRAV_ACCEL * sqrtf(K.x * K.x + K.y * K.y));
		}
	}

	// initialize the FFT plan 
	g_fft_plan = rfftw2d_create_plan(HeightMapSize, HeightMapSize, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE|FFTW_THREADSAFE);

	// clear
	memset(m_pHeightField, 0, sizeof(fftw_complex) * (HeightMapSize / 2) * (HeightMapSize + 4));
	memset(m_Dz,  0, sizeof(float) * (HeightMapSize + 2) * HeightMapSize);
	memset(m_Dx,  0, sizeof(float) * (HeightMapSize + 2) * HeightMapSize);
	memset(m_Dy,  0, sizeof(float) * (HeightMapSize + 2) * HeightMapSize);

	if(m_pHeightMapBack)
	{
		// Upload to vidmem
		D3DLOCKED_RECT rect;
		m_pHeightMapBack->LockRect(0, &rect, NULL, 0);
		FLOAT* pTexData = (FLOAT*)rect.pBits;
		{
			for (i = 0; i < HeightMapSize; i++)
			{
				for (j = 0; j < HeightMapSize; j++)
				{
					// Notice: dx and dy are swapped.
					pTexData[(HeightMapSize * i + j) * 4 + 0] = m_pHeightField0[i * (HeightMapSize + 4) + j].re;
					pTexData[(HeightMapSize * i + j) * 4 + 1] = m_pHeightField0[i * (HeightMapSize + 4) + j].re;
					pTexData[(HeightMapSize * i + j) * 4 + 2] = m_pHeightField0[i * (HeightMapSize + 4) + j].re;
				}
			}
		}
		m_pHeightMapBack->UnlockRect(0);
		LPDIRECT3DTEXTURE9 pTmp = m_pHeightMap;
		m_pHeightMap = m_pHeightMapBack;
		m_pHeightMapBack = pTmp;
	}

}


void OceanSurface::updateHeightMapNew()
{
	int i, j;

	QueryPerformanceCounter( &qwTime );
	qwElapsedTime.QuadPart		= qwTime.QuadPart - qwLastTime.QuadPart;
	qwLastTime.QuadPart			= qwTime.QuadPart;
	qwElapsedAppTime.QuadPart	= qwElapsedTime.QuadPart;
	qwAppTime.QuadPart		   += qwElapsedAppTime.QuadPart;
	fAppTime					= fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));

	float t = fAppTime * m_params.TimeScale ;
	int HeightMapSize = m_params.HeightMapSize;
//	float ChoppyScale = m_params.ChoppyScale;

	D3DXVECTOR2 vDirectiron = m_params.WindDir;

	float fen[4] = {0.1F,0.1F/3,0.1F/5,0.1F/7};
//	float fsp[4] = {1.1F,1.3F,1.5F,1.7F};

	// Upload to vidmem
	D3DLOCKED_RECT rect;
	m_pHeightMapBack->LockRect(0, &rect, NULL, 0);
	FLOAT* pTexData = (FLOAT*)rect.pBits;
    #pragma omp parallel default(shared) private(i, j)
	{
        #pragma omp for
		for (i = 0; i < HeightMapSize; i++)
		{
			for (j = 0; j < HeightMapSize; j++)
			{
				D3DXVECTOR2 vDirectiron = D3DXVECTOR2((FLOAT)i, (FLOAT)j);

				float K = D3DXVec2Dot(&m_params.WindDir,&vDirectiron)*omega[(HeightMapSize * i + j)];

				// Notice: dx and dy are swapped.
				float y = 0;
				for (int s=0;s<4;s++)
				{
					y += sinf(K*fen[s]+t) * 20;
				}
				pTexData[(HeightMapSize * i + j) * 4 + 0] = 0;
				pTexData[(HeightMapSize * i + j) * 4 + 1] = y;
				pTexData[(HeightMapSize * i + j) * 4 + 2] = 0;
			}
		}
	}
	m_pHeightMapBack->UnlockRect(0);
	LPDIRECT3DTEXTURE9 pTmp = m_pHeightMap;
	m_pHeightMap = m_pHeightMapBack;
	m_pHeightMapBack = pTmp;

}

void OceanSurface::updateHeightMap()
{
	int i, j;

    QueryPerformanceCounter( &qwTime );
    qwElapsedTime.QuadPart		= qwTime.QuadPart - qwLastTime.QuadPart;
    qwLastTime.QuadPart			= qwTime.QuadPart;
    qwElapsedAppTime.QuadPart	= qwElapsedTime.QuadPart;
    qwAppTime.QuadPart		   += qwElapsedAppTime.QuadPart;
    fAppTime					= fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));

	float t = fAppTime * m_params.TimeScale ;
	int HeightMapSize = m_params.HeightMapSize;
//	float ChoppyScale = m_params.ChoppyScale;

	#pragma omp parallel num_threads(4) default(shared) private(i, j)
	{
		// Deduce the frequency space height value at any given time t from the initial height field.
		#pragma omp for
		for (i = 0; i < HeightMapSize/2; i++)
		{
			int mi = HeightMapSize - i;
			for (j = 0; j < HeightMapSize; j++)
			{
				int mj = HeightMapSize - j;
				int idx = i * (HeightMapSize + 4) + j;
				int midx = mi * (HeightMapSize + 4) + mj;
				
				float cos_v = cosf(omega[idx] * t);
				float sin_v = sinf(omega[idx] * t);

				// now sum the plus and minus waves to get the total wave amplitude
				m_pHeightField[idx].re = (m_pHeightField0[idx].re + m_pHeightField0[midx].re) * cos_v - (m_pHeightField0[idx].im + m_pHeightField0[midx].im) * sin_v;
				m_pHeightField[idx].im = (m_pHeightField0[idx].re - m_pHeightField0[midx].re) * sin_v + (m_pHeightField0[idx].im - m_pHeightField0[midx].im) * cos_v;
			}
		}

		// Prepare for the in-place fast Fourier transform for the height field
		#pragma omp for nowait
		for (i = 0; i < HeightMapSize; i += 2)
		{
			for (j = 0; j < HeightMapSize; j++)
			{
				m_Dz[i + (HeightMapSize + 2) * j] = m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].re;      // real part
				m_Dz[i + 1 + (HeightMapSize + 2) * j] = m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].im;  // image part
			}
		}

		// Choppy offset
		#pragma omp for
		for (i = 0; i < HeightMapSize; i += 2)
		{
			float kx = (float)(i/2 - HeightMapSize/2);
			for (j = 0; j < HeightMapSize; j++)
			{
				float ky = (float)(j - HeightMapSize/2);
				float k = 1 / sqrtf(kx * kx + ky * ky);    // note k != 0
				m_Dx[i     + j * (HeightMapSize + 2)] =  m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].im * kx * k;
				m_Dx[i + 1 + j * (HeightMapSize + 2)] = -m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].re * kx * k;
				m_Dy[i     + j * (HeightMapSize + 2)] =  m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].im * ky * k;
				m_Dy[i + 1 + j * (HeightMapSize + 2)] = -m_pHeightField[(i / 2) * (HeightMapSize + 4) + j].re * ky * k;
			}
		}
	}

	// Inverse fourier transform here
	#pragma omp parallel sections
	{
		#pragma omp section
		rfftwnd_one_complex_to_real(g_fft_plan, (fftw_complex*)m_Dz, (float*)m_Dz);
		#pragma omp section
		rfftwnd_one_complex_to_real(g_fft_plan, (fftw_complex*)m_Dx, (float*)m_Dx);
		#pragma omp section
		rfftwnd_one_complex_to_real(g_fft_plan, (fftw_complex*)m_Dy, (float*)m_Dy);
	}

	// Upload to vidmem
	D3DLOCKED_RECT rect;
	m_pHeightMapBack->LockRect(0, &rect, NULL, 0);
	FLOAT* pTexData = (FLOAT*)rect.pBits;
	#pragma omp parallel default(shared) private(i, j)
	{
		#pragma omp for
		for (i = 0; i < HeightMapSize; i++)
		{
			for (j = 0; j < HeightMapSize; j++)
			{
//				int sign = (i + j) & 0x01;    // cos(pi * (m1 + m2))
				// Notice: dx and dy are swapped.
				//pTexData[(HeightMapSize * i + j) * 4 + 0] = m_Dy[i + (HeightMapSize + 2) * j] * (sign ? ChoppyScale : -ChoppyScale);
				//pTexData[(HeightMapSize * i + j) * 4 + 1] = m_Dx[i + (HeightMapSize + 2) * j] * (sign ? ChoppyScale : -ChoppyScale);
				//pTexData[(HeightMapSize * i + j) * 4 + 2] = m_Dz[i + (HeightMapSize + 2) * j] * (sign ? -1 : 1);
				pTexData[(HeightMapSize * i + j) * 4 + 0] = m_Dx[HeightMapSize * i + j];
				pTexData[(HeightMapSize * i + j) * 4 + 1] = m_Dy[HeightMapSize * i + j];
				pTexData[(HeightMapSize * i + j) * 4 + 2] = m_Dz[HeightMapSize * i + j];
			}
		}
	}
	m_pHeightMapBack->UnlockRect(0);
	LPDIRECT3DTEXTURE9 pTmp = m_pHeightMap;
	m_pHeightMap = m_pHeightMapBack;
	m_pHeightMapBack = pTmp;
}

void OceanSurface::updateGradientMap()
{
    HRESULT hr = E_FAIL;

	int HeightMapSize = m_params.HeightMapSize;

	// Push
	LPDIRECT3DSURFACE9 old_target, old_depth;
	m_pd3dDevice->GetRenderTarget(0, &old_target);
	m_pd3dDevice->GetDepthStencilSurface(&old_depth);
	m_pd3dDevice->SetDepthStencilSurface(NULL);
	D3DVIEWPORT9 old_vp;
	m_pd3dDevice->GetViewport(&old_vp);
	D3DVIEWPORT9 vpnew;
	vpnew.Width = vpnew.Height = HeightMapSize;
	vpnew.MaxZ = 1;vpnew.MinZ = 0;
	vpnew.X = vpnew.Y = 0;
	m_pd3dDevice->SetViewport(&vpnew);

	m_pd3dDevice->BeginScene();
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0);

	hr = m_pOceanFX->SetTechnique("RenderOceanSurfTech");
    KGLOG_COM_CHECK_ERROR(hr);

    m_pd3dDevice->SetVertexDeclaration(m_pMeshDecl);

	// ---------------------------------------------- Normal Map
    LPDIRECT3DSURFACE9 new_target = NULL;
	m_pGradientMap->GetSurfaceLevel(0, &new_target);
	m_pd3dDevice->SetRenderTarget(0, new_target);
	SAFE_RELEASE(new_target);

	m_pOceanFX->Begin(0, 0);
	{
		// constants
		m_pOceanFX->SetFloat("g_GridLength", m_params.UnitLength / HeightMapSize);
		//
		m_pOceanFX->SetInt("g_AddressType", D3DTADDRESS_WRAP);
		//
		m_pOceanFX->SetFloatArray("g_OneTexelOffset_Left",  D3DXVECTOR2(-1.0f/HeightMapSize, 0), 2);
		m_pOceanFX->SetFloatArray("g_OneTexelOffset_Right", D3DXVECTOR2( 1.0f/HeightMapSize, 0), 2);
		m_pOceanFX->SetFloatArray("g_OneTexelOffset_Back",  D3DXVECTOR2(0, -1.0f/HeightMapSize), 2);
		m_pOceanFX->SetFloatArray("g_OneTexelOffset_Front", D3DXVECTOR2(0,  1.0f/HeightMapSize), 2);
		//
		//m_pOceanFX->SetBool("g_HasRipple", false);

		// Foam - Subsurface
		//FLOAT foam_offset = m_params.WaveAmplitude * (2 * m_params.SubsurfaceCoverage - 2.0f);
		//m_pOceanFX->SetFloat("g_FoamOffset", foam_offset);
		m_pOceanFX->SetFloat("g_ChoppyScale_GridLen", m_params.ChoppyScale * HeightMapSize / m_params.UnitLength);

		// Textures
		m_pOceanFX->SetTexture("g_texHeightMap", m_pHeightMap);
		//m_pOceanFX->SetTexture("g_texFoldingMap", m_pFoldingMap);

		// Draw all slices in one pass.
		float half_tex = 0.5f / HeightMapSize;
		float vertices[] = {-1.0f,  1.0f, 0,	half_tex,      half_tex,     
							-1.0f, -1.0f, 0,	half_tex,      half_tex+1.0f,
							 1.0f,  1.0f, 0,	half_tex+1.0f, half_tex,	 
							 1.0f, -1.0f, 0,	half_tex+1.0f, half_tex+1.0f,};
		m_pOceanFX->BeginPass(Pass_GradientFolding);
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, 20);
		m_pOceanFX->EndPass();
	}
	m_pOceanFX->End();
	m_pd3dDevice->EndScene();

	// Pop
	m_pd3dDevice->SetRenderTarget(0, old_target);
	m_pd3dDevice->SetDepthStencilSurface(old_depth);
	m_pd3dDevice->SetViewport(&old_vp);
	SAFE_RELEASE(old_target);
	SAFE_RELEASE(old_depth);
	//m_pd3dDevice->SetViewport(&vp);
}

void OceanSurface::createSurfaceMesh()
{
	// --------------------------------- Vertex Buffer -------------------------------
	int MeshXN = m_params.MeshXN;
	int MeshYN = m_params.MeshYN;
	int TileX = m_params.TileX;
	int TileY = m_params.TileY;
	float UnitLength = m_params.UnitLength;
	int numVert = (MeshXN + 1) * (MeshYN + 1) + 8;

	SAFE_RELEASE(m_pOceanVB);
	m_pd3dDevice->CreateVertexBuffer(numVert * sizeof(ocean_vertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pOceanVB, NULL);
	assert(m_pOceanVB);

	ocean_vertex* pV = NULL;
	m_pOceanVB->Lock(0, 0, (void**)&pV, 0);
	assert(pV);

	int i, j;
	float xstep, ystep;
	float ustep, vstep;
	float xstart = - UnitLength / 2;
	float ystart = - UnitLength / 2;
	// Do not forget the half texel offset.
	float uoffset = 0.5f / m_params.HeightMapSize;
	float voffset = 0.5f / m_params.HeightMapSize;

	// Near sight mesh 
	xstep = UnitLength / MeshXN;
	ystep = UnitLength / MeshYN;
	ustep = 1.0f / MeshXN;
	vstep = 1.0f / MeshYN;
	for (i = 0; i <= MeshYN; i++)
	{
		for (j = 0; j <= MeshXN; j++)
		{
			pV[i * (MeshXN + 1) + j].p.x = xstart + xstep * j;
			pV[i * (MeshXN + 1) + j].p.y = ystart + ystep * i;
			pV[i * (MeshXN + 1) + j].p.z = 0;

			pV[i * (MeshXN + 1) + j].t.x = ustep * j + uoffset;
			pV[i * (MeshXN + 1) + j].t.y = vstep * i + voffset;
		}
	}

	// A big octagon
	pV += (MeshXN + 1) * (MeshYN + 1);
	float total_width = UnitLength * TileX;
	float total_height = UnitLength * TileY;
	pV[0].p = D3DXVECTOR3(-total_width/2, 0, 0);
	pV[1].p = D3DXVECTOR3(-0.35355339f*total_width, 0.35355339f*total_height, 0);
	pV[2].p = D3DXVECTOR3(0, total_height/2, 0);
	pV[3].p = D3DXVECTOR3(0.35355339f*total_width, 0.35355339f*total_height, 0);
	pV[4].p = D3DXVECTOR3(total_width/2, 0, 0);
	pV[5].p = D3DXVECTOR3(0.35355339f*total_width, -0.35355339f*total_height, 0);
	pV[6].p = D3DXVECTOR3(0, -total_height/2, 0);
	pV[7].p = D3DXVECTOR3(-0.35355339f*total_width, -0.35355339f*total_height, 0);

	float angle = D3DX_PI;
	float angle_step = -D3DX_PI * 2 / 8;
	for (i = 0; i < 8; i++)
	{
		float cos_v = cos(angle);
		float sin_v = sin(angle);
		pV[i].t = D3DXVECTOR2((cos_v * 0.5f + 0.5f) * TileX, (sin_v * 0.5f + 0.5f) * TileY);
		angle += angle_step;
	}

	m_pOceanVB->Unlock();

	// --------------------------------- Index Buffer -------------------------------
	int numIndex = (MeshXN * 2 + 2) * MeshYN + 8;

	SAFE_RELEASE(m_pOceanIB);
	m_pd3dDevice->CreateIndexBuffer(numIndex * sizeof(DWORD), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &m_pOceanIB, NULL);
	assert(m_pOceanIB);

	DWORD* pI;
	m_pOceanIB->Lock(0, 0, (void**)&pI, 0);
	assert(pI);

	int acc = 0;
	bool reverse = false;

	// Near sight mesh
	for (i = 0; i < MeshYN; i++)
	{
		if (reverse == false)
		{
			pI[acc++] = (DWORD)(i * (MeshXN + 1));
			pI[acc++] = (DWORD)((i + 1) * (MeshXN + 1));
			for (j = 0; j < MeshXN; j++)
			{
				pI[acc++] = (DWORD)(i * (MeshXN+1) + j + 1);
				pI[acc++] = (DWORD)((i + 1) * (MeshXN+1) + j + 1);
			}
		}
		else
		{
			pI[acc++] = (DWORD)(i * (MeshXN + 1) + MeshXN);
			pI[acc++] = (DWORD)((i + 1) * (MeshXN + 1) + MeshXN);
			for (j = MeshXN - 1; j >= 0; j--)
			{
				pI[acc++] = (DWORD)(i * (MeshXN+1) + j);
				pI[acc++] = (DWORD)((i + 1) * (MeshXN +1 ) + j);
			}
		}

		reverse = !reverse;
	}

	// The big one
	pI += (MeshXN * 2 + 2) * MeshYN;
	pI[0] = 0;
	pI[1] = 7;
	pI[2] = 1;
	pI[3] = 6;
	pI[4] = 2;
	pI[5] = 5;
	pI[6] = 3;
	pI[7] = 4;

	m_pOceanIB->Unlock();
}

void OceanSurface::createBicolorMap()
{
	if (m_pBicolorMap == NULL)
	{
		m_pd3dDevice->CreateTexture(BICOLOR_TEX_SIZE, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pBicolorMap, NULL);		
		if (m_pBicolorMap == NULL)
			return;
	}

	D3DLOCKED_RECT rect;
	m_pBicolorMap->LockRect(0, &rect, NULL, 0);
	DWORD* buffer = (DWORD*)rect.pBits;

	for (int i = 0; i < BICOLOR_TEX_SIZE; i++)
	{
		// R channel: interpolation weight
		// G channel: reflect fresnel
		// B channel: refract fresnel
		// A channel: underwater fresnel

		float cos_a = i / (FLOAT)BICOLOR_TEX_SIZE;
		DWORD weight = (DWORD)(powf(cos_a, m_params.ColorBlendFactor) * 255);//(D3DXFresnelTerm(cos_a, 1.33f) * 255);
		DWORD fl_fresnel = (DWORD)(powf(1 / (1 + cos_a), m_params.ReflFresnel) * 255);
		DWORD fr_fresnel = (DWORD)(powf(1 / (1 + cos_a), m_params.RefrFresnel) * 255);
		DWORD un_fresnel = 0;//(DWORD)(powf(1 / (1 + cos_a), m_params.UnderWaterFresnel) * 255);

		buffer[i] = (un_fresnel << 24) | (weight << 16) | (fl_fresnel << 8) | fr_fresnel;
	}

	// A minor fix.
	//buffer[0] = buffer[1];

	m_pBicolorMap->UnlockRect(0);
}

void OceanSurface::displayBuffer(OceanBufferType type)
{
    HRESULT hr = E_FAIL;

	if (type == Buffer_None)
		return;

	hr = m_pOceanFX->SetTechnique("DisplayBufferTech");
    KGLOG_COM_CHECK_ERROR(hr);

    m_pOceanFX->Begin(0, 0);
	{
		// Flat texture
		// Displacement Map
		m_pOceanFX->SetTexture("g_texHeightMap", m_pHeightMap);
		// Gradient/Folding Map (Normal/Foam)
		m_pOceanFX->SetTexture("g_texGradientMap", m_pGradientMap);
		//// Ripple Buffers
		//m_pOceanFX->SetTexture("g_texSource0", m_pRippleMaps[0]);
		//m_pOceanFX->SetTexture("g_texSource1", m_pRippleMaps[1]);
		//m_pOceanFX->SetTexture("g_texSource2", m_pRippleMaps[2]);
		//m_pOceanFX->SetTexture("g_texSource3", m_pRippleMaps[3]);
		//m_pOceanFX->SetTexture("g_texSource4", m_pRippleMaps[4]);
		//m_pOceanFX->SetTexture("g_texSource5", m_pRippleMaps[5]);
		//m_pOceanFX->SetTexture("g_texSource6", m_pRippleMaps[6]);
		//m_pOceanFX->SetTexture("g_texSource7", m_pRippleMaps[7]);
		//// Merged Buffers
		//m_pOceanFX->SetTexture("g_texRipple0", m_pMergedRippleMaps[0]);
		//m_pOceanFX->SetTexture("g_texRipple1", m_pMergedRippleMaps[1]);
		//// Reflect/Refract Buffers
		//m_pOceanFX->SetTexture("g_texReflectScene", RI->RenDev->m_pOceanReflectTex);
		//m_pOceanFX->SetTexture("g_texRefractScene", RI->RenDev->m_pOceanRefractTex);

		// Buffer type
		m_pOceanFX->SetInt("g_BufferType", type);
	
		D3DVIEWPORT9 vp;
		m_pd3dDevice->GetViewport(&vp);
		float width = 512.0f/vp.Width;
		float height = 512.0f/vp.Height;
		//if (buffer_type == 14 || buffer_type == 15)
		//{
		//	width = (FLOAT)MIRROR_WIDTH/vp.Width;
		//	height = (FLOAT)MIRROR_HEIGHT/vp.Height;
		//}

		// Draw slop-map
		m_pd3dDevice->SetVertexDeclaration(m_pMeshDecl);
		float half_tex = 0.5f / m_params.HeightMapSize;
		float vertices0[] = {0.98f - width, -0.96f + height, 0,    half_tex,      half_tex,
							 0.98f - width, -0.96f,          0,    half_tex,      half_tex+1.0f,
							 0.98f,         -0.96f + height, 0,    half_tex+1.0f, half_tex,
							 0.98f,         -0.96f,          0,    half_tex+1.0f, half_tex+1.0f};
		m_pOceanFX->BeginPass(0);
		m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices0, 20);
		m_pOceanFX->EndPass();
	}
	m_pOceanFX->End();
}

void OceanSurface::renderWireframe(/*const CBaseCamera& camera*/)
{
    HRESULT hr = E_FAIL;
	int MeshXN			= m_params.MeshXN;
	int MeshYN			= m_params.MeshYN;
	int TileX			= m_params.TileX;
	int TileY			= m_params.TileY;
	float UnitLength	= m_params.UnitLength;
	int num_face		= MeshXN * MeshYN * 2 + (MeshYN - 1) * 2;
	int num_vert		= (MeshXN + 1) * (MeshYN + 1);
	int start_index		= 0;
	int base_vert		= 0;
	D3DXMATRIX matView	= g_cGraphicsTool.GetCamera()->GetViewMatrix();// *camera.GetViewMatrix();
	D3DXMATRIX matProj	= g_cGraphicsTool.GetCamera()->GetProjectionMatrix();// *camera.GetProjMatrix();
	D3DXMATRIX matY2Z(1,0,0,0,   0,0,1,0,   0,1,0,0,   0,0,0,1);

	hr = m_pOceanFX->SetTechnique("RenderOceanSurfTech");
    KGLOG_COM_CHECK_ERROR(hr);

    m_pOceanFX->Begin(0, 0);
	{
		// Textures
		m_pOceanFX->SetTexture("g_texHeightMap", m_pHeightMap);

		m_pd3dDevice->SetVertexDeclaration(m_pMeshDecl);
		m_pd3dDevice->SetStreamSource(0, m_pOceanVB, 0, sizeof(ocean_vertex));
		m_pd3dDevice->SetIndices(m_pOceanIB);

		for (int i = 0; i < TileY; i++)
		{
			for (int j = 0; j < TileX; j++)
			{
				// Matrices
				D3DXMATRIX matTile;
				D3DXMatrixTranslation(&matTile, UnitLength * ((1 - TileX) / 2.0f + j), UnitLength * ((1 - TileY) / 2.0f + i), 0);
				D3DXMATRIX matAll = matTile * matY2Z * matView * matProj;
				m_pOceanFX->SetMatrix("g_matWorldViewProj", &matAll);

				m_pOceanFX->BeginPass(Pass_TileWireframe);
				m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, base_vert, 0, num_vert, start_index, num_face);
				m_pOceanFX->EndPass();
			}
		}
	}
	m_pOceanFX->End();
}

void OceanSurface::renderDepth(/*const CBaseCamera& camera*/)
{
}

void OceanSurface::renderShaded(KG3DTerrainRender* pRender)
{
    HRESULT hr = E_FAIL;
//	int MeshXN			= m_params.MeshXN;
//	int MeshYN			= m_params.MeshYN;
//	int TileX			= m_params.TileX;
//	int TileY			= m_params.TileY;
	float UnitLength	= m_params.UnitLength;
	int HeightMapSize	= m_params.HeightMapSize;
//	int num_face		= MeshXN * MeshYN * 2 + (MeshYN - 1) * 2;
//	int num_vert		= (MeshXN + 1) * (MeshYN + 1);
//	int start_index		= 0;
//	int base_vert		= 0;
	D3DXMATRIX matView	= g_cGraphicsTool.GetCamera()->GetViewMatrix();//*camera.GetViewMatrix();
	D3DXMATRIX matProj	= g_cGraphicsTool.GetCamera()->GetProjectionMatrix();//*camera.GetProjMatrix();
	D3DXMATRIX matY2Z(1,0,0,0,   0,0,1,0,   0,1,0,0,   0,0,0,1);

	//int num_lod4_face		= 6;
	//int num_lod4_vert		= 8;
	//int start_index_lod4	= start_index_lod3 + (m_Lod3MeshXN * 2 + 2) * m_Lod3MeshYN;
	//int base_vert_lod4		= base_vert_lod3 + (m_Lod3MeshXN + 1) * (m_Lod3MeshYN + 1);

	hr = m_pOceanFX->SetTechnique("RenderOceanSurfTech");
    KGLOG_COM_CHECK_ERROR(hr);

	m_pOceanFX->Begin(0, 0);
	{
		float grid_len = UnitLength/HeightMapSize;
		m_pOceanFX->SetFloat("g_GridLength", grid_len);
		m_pOceanFX->SetFloat("g_GridLength_x2", grid_len * 2);

		// Texture
		m_pOceanFX->SetTexture("g_texHeightMap", m_pHeightMap);
		m_pOceanFX->SetTexture("g_texColorMap", m_pBicolorMap);
		m_pOceanFX->SetTexture("g_texGradientMap", m_pGradientMap);

		// Colors
		m_pOceanFX->SetVector("g_ShallowColor", &m_params.ShallowColor);
		m_pOceanFX->SetVector("g_DeepColor",  &m_params.DeepColor);
		m_pOceanFX->SetFloat("g_ReflectScale", m_params.ReflectScale);
		m_pOceanFX->SetFloat("g_ReflectIndex", m_params.ReflDistort * 0.1f);
		m_pOceanFX->SetFloat("g_RefractIndex", m_params.RefrDistort * 0.05f);


		{

			UINT iPass = Pass_TileVS_WavePS;
			D3DXMATRIX matAll = matView * matProj;
			m_pOceanFX->SetMatrix("g_matWorldViewProj", &matAll);
			D3DXMATRIX matInvWV = matView;
			D3DXMatrixInverse(&matInvWV, NULL, &matInvWV);
			D3DXVECTOR4 vLocalEye(0, 0, 0, 1);
			D3DXVec4Transform(&vLocalEye, &vLocalEye, &matInvWV);
			m_pOceanFX->SetVector("g_LocalEye", &vLocalEye);

			m_pOceanFX->BeginPass(iPass);

			pRender->RenderWaterPrimitive();

			m_pOceanFX->EndPass();

		}
	}
	m_pOceanFX->End();
}

void OceanSurface::renderShaded(/*const CBaseCamera& camera*/)
{
    HRESULT hr = E_FAIL;
	int MeshXN			= m_params.MeshXN;
	int MeshYN			= m_params.MeshYN;
	int TileX			= m_params.TileX;
	int TileY			= m_params.TileY;
	float UnitLength	= m_params.UnitLength;
	int HeightMapSize	= m_params.HeightMapSize;
	int num_face		= MeshXN * MeshYN * 2 + (MeshYN - 1) * 2;
	int num_vert		= (MeshXN + 1) * (MeshYN + 1);
	int start_index		= 0;
	int base_vert		= 0;
	D3DXMATRIX matView	= g_cGraphicsTool.GetCamera()->GetViewMatrix();//*camera.GetViewMatrix();
	D3DXMATRIX matProj	= g_cGraphicsTool.GetCamera()->GetProjectionMatrix();//*camera.GetProjMatrix();
	D3DXMATRIX matY2Z(1,0,0,0,   0,0,1,0,   0,1,0,0,   0,0,0,1);

	//int num_lod4_face		= 6;
	//int num_lod4_vert		= 8;
	//int start_index_lod4	= start_index_lod3 + (m_Lod3MeshXN * 2 + 2) * m_Lod3MeshYN;
	//int base_vert_lod4		= base_vert_lod3 + (m_Lod3MeshXN + 1) * (m_Lod3MeshYN + 1);

	hr = m_pOceanFX->SetTechnique("RenderOceanSurfTech");
    KGLOG_COM_CHECK_ERROR(hr);

	m_pOceanFX->Begin(0, 0);
	{
		float grid_len = UnitLength/HeightMapSize;
		m_pOceanFX->SetFloat("g_GridLength", grid_len);
		m_pOceanFX->SetFloat("g_GridLength_x2", grid_len * 2);

		// Texture
		m_pOceanFX->SetTexture("g_texHeightMap", m_pHeightMap);
		m_pOceanFX->SetTexture("g_texColorMap", m_pBicolorMap);
		m_pOceanFX->SetTexture("g_texGradientMap", m_pGradientMap);

		// Colors
		m_pOceanFX->SetVector("g_ShallowColor", &m_params.ShallowColor);
		m_pOceanFX->SetVector("g_DeepColor",  &m_params.DeepColor);
		m_pOceanFX->SetFloat("g_ReflectScale", m_params.ReflectScale);
		m_pOceanFX->SetFloat("g_ReflectIndex", m_params.ReflDistort * 0.1f);
		m_pOceanFX->SetFloat("g_RefractIndex", m_params.RefrDistort * 0.05f);

		// Stream
		m_pd3dDevice->SetVertexDeclaration(m_pMeshDecl);
		m_pd3dDevice->SetStreamSource(0, m_pOceanVB, 0, sizeof(ocean_vertex));
		m_pd3dDevice->SetIndices(m_pOceanIB);

		for (int i = 0; i < TileY; i++)
		{
			for (int j = 0; j < TileX; j++)
			{
				UINT iPass = Pass_TileVS_WavePS;

				D3DXMATRIX matTile;
				D3DXMatrixTranslation(&matTile, UnitLength * ((1 - TileX) / 2.0f + j), UnitLength * ((1 - TileY) / 2.0f + i), 0);
				D3DXMATRIX matAll = matTile * matY2Z * matView * matProj;
				m_pOceanFX->SetMatrix("g_matWorldViewProj", &matAll);
				D3DXMATRIX matInvWV = matTile * matY2Z * matView;
				D3DXMatrixInverse(&matInvWV, NULL, &matInvWV);
				D3DXVECTOR4 vLocalEye(0, 0, 0, 1);
				D3DXVec4Transform(&vLocalEye, &vLocalEye, &matInvWV);
				m_pOceanFX->SetVector("g_LocalEye", &vLocalEye);

				m_pOceanFX->BeginPass(iPass);
				m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, base_vert, 0, num_vert, start_index, num_face);
				m_pOceanFX->EndPass();
			}
		}
	}
	m_pOceanFX->End();
}

void OceanSurface::clearTexture(LPDIRECT3DTEXTURE9 pTex, D3DCOLOR color, UINT width, UINT height)
{
	LPDIRECT3DSURFACE9 old_target, old_depth;
	m_pd3dDevice->GetRenderTarget(0, &old_target);
	m_pd3dDevice->GetDepthStencilSurface(&old_depth);
	m_pd3dDevice->SetDepthStencilSurface(NULL);

	LPDIRECT3DSURFACE9 new_target = NULL;
	pTex->GetSurfaceLevel(0, &new_target);
	m_pd3dDevice->SetRenderTarget(0, new_target);
	SAFE_RELEASE(new_target);

	// Clear to black
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0);

	m_pd3dDevice->SetRenderTarget(0, old_target);
	m_pd3dDevice->SetDepthStencilSurface(old_depth);
	SAFE_RELEASE(old_target);
	SAFE_RELEASE(old_depth);
}

HRESULT OceanSurface::OnLostDevice()
{
    HRESULT hr = S_OK;

    if (m_pOceanFX)
    {
        hr = m_pOceanFX->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

HRESULT OceanSurface::OnResetDevice()
{
    HRESULT hr = S_OK;

    if (m_pOceanFX)
    {
        hr = m_pOceanFX->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit0:
    return hr;
}

