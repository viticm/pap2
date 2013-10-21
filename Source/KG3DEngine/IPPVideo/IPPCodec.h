////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IPPCodec.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-08 14:57:32
//  Comment     : ipp interface
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_IPPINTERFACE_H_
#define _INCLUDE_IPPINTERFACE_H_



////////////////////////////////////////////////
// ipp video import / exporter
#ifdef IPPVIDEO_EXPORTS
	#define IPPVIDEO_API __declspec(dllexport)
#else
	#define IPPVIDEO_API __declspec(dllimport)
#endif



#include  "IppBase.h"



//////////////////////////////////////////////////////////////////////
//   class  :   IIPPCodec
//   brief  :   ipp video encode interface
//   author :   dengchao
//   log    :     
//               
//////////////////////////////////////////////////////////////////////
class IIPPCodec
{
public:
	virtual ~IIPPCodec();

	virtual  BOOL  Initialize(LPDIRECT3DDEVICE9 pD3dDevice, IPPSpec_Params_t const& Params) = 0;
	virtual  void  CleanUp( ) = 0;

	virtual  void  SetViewport(SIZE Viewport) = 0;
	virtual  void  GetViewport(SIZE* Viewport) const = 0;

	virtual  BOOL  Prepare() = 0;
	virtual  void  Flush() = 0;
	virtual  void  End() = 0;
};

#define	FN_CREATE_IPPCODEC "CreateIPPCodec"
typedef IIPPCodec* (*fnCreateIPPCodec)();

extern "C" IPPVIDEO_API IIPPCodec* CreateIPPCodec();


#endif //_INCLUDE_IPPINTERFACE_H_