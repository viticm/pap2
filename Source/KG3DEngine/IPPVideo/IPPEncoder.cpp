////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IPPEncoder.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-08 14:57:32
//  Comment     : ipp encoder
//
////////////////////////////////////////////////////////////////////////////////


#include  "IPPBase.h"
#include  "IPPEncoder.h"
#include <atltrace.h>

//#define CODEC_PARAMS_READ

namespace
{

	// pal spec function
	typedef UMC::VideoEncoder* ( *PFUNC_CREATE_VIDEOENCODER )( );
	typedef UMC::VideoEncoderParams* ( *PFUNC_CREATE_VIDEOENCODERPARAMS )( vm_char const* ParFileName );

	UMC::VideoEncoderParams* createH261EncoderParams( vm_char const* ParFileName )	
	{	
		std::auto_ptr< UMC::H261EncoderParams >  pParams( new UMC::H261EncoderParams );

#ifdef CODEC_PARAMS_READ
		if ( UMC::UMC_OK != pParams->ReadParamFile( ParFileName) )
		{
			pParams.reset();
		}
#endif

		return pParams.release();
	}

	UMC::VideoEncoderParams* createH263EncoderParams( vm_char const* ParFileName )	
	{	
		std::auto_ptr< UMC::H263EncoderParams >  pParams( new UMC::H263EncoderParams );

#ifdef CODEC_PARAMS_READ
		if ( UMC::UMC_OK != pParams->ReadParamFile( ParFileName) )
		{
			pParams.reset();
		}
#endif

		return pParams.release();
	}

	UMC::VideoEncoderParams* createH264EncoderParams( vm_char const* ParFileName )	
	{	
		std::auto_ptr< UMC::H264EncoderParams >  pParams( new UMC::H264EncoderParams );

#ifdef CODEC_PARAMS_READ
		vm_char Temp[MAX_PATH];
		vm_string_strncpy(Temp, ParFileName, sizeof(Temp));
		Temp[MAX_PATH - 1] = '\0';
		if ( UMC::UMC_OK != pParams->ReadParamFile( Temp) )
		{
			pParams.reset();
		}
#endif

		return pParams.release();
	}

	UMC::VideoEncoderParams* createMPEG2EncoderParams( vm_char const* ParFileName )	
	{	
		std::auto_ptr< UMC::MPEG2EncoderParams >  pParams( new UMC::MPEG2EncoderParams );

#ifdef CODEC_PARAMS_READ
		vm_char Temp[ MAX_PATH ];
		if ( UMC::UMC_OK != pParams->ReadParamFile( ParFileName, Temp ) )
		{
			pParams.reset();
		}
#endif

		return pParams.release();
	}


	UMC::VideoEncoderParams* createMPEG4EncoderParams( vm_char const* ParFileName )
	{	
		std::auto_ptr< UMC::MPEG4EncoderParams >  pParams( new UMC::MPEG4EncoderParams );

#ifdef CODEC_PARAMS_READ
		if ( UMC::UMC_OK != pParams->ReadParamFile( ParFileName ) )
		{
			pParams.reset();
		}
#endif
		//else
		//{
		//	pParams->m_SourceFileName[0] = TEXT( '\0' );
		//}

		pParams->m_Param.Width = 360;
		pParams->m_Param.Height = 288;
		pParams->m_Param.NumOfFrames = 1000000;
		pParams->m_Param.TimeIncrement = 1;
		pParams->m_Param.TimeResolution = 30;
		pParams->m_Param.RateControl = 1;
		pParams->m_Param.BitRate =2000000;//25000;
		pParams->m_Param.quant_type = 0;
		pParams->m_Param.quantIVOP = 4;
		pParams->m_Param.quantPVOP = 4;
		pParams->m_Param.quantBVOP = 6;
		pParams->m_Param.short_video_header = 0;
		pParams->m_Param.IVOPdist = 300;
		pParams->m_Param.BVOPdist = 0;
		pParams->m_Param.PVOPsearchWidth = 15;
		pParams->m_Param.PVOPsearchHeight = 15;
		pParams->m_Param.BVOPsearchWidthForw = 15;
		pParams->m_Param.BVOPsearchHeightForw = 15;
		pParams->m_Param.BVOPsearchWidthBack = 15;
		pParams->m_Param.BVOPsearchHeightBack = 15;pParams->m_Param.MEalgorithm = 1;
		pParams->m_Param.MEaccuracy = 2;
		pParams->m_Param.ME4mv = 0;
		pParams->m_Param.obmc_disable = 1;
		pParams->m_Param.RoundingControl = 1;
		pParams->m_Param.calcPSNR = 0;
		pParams->m_Param.SceneChangeThreshold = 50;
		pParams->m_Param.insertGOV = 0;
		pParams->m_Param.repeatHeaders = 0;
		pParams->m_Param.resync = 0;
		pParams->m_Param.VideoPacketLenght = 8192;
		pParams->m_Param.data_partitioned = 0;
		pParams->m_Param.reversible_vlc = 0;
		pParams->m_Param.interlaced = 0;
		pParams->m_Param.top_field_first = 1;
		pParams->m_Param.alternate_vertical_scan_flag = 1;
		pParams->m_Param.interlacedME = 0;
		pParams->m_Param.sprite_enable = 0;
		pParams->m_Param.no_of_sprite_warping_points = 0;
		pParams->m_Param.sprite_warping_accuracy = 0;
		pParams->m_Param.sprite_brightness_change = 0;
		pParams->m_Param.sprite_left_coordinate = 0;
		pParams->m_Param.sprite_top_coordinate = 0;
		pParams->m_Param.sprite_width = 0;
		pParams->m_Param.sprite_height = 0;
		pParams->m_Param.warping_mv_code_du = NULL;
		pParams->m_Param.warping_mv_code_dv = NULL;
		pParams->m_Param.brightness_change_factor = NULL;
		pParams->m_Param.load_intra_quant_mat = 0;
		pParams->m_Param.load_intra_quant_mat_len = 0;
		pParams->m_Param.load_nonintra_quant_mat = 0;
		pParams->m_Param.load_nonintra_quant_mat_len = 0;
		pParams->m_Param.bsBuffer = (Ipp8u*)1;
		pParams->m_Param.bsBuffSize = 1; // encoder will not allocate 
		pParams->m_Param.padType = 2; // set 1 for QuickTime(tm) and 2 for DivX (tm) v. >= 5


		return pParams.release();
	}



