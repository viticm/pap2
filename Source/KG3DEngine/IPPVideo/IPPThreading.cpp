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
#include <Vfw.h>

#define FRAME_LOCKBUFFER( PTR_FRAME, PTR_FUNC, PTR_DATA )		\
	do															\
	{															\
		umcSts = PTR_FRAME->PTR_FUNC( &PTR_DATA );				\
		if ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )				\
			|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	)		\
		{														\
			vm_time_sleep(5);									\
		}														\
																\
	} while ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )			\
			|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	);		\
																\
	if ( UMC::UMC_OK != umcSts )								\
	{															\
		break;													\
	}


#define FRAME_UNLOCKBUFFER( PTR_FRAME, PTR_FUNC, PTR_DATA )		\
	umcSts = PTR_FRAME->PTR_FUNC( &PTR_DATA );					\
	if ( UMC::UMC_OK != umcSts )								\
	{															\
		break;													\
	}
unsigned int IPP_ThreadSaveing( void* Parameter )
{
	// get spec info pointer
	IPPSpec_Infos_t* pSpecInfo = ( IPPSpec_Infos_t* )( Parameter );
	assert( pSpecInfo != NULL );


	// init local info
	UMC::VideoEncoder* pEncoder = pSpecInfo->pEncoder;
	UMC::VideoEncoderParams* pParams = pSpecInfo->pParams;
	UMC::VideoData		compressed_data;
	UMC::Status			umcSts = UMC::UMC_OK;

	AVIFileInit();
	AVISTREAMINFO aviStreamInfo;
	PAVIFILE pAviFile = NULL;
	PAVISTREAM pAviStream = NULL;
	BITMAPINFOHEADER bitmapInfoHeader;

	AVISTREAMINFO aviSoundStreamInfo;
	PAVISTREAM pAviSoundStream = NULL; 
	int nSoundFrameSave = 0;
	int nSoud = false;
	if (pSpecInfo->pSoundShell)
		nSoud = true;

	pSpecInfo->nFrameSave = 0;

	AVIFileOpen(&pAviFile, pSpecInfo->FileAvis, OF_WRITE | OF_CREATE,NULL);
	if ( !pAviFile )
	{
		AVIFileExit();
		return 0x1001;
	}

	memset(&aviStreamInfo, 0, sizeof(aviStreamInfo));
	aviStreamInfo.fccType = streamtypeVIDEO;
	aviStreamInfo.fccHandler = pSpecInfo->fccHandler;
	aviStreamInfo.dwScale = 1;
	aviStreamInfo.dwRate = (DWORD)pSpecInfo->pParams->FrameRate;
	aviStreamInfo.dwLength = aviStreamInfo.dwScale * aviStreamInfo.dwRate;
	aviStreamInfo.dwQuality = pSpecInfo->pParams->qualityMeasure;
	SetRect(&aviStreamInfo.rcFrame, 0, 0, pSpecInfo->pParams->dst_width, pSpecInfo->pParams->dst_height);
	_tcscpy(aviStreamInfo.szName, _T("Sword online 3 video stream, Coded by hu chang yin!"));

	AVIFileCreateStream(pAviFile, &pAviStream, &aviStreamInfo); 
	if (!pAviStream)
	{
		AVIFileClose(pAviFile);
		AVIFileExit();
		return 0x1001;
	}

	memset(&bitmapInfoHeader, 0, sizeof(bitmapInfoHeader));
	bitmapInfoHeader.biBitCount = pSpecInfo->SnapPitch / pSpecInfo->SnapDest.width * 8;
	bitmapInfoHeader.biCompression = pSpecInfo->fccHandler;
	bitmapInfoHeader.biWidth = pSpecInfo->pParams->dst_width;
	bitmapInfoHeader.biHeight = pSpecInfo->pParams->dst_height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biSizeImage = pSpecInfo->SnapDest.height * pSpecInfo->SnapPitch;

	AVIStreamSetFormat(pAviStream, pSpecInfo->nFrameSave, &bitmapInfoHeader, sizeof(bitmapInfoHeader));

	if (nSoud)
	{
		WAVEFORMATEX *pWaveFormatEx = (WAVEFORMATEX *)pSpecInfo->pSoundShell->GetRecordSoundWaveFormatEx();
		memset(&aviSoundStreamInfo, 0, sizeof(aviSoundStreamInfo));
		aviSoundStreamInfo.fccType = streamtypeAUDIO;
		aviSoundStreamInfo.dwScale = pWaveFormatEx->nBlockAlign;
		aviSoundStreamInfo.dwRate = pWaveFormatEx->nSamplesPerSec * pWaveFormatEx->nBlockAlign;
		aviSoundStreamInfo.dwQuality = -1;
		aviSoundStreamInfo.dwSampleSize = pWaveFormatEx->nBlockAlign;
		_tcscpy(aviSoundStreamInfo.szName, _T("Sword online 3 audio stream, Coded by hu chang yin!"));

		AVIFileCreateStream(pAviFile, &pAviSoundStream, &aviSoundStreamInfo); 
		if (pAviSoundStream)
			AVIStreamSetFormat(pAviSoundStream, nSoundFrameSave, pWaveFormatEx, sizeof(WAVEFORMATEX));
		else
			nSoud = false;
	}

	// for
	HANDLE  hEventWait[] = { pSpecInfo->hThrEtSave[0].handle, pSpecInfo->hThrEtExit[0].handle};
	DWORD   hEventCount = 2;

	while ( true )
	{
		DWORD hWait = ::WaitForMultipleObjects( hEventCount, &hEventWait[0], FALSE, INFINITE );
		if (hWait != WAIT_OBJECT_0)
			break;
		vm_event_reset( pSpecInfo->hThrEtSave );
		while ( UMC::UMC_OK == umcSts )
		{
			HRESULT hRetCode = E_FAIL;
			// get source buffer
			//FRAME_LOCKBUFFER( pSpecInfo->CompressedFrames, LockOutputBuffer, compressed_data );
			do															
			{															
				umcSts = pSpecInfo->CompressedFrames->LockOutputBuffer( &compressed_data );
				if ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )
					|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	)
				{												
					vm_time_sleep(5);								
				}
			} while ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts ) || ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts ));	
			if ( UMC::UMC_OK != umcSts )								
			{															
				break;													
			}

			hRetCode = AVIStreamWrite(pAviStream, pSpecInfo->nFrameSave, 1, compressed_data.GetDataPointer(), 
				(long)compressed_data.GetDataSize(), AVIIF_KEYFRAME, NULL, NULL);

			compressed_data.SetDataSize(0);
			//FRAME_UNLOCKBUFFER( pSpecInfo->CompressedFrames, UnLockOutputBuffer, compressed_data );
			umcSts = pSpecInfo->CompressedFrames->UnLockOutputBuffer( &compressed_data );
			if ( UMC::UMC_OK != umcSts )
			{							
				break;						
			}

			pSpecInfo->nFrameSave ++;

			if (FAILED(hRetCode))
				break;

			if (nSoud && pSpecInfo->pSoundShell)
			{
				void *pBuffer = NULL;
				DWORD dwSize = 0;
				hRetCode = pSpecInfo->pSoundShell->LockRecordSoundBuffer(&pBuffer, &dwSize);
				if (SUCCEEDED(hRetCode))
				{
					if (pBuffer && dwSize)
					{
						hRetCode = AVIStreamWrite(pAviSoundStream, nSoundFrameSave, 1, pBuffer, dwSize, AVIIF_KEYFRAME, NULL, NULL);
						nSoundFrameSave++;
						pSpecInfo->pSoundShell->ClearRecordSoundBuffer();
					}
					pSpecInfo->pSoundShell->UnlockRecordSoundBuffer();
					if (FAILED(hRetCode))
						break;
				}
			}
		}
		break;
	}

	if (pAviSoundStream)
	{
		AVIStreamClose(pAviSoundStream);
	}
	AVIStreamClose(pAviStream);
	AVIFileClose(pAviFile);
	AVIFileExit();
	return UMC::UMC_OK;
}

