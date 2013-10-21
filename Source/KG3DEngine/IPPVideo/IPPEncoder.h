////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IPPEncoder.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-08 14:57:32
//  Comment     : ipp encoder
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_IPPENCODE_H_
#define _INCLUDE_IPPENCODE_H_


#include  <tchar.h>
#include  <memory>

#pragma warning(push)
#pragma warning(disable : 4005)
#pragma warning(disable : 4267)
#pragma warning(disable : 4819)

#include  "ipp.h"
#include  "ippcc.h"
#include  "ippcore.h"

#include  "vm_time.h"
#include  "vm_thread.h"

#include  "umc_video_data.h"
#include  "umc_cyclic_buffer.h"
#include  "umc_automatic_mutex.h"

#include  "umc_h261_video_encoder.h"
#include  "umc_h263_video_encoder.h"
#include  "umc_h264_video_encoder.h"
#include  "umc_mpeg2_video_encoder.h"
#include  "umc_mpeg4_video_encoder.h"

#pragma warning(pop)

#include "IKG3DSoundShell.h"

////////////////////////////////////////////////
// ipp thread spec info, runtime
typedef struct 
{
	float xFactor;
	float yFactor;

} IppiFactor;

struct IPPSpec_Infos_t
{
	// path info
	TCHAR   FileAvis[MAX_PATH];
	TCHAR   Paths[MAX_PATH];


	// encoder info
	UMC::VideoEncoder*			pEncoder;
	UMC::VideoEncoderParams*	pParams;

	UMC::SampleBuffer			VideoFrames[1];
	UMC::SampleBuffer			CompressedFrames[1];


	// encoder spec
	UINT						SpecFmt;
	UINT						SpecFilter;


	// capture buffer spec: backbuffer
	Ipp32s						SnapPitch;
	Ipp32s						SnapInPitch;

	bool						SnapResize;
	IppiSize					SnapDest;
	IppiFactor					SnapFactor;
	IppiSize					SnapViewport;

	//Ipp32s					SnapByte;
	//Ipp32s					SnapPitch;
	//IppiSize					SnapCanvas;


	DWORD						fccHandler;

	// thread info
	vm_thread					hThrSave[1];
	vm_thread					hThrEncoder[1];

	vm_event					hThrEtExit[1];
	vm_event					hThrEtSave[1];
	vm_event					hThrEtEncoder[1];


	// frame info
	float						nFrameRate;
	float						nFrameWeight;

	UINT						nFrameCapture;
	UINT						nFrameRecord;

	vm_tick						nFrameTimes;
	vm_tick						nFrameFrequency;

	UINT						nFrameSave;
	UINT						nFrameEncoder;
	UINT						nFrameSampling;

	IKG3DSoundShell				*pSoundShell;
};


//////////////////////////////////////////////////////////////////////
//   class  :   CIPPEncoder
//   brief  :   ipp encoder class
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
#include  "IppCodec.h"



class CIPPEncoder : public IIPPCodec
{
public:
	////////////////////////////////////////////////
	// constructor / destructor
	CIPPEncoder( );
	virtual ~CIPPEncoder();

	BOOL Initialize(LPDIRECT3DDEVICE9 pD3dDevice, IPPSpec_Params_t const& Params);
	void CleanUp( );

	void SetViewport(SIZE Viewport);
	void GetViewport(SIZE* Viewport) const;

	BOOL Prepare();
	void Flush();
	void End();


protected:
	void CrorrectCodecParams();
	////////////////////////////////////////////////
	// init device
	bool  initD3d9Mng( LPDIRECT3DDEVICE9 pD3dDevice );
	void  cleanD3d9Mng( );


	////////////////////////////////////////////////
	// init codec mng
	void  initCodecParams( );
	bool  initCodecMng( );
	void  cleanCodecMng( );


	////////////////////////////////////////////////
	// init snap mng
	bool  initSnapMng( );
	void  cleanSnapMng( );


	////////////////////////////////////////////////
	// init frame mng
	bool  initFrameMng( );
	void  cleanFrameMng( );

	////////////////////////////////////////////////
	// init sound record
	bool  initSoundRecord( );
	void  cleanSoundRecord( );

	////////////////////////////////////////////////
	// init thread mng
	bool  initThreadsMng( );
	void  cleanThreadsMng( );



	////////////////////////////////////////////////
	// get snap buffer
	void   snap16BitBuffer( void* pBit, int Pitch );
	void   snapHighBitBuffer( void* pBit, int Pitch );
	

	////////////////////////////////////////////////
	// get snap buffer
	Ipp8u* getSnapBuffer( bool newAlloc = false );
	Ipp8u* getSnapInnerBuffer( bool newAlloc = false );


	////////////////////////////////////////////////
	// get module path
	void  getEncodeAvi( PTCHAR Module );
	void  getEncodeModule( PTCHAR Module );


	////////////////////////////////////////////////
	// get frame rate
	UINT  getFrameFps( );
	UINT  getFrameRate( );
	float getFrameWeight( vm_tick DelayTime );



protected:
	////////////////////////////////////////////////
	// d3d device
	D3DSURFACE_DESC				m_pD3dDesc[1];
	LPDIRECT3DDEVICE9			m_pD3dDevice;
	LPDIRECT3DSURFACE9			m_pD3dPlanSurface;
	

	////////////////////////////////////////////////
	// ipp spec
	UINT						m_SpecFps;
	UINT						m_SpecRate;
	UINT						m_SpecWidth;
	UINT						m_SpecHeight;
	UINT						m_CapsSpec;

	IPPSpec_Infos_t				m_SpecInfo[1];
	IPPSpec_Params_t			m_SpecParams[1];



	////////////////////////////////////////////////
	// ipp encoder
	UMC::VideoData				m_pIppData[1];
	UMC::VideoEncoderParams*	m_pIppParams;
	UMC::VideoEncoder*			m_pIppEncoder;



	////////////////////////////////////////////////
	// ipp encoder buffer
	typedef Ipp8u* SnapPtrT;
	typedef void   ( CIPPEncoder::*SnapViewportT )( void* pBit, int Pitch );

	//typedef IppStatus (__stdcall *SnapCopyT )( const Ipp8u*  pSrc, Ipp8u* pDst, int Pitch );
	typedef IppStatus (__stdcall *SnapCopyT )( const Ipp8u*  pSrc, int srcStep, Ipp8u* pDst, int dstStep, IppiSize roiSize );
	typedef IppStatus (__stdcall *SnapCodecT )( const Ipp8u*  pSrc, int srcStep, Ipp8u* pDst[3], int dstStep[3], IppiSize roiSize );
	typedef IppStatus (__stdcall *SnapResizeT )( const Ipp8u* pSrc, IppiSize srcSize, int srcStep, IppiRect srcRoi,
		Ipp8u* pDst, int dstStep, IppiSize dstRoiSize, double xFactor, double yFactor, int interpolation );


	Ipp32s						m_SnapBpp;
	Ipp32s						m_SnapSize;
	Ipp32s						m_SnapInSize;

	SnapPtrT					m_SnapBuffer;
	SnapPtrT					m_SnapInnerBuffer;

	SnapCopyT					PFUNC_SNAPCOPY;
	SnapCodecT					PFUNC_SNAPCODEC;
	SnapResizeT					PFUNC_SNAPRESIZE;
	SnapViewportT				PFUNC_SNAPVIEWPORT;


};



#endif //_INCLUDE_IPPENCODE_H_