	// pal spec traits
	struct IPPSPEC_TAG
	{
		// MEMBER
		TCHAR   SPEC_MODULE[MAX_PATH];
		UINT	SPEC_FPS;
		UINT	SPEC_WIDTH;
		UINT	SPEC_HEIGHT;
		DWORD	SPEC_FCC_HANDLER;


		// FUCTION
		PFUNC_CREATE_VIDEOENCODER		pFuncEncoder;
		PFUNC_CREATE_VIDEOENCODERPARAMS	pFuncEncoderParams;

	};


	static IPPSPEC_TAG IPPSPEC_TRAITS[ IPPSPEC_COUNT ] = 
		{
			{	TEXT("h261.par"), 25, 352, 288, mmioFOURCC('h', '2', '6', '1'), UMC::createH261VideoEncoder, createH261EncoderParams },
			{	TEXT("h263.par"), 25, 352, 288, mmioFOURCC('h', '2', '6', '3'), UMC::createH263VideoEncoder, createH263EncoderParams },
			{	TEXT("h264.par"), 25, 352, 288, mmioFOURCC('h', '2', '6', '4'), UMC::createH264VideoEncoder, createH264EncoderParams },
			{	TEXT("mpeg2.par"), 25, 352, 288, mmioFOURCC('m', 'p', 'g', '2'), UMC::createMPEG2VideoEncoder, createMPEG2EncoderParams	},
			{	TEXT("mpeg4.par"), 25, 360, 288, mmioFOURCC('d', 'i', 'v', 'x'), UMC::createMPEG4VideoEncoder, createMPEG4EncoderParams	},
			
		};


	inline int appIppFromFilter( int filter )
	{
		switch ( filter )
		{
		case IPPSPEC_FILTER_NOT:	return IPPI_INTER_NN;
		case IPPSPEC_FILTER_CUBIC:	return IPPI_INTER_CUBIC;
		case IPPSPEC_FILTER_LINEAR:	return IPPI_INTER_LINEAR;
		}

	
		return IPPI_INTER_NN;
	}


	inline Ipp8u appD3dFmtToByte( D3DFORMAT fmt )
	{
		Ipp8u bpp = 32;

		switch ( fmt )
		{
		case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
            bpp = 32;
			break ;

		case D3DFMT_R8G8B8:
            bpp = 24;
			break ;

		case D3DFMT_R5G6B5:
            bpp = 16;
			break ;

		case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
            bpp = 15;
			break ;
		}

		return bpp;
	}

}


////////////////////////////////////////////////
// constructor / destructor
CIPPEncoder::CIPPEncoder( )
	:	m_pD3dDevice( NULL ),
		m_pD3dPlanSurface( NULL ),
		m_pIppEncoder( NULL ),
		m_pIppParams( NULL )
{
	m_SpecInfo->pSoundShell = NULL;
}


CIPPEncoder::~CIPPEncoder()
{
	CleanUp();
}


BOOL CIPPEncoder::Initialize(LPDIRECT3DDEVICE9 pD3dDevice, IPPSpec_Params_t const& Params)
{
    BOOL bRetCode = FALSE;
	BOOL bResult = FALSE;

	memcpy(&m_SpecParams, &Params, sizeof(m_SpecParams));

	CrorrectCodecParams();

	bRetCode = initD3d9Mng(pD3dDevice);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = initCodecMng();
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = initSnapMng();
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = initFrameMng();
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = initSoundRecord();
	KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = initThreadsMng();
    KGLOG_PROCESS_ERROR(bRetCode);

	bResult = TRUE;

Exit0:
	if (!bResult)
		CleanUp();

	return bResult;
}