unsigned int IPP_ThreadEncoding( void* Parameter )
{
	// get spec info pointer
	IPPSpec_Infos_t* pSpecInfo = ( IPPSpec_Infos_t* )( Parameter );
	assert( pSpecInfo != NULL );


	// init local info
	UMC::VideoEncoder* pEncoder = pSpecInfo->pEncoder;
	UMC::VideoEncoderParams* pParams = pSpecInfo->pParams;
	UMC::VideoData		video_data;
	UMC::MediaData		compressed_data;
	UMC::Status			umcSts = UMC::UMC_OK;
	int					nExistCount = 0;


	video_data.Init( 
		pSpecInfo->SnapDest.width, 
		pSpecInfo->SnapDest.height,
		(UMC::ColorFormat)pSpecInfo->SpecFmt 
		);


	// for
	HANDLE  hEventWait[] = { pSpecInfo->hThrEtEncoder[0].handle, pSpecInfo->hThrEtExit[0].handle };
	DWORD   hEventCount = 2;

	while ( true )
	{
		DWORD hWait = ::WaitForMultipleObjects( hEventCount, &hEventWait[0], FALSE, INFINITE );
		if ( hWait != WAIT_OBJECT_0 ) 
		{
			break ;
		}


		vm_event_reset( pSpecInfo->hThrEtEncoder );
		vm_event_signal( pSpecInfo->hThrEtSave );
		while ( UMC::UMC_OK == umcSts )
		{
			// get destination buffer
			//FRAME_LOCKBUFFER( pSpecInfo->CompressedFrames, LockInputBuffer, compressed_data );
			do															
			{															
				umcSts = pSpecInfo->CompressedFrames->LockInputBuffer( &compressed_data );
				if ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )
					|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	)
				{												
					vm_time_sleep(5);								
				}
			} while ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts ) || ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts ));	
			if ( UMC::UMC_OK != umcSts )								
			{														
				break;													
			}

			//FRAME_LOCKBUFFER( pSpecInfo->VideoFrames, LockOutputBuffer, video_data );
			do															
			{															
				umcSts = pSpecInfo->VideoFrames->LockOutputBuffer( &video_data );
				if ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )
					|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	)
				{												
					vm_time_sleep(5);								
				}
			} while ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts ) || ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts ));	
			if ( UMC::UMC_OK != umcSts )								
			{	
				pSpecInfo->CompressedFrames->UnLockInputBuffer( &compressed_data);
				break;													
			}

			// compress
			UMC::VideoData* p_video_data = ( video_data.GetTime() >= 0 ) ? &video_data : NULL;
			if ( UMC::UMC_OK != ( umcSts = pEncoder->GetFrame( p_video_data, &compressed_data ) ) )
			{
				break;
			}


			video_data.SetDataSize( 0 );
			//FRAME_UNLOCKBUFFER( pSpecInfo->VideoFrames, UnLockOutputBuffer, video_data );
			umcSts = pSpecInfo->VideoFrames->UnLockOutputBuffer( &video_data );
			if ( UMC::UMC_OK != umcSts )
			{
				pSpecInfo->CompressedFrames->UnLockInputBuffer( &compressed_data);
				break;						
			}
			//FRAME_UNLOCKBUFFER( pSpecInfo->CompressedFrames, UnLockInputBuffer, compressed_data );
			umcSts = pSpecInfo->CompressedFrames->UnLockInputBuffer( &compressed_data);
			if ( UMC::UMC_OK != umcSts )
			{							
				break;						
			}

			pSpecInfo->nFrameEncoder ++;
			vm_event_signal( pSpecInfo->hThrEtSave );
		}
	}


	// epilog
	while ( true )
	{
		// get destination buffer
		//FRAME_LOCKBUFFER( pSpecInfo->CompressedFrames, LockInputBuffer, compressed_data );
		do															
		{															
			umcSts = pSpecInfo->CompressedFrames->LockInputBuffer( &compressed_data );
			if ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts )
				|| ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts )	)
			{												
				vm_time_sleep(5);								
			}
		} while ( ( UMC::UMC_NOT_ENOUGH_DATA == umcSts ) || ( UMC::UMC_NOT_ENOUGH_BUFFER == umcSts ));	
		if ( UMC::UMC_OK != umcSts )								
		{														
			break;													
		}


		// compress
		umcSts = pEncoder->GetFrame( NULL, &compressed_data );
		//if ( ( UMC::UMC_OK != umcSts ) 
		//	&& ( UMC::UMC_END_OF_STREAM != umcSts ) )
		nExistCount++;
		if ( UMC::UMC_OK != umcSts || nExistCount > 100) 
		{
			// put EOS and exit
			compressed_data.SetTime(-1.0);
			compressed_data.SetDataSize(0);
			pSpecInfo->CompressedFrames->UnLockInputBuffer( &compressed_data, UMC::UMC_END_OF_STREAM );
			break;
		}

		if ( UMC::UMC_OK != ( umcSts = pSpecInfo->CompressedFrames->UnLockInputBuffer( &compressed_data ) ) )
		{
			break;
		}


		pSpecInfo->nFrameEncoder ++;
		vm_event_signal( pSpecInfo->hThrEtSave );
	}

	return UMC::UMC_OK;
}

