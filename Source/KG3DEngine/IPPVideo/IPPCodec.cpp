////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IPPCodec.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-08 14:57:32
//  Comment     : ipp encoder
//
////////////////////////////////////////////////////////////////////////////////


#include  "IPPCodec.h"
#include  "IPPEncoder.h"

IIPPCodec::~IIPPCodec()
{
}

static CIPPEncoder g_CodecPtr;


BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


extern "C" IPPVIDEO_API IIPPCodec* CreateIPPCodec()
{
	return &g_CodecPtr;
}