void CIPPEncoder::CrorrectCodecParams()
{
	if (m_SpecParams->CapsDSize < 1)
		m_SpecParams->CapsDSize = 1;
	m_SpecWidth  = m_SpecParams->ClientWidth / m_SpecParams->CapsDSize;
	m_SpecHeight = m_SpecParams->ClientHeight / m_SpecParams->CapsDSize;
	m_SpecParams->CapsDSize = 1;

	if (m_SpecParams->CapsSpec >= IPPSPEC_COUNT || m_SpecParams->CapsSpec < 0)
		m_SpecParams->CapsSpec = IPPSPEC_MPEG2;
	if (m_SpecParams->CapsFps != IPPSPEC_FPS_25 && m_SpecParams->CapsFps != IPPSPEC_FPS_30 && 
		m_SpecParams->CapsFps != IPPSPEC_FPS_50 && m_SpecParams->CapsFps != IPPSPEC_FPS_60)
		m_SpecParams->CapsFps = IPPSPEC_FPS_25;

	switch(m_SpecParams->CapsSpec)
	{
	case IPPSPEC_H261:
		{
			if (m_SpecWidth > 176)
			{
				m_SpecWidth = 352;
				m_SpecHeight = 288;
				m_SpecParams->CapsQuality = IPPSPEC_QUALITY_CINEMATIC;
			}
			else
			{
				m_SpecWidth = 176;
				m_SpecHeight = 144;
				m_SpecParams->CapsQuality = IPPSPEC_QUALITY_DISC;
			}
		}
		break;
	case IPPSPEC_H263:
		{
			if (m_SpecWidth >= (1408 + 704) / 2)
			{
				m_SpecWidth = 1408;
				m_SpecHeight = 1152;
				m_SpecParams->CapsQuality = min(m_SpecParams->CapsQuality, IPPSPEC_QUALITY_CINEMATIC4);
			}
			else if (m_SpecWidth >= (704 + 352) / 2)
			{
				m_SpecWidth = 704;
				m_SpecHeight = 576;
				m_SpecParams->CapsQuality = min(m_SpecParams->CapsQuality, IPPSPEC_QUALITY_CINEMATIC2);
			}
			else if (m_SpecWidth >= (352 + 176) / 2)
			{
				m_SpecWidth = 352;
				m_SpecHeight = 288;
				m_SpecParams->CapsQuality = min(m_SpecParams->CapsQuality, IPPSPEC_QUALITY_CINEMATIC);
			}
			else if (m_SpecWidth >= (176 + 128) / 2)
			{
				m_SpecWidth = 176;
				m_SpecHeight = 144;
				m_SpecParams->CapsQuality = min(m_SpecParams->CapsQuality, IPPSPEC_QUALITY_CINEMATIC);
			}
			else
			{
				m_SpecWidth = 128;
				m_SpecHeight = 96;
				m_SpecParams->CapsQuality = min(m_SpecParams->CapsQuality, IPPSPEC_QUALITY_DISC);
			}
		}
		break;
	case IPPSPEC_H264:
		{

		}
		break;
	case IPPSPEC_MPEG2:
		{

		}
		break;
	case IPPSPEC_MPEG4:
		{
			if (m_SpecWidth & 1)
				m_SpecWidth++;
			if (m_SpecHeight & 1)
				m_SpecHeight++;
			m_SpecParams->CapsFps = IPPSPEC_FPS_30;
		}
		break;
	}
}

void  CIPPEncoder::CleanUp()
{
	cleanThreadsMng();
	cleanSoundRecord();
	cleanFrameMng();
	cleanSnapMng();
	cleanCodecMng();
	cleanD3d9Mng();

}

bool  CIPPEncoder::initSoundRecord( )
{
	m_SpecInfo->pSoundShell = m_SpecParams->pSoundShell;
	if (m_SpecInfo->pSoundShell)
		m_SpecInfo->pSoundShell->StartRecordSound();
	return true;
}

void  CIPPEncoder::cleanSoundRecord( )
{
	if (m_SpecInfo->pSoundShell)
		m_SpecInfo->pSoundShell->StopRecordSound();
	m_SpecInfo->pSoundShell = NULL;
}


////////////////////////////////////////////////
// adjust
void  CIPPEncoder::SetViewport( SIZE Viewport )
{
	// set viewport / factor
	IppiSize SpecDest = { m_SpecWidth, m_SpecHeight };
	IppiSize SpecViewport = { std::min< int >( Viewport.cx, m_pD3dDesc->Width ), std::min< int >( Viewport.cy, m_pD3dDesc->Height ) };
	IppiFactor	SpecFactor = { (float)SpecDest.width / (float)SpecViewport.width, (float)SpecDest.height / (float)SpecViewport.height };


	m_SpecInfo->SnapResize		= ( SpecDest.width != SpecViewport.width ) || ( SpecDest.height != SpecViewport.height );
	m_SpecInfo->SnapDest		= SpecDest;
	m_SpecInfo->SnapFactor		= SpecFactor;
	m_SpecInfo->SnapViewport	= SpecViewport;



	// snap buffer
	Ipp32s const Width  = SpecViewport.width & (~1);
	Ipp32s const Height = SpecViewport.height & (~1);

	m_SnapSize			= m_SpecWidth * m_SpecHeight * m_SnapBpp;
	m_SnapInSize		= Width * Height * m_SnapBpp;

	m_SnapBuffer		= getSnapBuffer( true );
	m_SnapInnerBuffer	= getSnapInnerBuffer( true );

	m_SpecInfo->SnapPitch		= m_SpecWidth * m_SnapBpp;
	m_SpecInfo->SnapInPitch		= Width * m_SnapBpp;

}

void  CIPPEncoder::GetViewport( SIZE* Viewport ) const
{
	if ( Viewport )
	{
		Viewport->cx = m_SpecInfo->SnapViewport.width;
		Viewport->cy = m_SpecInfo->SnapViewport.height;
	}

}


BOOL CIPPEncoder::Prepare( )
{
	// capture frame rate
	vm_tick const curTick = vm_time_get_tick();
	static vm_tick curRate = m_SpecInfo->nFrameFrequency * m_SpecRate / 1000;

	if ( curRate > ( curTick - m_SpecInfo->nFrameTimes ) )
	{
		return FALSE;
	}

	// capture frame
	LPDIRECT3DSURFACE9	lpD3DTarget = NULL;
	HRESULT hr = D3D_OK;

	//hr = m_pD3dDevice->GetRenderTarget( 0, &lpD3DTarget );
	hr = m_pD3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &lpD3DTarget );
	if ( hr == D3D_OK )
	{
		hr = m_pD3dDevice->GetRenderTargetData( lpD3DTarget, m_pD3dPlanSurface );
		if ( hr == D3D_OK )
		{
			D3DSURFACE_DESC desc;
			D3DLOCKED_RECT lock_rc;
			RECT rcWievPort = {0, 0, m_SpecInfo->SnapViewport.width, m_SpecInfo->SnapViewport.height};

			m_pD3dPlanSurface->GetDesc(&desc);
			if ( D3D_OK == m_pD3dPlanSurface->LockRect(&lock_rc, &rcWievPort, 0 ) )
			{
				// load next frame
				( this->*PFUNC_SNAPVIEWPORT )( lock_rc.pBits, lock_rc.Pitch );
				m_SpecInfo->nFrameRecord ++;
				
				/*for ( register UINT y = 0; y < desc.Height; ++ y )
				{
					PFUNC_SNAPCOPY( pVideoBuffer, pTempBuffer, Pitch );
					pVideoBuffer += lock_rc.Pitch;
					pTempBuffer  += Pitch;
				}*/


				m_pD3dPlanSurface->UnlockRect();
			}

		} 

		lpD3DTarget->Release();
	}

	return TRUE;
}


void  CIPPEncoder::Flush( )
{
	// determine whether this frame needs to be grabbed when recording. or just skipped.
	UINT framePrevs = m_SpecInfo->nFrameCapture;
	UINT frameDups = getFrameRate();
	if ( frameDups <= 0 )	// i want this frame?
	{
		return ;
	}


	m_SpecInfo->nFrameCapture += frameDups;
	//m_SpecInfo->nFrameRate     = (float)m_SpecInfo->nFrameCapture / (float)m_SpecFps;



	// lock input buffer
	UMC::Status	umcSts = UMC::UMC_OK;

	for ( ; framePrevs != m_SpecInfo->nFrameCapture; ++ framePrevs )
	{
		do
		{
			umcSts = m_SpecInfo->VideoFrames->LockInputBuffer( m_pIppData );
			if ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )
			{
				vm_event_signal( m_SpecInfo->hThrEtEncoder );
				vm_time_sleep(1);
			}

		} while( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts );


		if ( UMC::UMC_OK == umcSts )
		{
			// load next frame
			int stepY = (int)m_pIppData->m_lPitch[0];
			int stepU = (int)m_pIppData->m_lPitch[1];
			int stepV = (int)m_pIppData->m_lPitch[2];


			Ipp8u* pTempBuffer = getSnapBuffer();
			Ipp8u* pYUV[] = { (Ipp8u*)m_pIppData->m_lpDest[0], (Ipp8u*)m_pIppData->m_lpDest[1], (Ipp8u*)m_pIppData->m_lpDest[2] };


			PFUNC_SNAPCODEC( pTempBuffer, m_SpecInfo->SnapPitch,
				pYUV, (int*)m_pIppData->m_lPitch, m_SpecInfo->SnapDest );

			//m_pIppData->SetTime( m_SpecInfo->nFrameRate );
			m_pIppData->SetTime( (float)framePrevs * m_SpecInfo->nFrameRate  );
			m_pIppData->SetDataSize( m_SpecHeight * stepY + m_SpecHeight * stepU + m_SpecHeight * stepV );

			m_SpecInfo->VideoFrames->UnLockInputBuffer( m_pIppData );
			m_SpecInfo->nFrameSampling ++;

			vm_event_signal( m_SpecInfo->hThrEtEncoder );
		}

	}


	//// get render target
	//int const size = m_SnapSize;
	//char tgaHeader[18] = { 0 };
	//FILE* fp = fopen( "Test.TGA", "wb" );

	//tgaHeader[2]  = 2;		// uncompressed type
	//tgaHeader[12] = m_SpecInfo->SnapDest.width &  255;
	//tgaHeader[13] = m_SpecInfo->SnapDest.width >> 8;
	//tgaHeader[14] = m_SpecInfo->SnapDest.height &  255;
	//tgaHeader[15] = m_SpecInfo->SnapDest.height >> 8;
	//tgaHeader[16] = m_SpecInfo->SnapPitch / m_SpecInfo->SnapDest.width * 8;	   // pixel size
	//tgaHeader[17] = 0x20;


	//// save
	//if ( NULL != fp )
	//{
	//	fwrite( tgaHeader, 1, 18, fp );
	//	fwrite( getSnapBuffer(), 1, size, fp );
	//	//fflush( fp );
	//	fclose( fp );
	//}
	return ;
}


void  CIPPEncoder::End( )
{
	// set EOS
 	UMC::Status	umcSts = UMC::UMC_OK;
	UINT umcTicks = 0;

	do
	{
		umcSts = m_SpecInfo->VideoFrames->LockInputBuffer( m_pIppData );
		if ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )
		{
			vm_time_sleep(5);
			umcTicks ++;
		}

	} while ( ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts ) && ( umcTicks < 100 ) );  // 1 sec
	
	if ( UMC::UMC_OK == umcSts )
	{
		m_pIppData->SetTime(-1);
		m_pIppData->SetDataSize(0);
		//m_SpecInfo->VideoFrames->UnLockInputBuffer( m_pIppData, UMC::UMC_END_OF_STREAM );
		m_SpecInfo->VideoFrames->UnLockInputBuffer( m_pIppData, UMC::UMC_STOP );
	}

}


////////////////////////////////////////////////
// init ipp & device
bool  CIPPEncoder::initD3d9Mng( LPDIRECT3DDEVICE9 pD3dDevice )
{
	// init intel lib
	if ( ippStaticInit() < ippStsNoErr )
	{
		return false;
	}


	// create plain surface
	LPDIRECT3DSURFACE9 lpTarget = NULL;
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	HRESULT hr = D3D_OK;


	assert( NULL != pD3dDevice );
	hr = pD3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &lpTarget );
	if ( D3D_OK != hr )
	{
		return false;
	}


	lpTarget->GetDesc( m_pD3dDesc );
	lpTarget->Release();

	hr = pD3dDevice->CreateOffscreenPlainSurface( 
		m_pD3dDesc->Width, 
		m_pD3dDesc->Height, 
		m_pD3dDesc->Format, 
		D3DPOOL_SYSTEMMEM, 
		&lpSurface, 
		NULL 
		);
	if ( hr != D3D_OK )
	{
		return false;
	}

	m_pD3dDevice = pD3dDevice;
    m_pD3dDevice->AddRef();

	m_pD3dPlanSurface = lpSurface;
	return true;
}

void  CIPPEncoder::cleanD3d9Mng( )
{
	SAFE_RELEASE( m_pD3dPlanSurface );
	SAFE_RELEASE(m_pD3dDevice);
}


////////////////////////////////////////////////
// init codec mng
void  CIPPEncoder::initCodecParams( )
{
	// check spec
	UINT CapsSpec = m_SpecParams->CapsSpec;
	UINT CapsFilter = m_SpecParams->CapsFilter;
	UINT CapsQuality = m_SpecParams->CapsQuality;

	m_CapsSpec	 = CapsSpec;
	m_SpecFps    = std::max< UINT >( IPPSPEC_TRAITS[ CapsSpec ].SPEC_FPS, m_SpecParams->CapsFps );
	m_SpecRate   = 1000 / m_SpecFps;
//	m_SpecWidth  = m_SpecParams->ClientWidth;
//	m_SpecHeight = m_SpecParams->ClientHeight;

	m_SpecInfo->fccHandler	= IPPSPEC_TRAITS[CapsSpec].SPEC_FCC_HANDLER;
	m_SpecParams->CapsQuality = std::min< UINT >( CapsQuality, IPPSPEC_QUALITY_MAX );
	m_SpecParams->CapsFilter = std::min< UINT >( CapsFilter, IPPSPEC_FILTER_LINEAR );
	m_SpecParams->CapsSpec = CapsSpec;
}

bool  CIPPEncoder::initCodecMng( )
{
	// init video encoder
	UMC::VideoEncoderParams* pParams = NULL;
	UMC::VideoEncoder*		 pEncoder = NULL;
	
	TCHAR Module[ MAX_PATH ];
	UINT const CapsSpec = m_SpecParams->CapsSpec;
	UMC::ColorFormat CapsFmt = UMC::YV12;


	initCodecParams( );
	getEncodeModule( Module );

	pParams  = IPPSPEC_TRAITS[ CapsSpec ].pFuncEncoderParams( Module );
	pEncoder = IPPSPEC_TRAITS[ CapsSpec ].pFuncEncoder( );
	if ( ( NULL == pParams ) || ( NULL == pEncoder ) )
	{
		return false;
	}


	pParams->src_width  = m_SpecWidth;
	pParams->src_height = m_SpecHeight;

	pParams->dst_width = m_SpecWidth;
	pParams->dst_height = m_SpecHeight;

	pParams->numThreads = 1;
	//pParams->numFramesToEncode = 0x7fffffff;
	pParams->numFramesToEncode = m_SpecParams->CapsTimes * m_SpecFps;

	pParams->FrameRate = m_SpecFps;
//	if (CapsSpec == IPPSPEC_MPEG4)
//		pParams->FrameRate = 29.99f;
	pParams->BitRate   = m_SpecParams->CapsQuality;

	
	if ( UMC::UMC_OK != m_pIppData->Init( m_SpecWidth, m_SpecHeight, CapsFmt ) )
	{
		SAFE_DELETE( pParams );
		SAFE_DELETE( pEncoder );
		return false;
	}

	if ( UMC::UMC_OK != pEncoder->Init( pParams ) ) 
	{
		//fprintf(stderr,"Error: Video encoder initialization failed\n");
		SAFE_DELETE( pParams );
		SAFE_DELETE( pEncoder );
		return false;
	}


	if ( UMC::UMC_OK != pEncoder->GetInfo( pParams ) )
	{
		//vm_string_fprintf(stderr, VM_STRING("Error: Video encoder GetInfo failed\n"));
		SAFE_DELETE( pParams );
		SAFE_DELETE( pEncoder );
		return false;
	}

	assert( m_SpecWidth == pParams->src_width );
	assert( m_SpecHeight == pParams->src_height );



	// init video frame buffer
	UMC::VideoBufferParams paramVB;
	UINT const nSize = m_SpecWidth * m_SpecHeight;

	//paramVB.m_numberOfFrames       = 16;//m_SpecFps;
	//paramVB.m_prefInputBufferSize  = 2 * ( nSize + nSize / 2 );//nSize << 2;
	paramVB.m_numberOfFrames       = static_cast<vm_var32>( ( float )m_SpecFps * 0.618f );
	paramVB.m_prefInputBufferSize  = static_cast<size_t>( float( nSize << 1 ) * 1.618f );
	paramVB.m_prefOutputBufferSize = paramVB.m_prefInputBufferSize;//nSize << 2;

	if ( UMC::UMC_OK != ( m_SpecInfo->VideoFrames->Init( &paramVB ) ) )
	{
		SAFE_DELETE( pParams );
		SAFE_DELETE( pEncoder );
		return false;
	}


	// init compressed frames buffer
	//paramVB.m_numberOfFrames       = 4;//m_SpecFps;
	if ( UMC::UMC_OK != ( m_SpecInfo->CompressedFrames->Init( &paramVB ) ) )
	{
		SAFE_DELETE( pParams );
		SAFE_DELETE( pEncoder );
		return false;
	}



	// init spec info
	m_pIppParams				= pParams;
	m_pIppEncoder				= pEncoder;
	m_SpecInfo->pParams			= pParams;
	m_SpecInfo->pEncoder		= pEncoder;


	m_SpecInfo->SpecFmt			= CapsFmt;
	m_SpecInfo->SpecFilter		= appIppFromFilter( m_SpecParams->CapsFilter );


	return true;
}

void  CIPPEncoder::cleanCodecMng( )
{
	m_SpecInfo->CompressedFrames->Close();
	m_SpecInfo->VideoFrames->Close();

	SAFE_DELETE( m_pIppEncoder );
	SAFE_DELETE( m_pIppParams );

}


////////////////////////////////////////////////
// init snap mng
bool  CIPPEncoder::initSnapMng( )
{
	// init spec info
	Ipp8u  const Bpp    = appD3dFmtToByte( m_pD3dDesc->Format );
	Ipp8u  const Size   = ( ( Bpp + 1 ) >> 3 );
	SIZE const Viewport = { m_SpecParams->ClientWidth, m_SpecParams->ClientHeight };

	m_SnapBpp	= Size;
	SetViewport( Viewport );


		// init snap buffer
	PFUNC_SNAPCOPY		= NULL;
	PFUNC_SNAPCODEC		= NULL;
	PFUNC_SNAPRESIZE	= NULL;
	PFUNC_SNAPVIEWPORT  = ( SnapViewportT )&CIPPEncoder::snapHighBitBuffer;


	if ( Size == 4 )
	{
		//PFUNC_SNAPCOPY  = ( SnapCopyT )ippsCopy_8u;//ippsCopy_32f;
		PFUNC_SNAPCOPY   = ( SnapCopyT   )ippiCopy_8u_AC4R;
		PFUNC_SNAPCODEC  = ( SnapCodecT  )ippiBGRToYCbCr420_8u_AC4P3R;
		PFUNC_SNAPRESIZE = ( SnapResizeT )ippiResize_8u_AC4R;
		//PFUNC_SNAPVIEWPORT  = ( SnapViewportT )&CIPPEncoder::snap16BitBuffer;
	}

	if ( Size == 3 )
	{
		PFUNC_SNAPCOPY   = ( SnapCopyT   )ippiCopy_8u_C3R;
		PFUNC_SNAPCODEC  = ( SnapCodecT  )ippiBGRToYCbCr420_8u_C3P3R;
		PFUNC_SNAPRESIZE = ( SnapResizeT )ippiResize_8u_C3R;
	}

	if ( Size == 2 )
	{
		PFUNC_SNAPCODEC     = ( SnapCodecT    )ippiBGR565ToYCbCr420_16u8u_C3P3R;
		PFUNC_SNAPVIEWPORT  = ( SnapViewportT )&CIPPEncoder::snap16BitBuffer;
	}

	if ( Bpp  == 15 )
	{
		PFUNC_SNAPCODEC     = ( SnapCodecT    )ippiBGR555ToYCbCr420_16u8u_C3P3R;
		PFUNC_SNAPVIEWPORT  = ( SnapViewportT )&CIPPEncoder::snap16BitBuffer;
	}


	// init path
	memcpy( m_SpecInfo->FileAvis, m_SpecParams->FileAvis, sizeof( m_SpecInfo->FileAvis ) );
	memcpy( m_SpecInfo->Paths, m_SpecParams->Paths, sizeof( m_SpecInfo->Paths ) );



	return true;
}

void  CIPPEncoder::cleanSnapMng( )
{
    if (m_SnapBuffer)
    {
	    ippFree(m_SnapBuffer);
        m_SnapBuffer = NULL;
    }
	if (m_SnapInnerBuffer)
    {
        ippFree(m_SnapInnerBuffer);
        m_SnapInnerBuffer = NULL;
    }
}


////////////////////////////////////////////////
// init frame mng
bool  CIPPEncoder::initFrameMng( )
{
	// init frame info
	m_SpecInfo->nFrameRate			= 1.0f / m_SpecFps;
	m_SpecInfo->nFrameWeight		= 0;

	m_SpecInfo->nFrameRecord		= 0;
	m_SpecInfo->nFrameCapture		= 0;

	m_SpecInfo->nFrameTimes			= 0;
	m_SpecInfo->nFrameFrequency		= vm_time_get_frequency();

	m_SpecInfo->nFrameSave			= 0;
	m_SpecInfo->nFrameEncoder		= 0;
	m_SpecInfo->nFrameSampling		= 0;



	return true;
}

void  CIPPEncoder::cleanFrameMng( )
{

}



////////////////////////////////////////////////
// init thread mng
extern unsigned int IPP_ThreadSaveing( void* Parameter );
extern unsigned int IPP_ThreadEncoding( void* Parameter );

bool  CIPPEncoder::initThreadsMng( )
{
	vm_event_set_invalid( m_SpecInfo->hThrEtExit );
	vm_event_set_invalid( m_SpecInfo->hThrEtSave );
	vm_event_set_invalid( m_SpecInfo->hThrEtEncoder );

	vm_thread_set_invalid( m_SpecInfo->hThrSave );
	vm_thread_set_invalid( m_SpecInfo->hThrEncoder );


	if ( VM_OK != vm_event_init( m_SpecInfo->hThrEtExit, TRUE, FALSE ) )
	{
		return false;
	}

	if ( VM_OK != vm_event_init( m_SpecInfo->hThrEtSave, TRUE, FALSE ) )
	{
		return false;
	}

	if ( VM_OK != vm_event_init( m_SpecInfo->hThrEtEncoder, TRUE, FALSE ) )
	{
		return false;
	}


	if ( !vm_thread_create( m_SpecInfo->hThrEncoder, IPP_ThreadEncoding, &m_SpecInfo ) )
	{
		vm_string_printf(__VM_STRING("ThreadEncoding creation failed\n"));
		return false;
	}

	if ( !vm_thread_create( m_SpecInfo->hThrSave, IPP_ThreadSaveing, &m_SpecInfo ) )
	{
		vm_string_printf(__VM_STRING("ThreadSaveFrame creation failed\n"));
		return false;
	}


	vm_thread_set_priority( m_SpecInfo->hThrEncoder, VM_THREAD_PRIORITY_HIGHEST );
	vm_thread_set_priority( m_SpecInfo->hThrSave , VM_THREAD_PRIORITY_NORMAL);

	return true;
}

void  CIPPEncoder::cleanThreadsMng( )
{
	vm_event_signal( m_SpecInfo->hThrEtEncoder );
	vm_event_signal( m_SpecInfo->hThrEtSave );
	vm_event_signal( m_SpecInfo->hThrEtExit );

	vm_thread_close( m_SpecInfo->hThrSave );
	vm_thread_close( m_SpecInfo->hThrEncoder );

	vm_event_destroy( m_SpecInfo->hThrEtExit );
	vm_event_destroy( m_SpecInfo->hThrEtSave );
	vm_event_destroy( m_SpecInfo->hThrEtEncoder );

	vm_thread_set_invalid( m_SpecInfo->hThrSave );
	vm_thread_set_invalid( m_SpecInfo->hThrEncoder );


#if 0
	TCHAR szTemp[MAX_PATH];

	_sntprintf( 
		szTemp, 
		sizeof( TCHAR ) * MAX_PATH, 
		TEXT( "%s\\Dump.txt" ), 
		m_SpecParams->Paths
		);

	FILE* fp = _tfopen( szTemp, "ab" );

	if ( fp )
	{
		fprintf( fp, "///////////////////////////////////////////////////////////////\r\n" );
		fprintf( fp, "frame capture: %d\r\n", m_SpecInfo->nFrameCapture );
		fprintf( fp, "frame record: %d\r\n",  m_SpecInfo->nFrameRecord );
		fprintf( fp, "frame sample:  %d\r\n", m_SpecInfo->nFrameSampling );
		fprintf( fp, "frame encode:  %d\r\n", m_SpecInfo->nFrameEncoder );
		fprintf( fp, "frame save:    %d\r\n", m_SpecInfo->nFrameSave );
		fprintf( fp, "\r\n\r\n\r\n\r\n" );

		fflush( fp );
		fclose( fp );
	}

#endif

}


////////////////////////////////////////////////
// get snap buffer
void   CIPPEncoder::snap16BitBuffer( void* pBit, int Pitch )
{
	// load next frame
	Ipp8u* pVideoBuffer  = (Ipp8u*)pBit;
	Ipp8u* pTempBuffer   = getSnapBuffer();
	
	UINT const PtichDest = m_SpecInfo->SnapPitch;
	UINT const Height = m_SpecInfo->SnapDest.height;
	UINT const Width  = m_SpecInfo->SnapDest.width;

	for ( register UINT y = 0; y < Height; ++ y )
	{
		ippsCopy_8u( pVideoBuffer, pTempBuffer, PtichDest );
		pVideoBuffer += Pitch;
		pTempBuffer  += PtichDest;
	}

}

void   CIPPEncoder::snapHighBitBuffer( void* pBit, int Pitch )
{
	// load next frame
	Ipp8u* pVideoBuffer  = (Ipp8u*)pBit;
	Ipp8u* pTempBuffer   = getSnapBuffer();

	if ( m_SpecInfo->SnapResize )
	{
		IppiRect RectSrc = { 0, 0,  m_SpecInfo->SnapViewport.width,  m_SpecInfo->SnapViewport.height };
		Ipp8u* pTempInBuffer = getSnapInnerBuffer();


		PFUNC_SNAPCOPY( 
			pVideoBuffer, Pitch, 
			pTempInBuffer, m_SpecInfo->SnapInPitch, 
			m_SpecInfo->SnapViewport
			);
		PFUNC_SNAPRESIZE( 
			pTempInBuffer, m_SpecInfo->SnapViewport, m_SpecInfo->SnapInPitch, RectSrc,
			pTempBuffer, m_SpecInfo->SnapPitch, m_SpecInfo->SnapDest, 
			m_SpecInfo->SnapFactor.xFactor, m_SpecInfo->SnapFactor.yFactor,
			m_SpecInfo->SpecFilter 
			);

	}
	else
	{
		PFUNC_SNAPCOPY( 
			pVideoBuffer, Pitch, 
			pTempBuffer, m_SpecInfo->SnapPitch, m_SpecInfo->SnapDest
			);

	}


}


////////////////////////////////////////////////
// get snap buffer
Ipp8u* CIPPEncoder::getSnapBuffer( bool newAlloc )
{
	if ( newAlloc )
	{
        if (m_SnapBuffer)
        {
		    ippFree(m_SnapBuffer);
            m_SnapBuffer = NULL;
        }
	}

	if (m_SnapBuffer == NULL)
	{
		m_SnapBuffer = (Ipp8u*)ippMalloc(m_SnapSize);
	}

	return m_SnapBuffer;
}

Ipp8u* CIPPEncoder::getSnapInnerBuffer( bool newAlloc )
{
	if ( newAlloc )
	{
        if (m_SnapInnerBuffer)
        {
		    ippFree(m_SnapInnerBuffer);
            m_SnapInnerBuffer = NULL;
        }
	}

	if (m_SnapInnerBuffer == NULL)
	{
		m_SnapInnerBuffer = (Ipp8u*)ippMalloc(m_SnapInSize);
	}

	return m_SnapInnerBuffer;
}



////////////////////////////////////////////////
// get module path
void  CIPPEncoder::getEncodeAvi( PTCHAR Module )
{
	//_snprintf_s( 
	_sntprintf( 
		Module, 
		sizeof( TCHAR ) * MAX_PATH, 
		//MAX_PATH, 
		TEXT( "%s" ), 
		m_SpecParams->FileAvis
		);

}

void  CIPPEncoder::getEncodeModule( PTCHAR Module )
{
	//_snprintf_s( 
	_sntprintf( 
		Module, 
		sizeof( TCHAR ) * MAX_PATH, 
		//MAX_PATH, 
		TEXT( "%s\\%s" ), 
		m_SpecParams->Paths,
		IPPSPEC_TRAITS[ m_SpecParams->CapsSpec ].SPEC_MODULE 
		);
}


////////////////////////////////////////////////
// get frame rate
UINT  CIPPEncoder::getFrameFps( )
{
	//vm_tick const curTick = vm_time_get_tick();
	//vm_tick const curDelay = curTick - m_nFrameTimes;

	//( curTick - m_nFrameTimes ) / m_SpecRate;
	//{
	//	vm_time_sleep(5);
	//	return false;
	//}

	return 0;
}


UINT   CIPPEncoder::getFrameRate( )
{
	// called during frame present
	// determine whether this frame needs to be grabbed when recording.
	// RETURN:
	//  dwFrameDups = number of frames to record to match my desired frame rate.


	// how long since the last frame?
	vm_tick const curTick = vm_time_get_tick();
	vm_tick const curDelay = curTick - m_SpecInfo->nFrameTimes;

	if ( ( m_SpecInfo->nFrameTimes == 0 ) 
		|| ( curDelay <= 0 ) ) // first frame i've seen?
	{
		m_SpecInfo->nFrameTimes = curTick;
		return 1;
	}


	// get frame weight
	float fFrameWeight = m_SpecInfo->nFrameWeight + getFrameWeight( curDelay );
	
	m_SpecInfo->nFrameTimes  = curTick;
	m_SpecInfo->nFrameWeight = fFrameWeight - (UINT)(fFrameWeight);


	return (UINT)(fFrameWeight);
}

float CIPPEncoder::getFrameWeight( vm_tick DelayTime )
{
	float const fFrameRate = (float)m_SpecFps;
	float const fFrameRateCur = (float)m_SpecInfo->nFrameFrequency / (float)DelayTime;

	
	if ( fFrameRateCur <= 0 )
	{
		return 0;
	}


	return fFrameRate / fFrameRateCur;
}



