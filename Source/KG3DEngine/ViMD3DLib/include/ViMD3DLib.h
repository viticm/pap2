//ViMD3DLib.h
//-------------------------------------------------------------------
/*///////////////////////////////////////////////////////////////////

DESCRIPTION:	VisuMotion D3D Library

CREATED BY:		Jens Meichsner

HISTORY:		Created 02/01/2006

*////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------

#ifndef _VIM_D3DLIB_
#define _VIM_D3DLIB_

//-------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////

#if(DIRECT3D_VERSION >= 0x0900)
	#define VIMD3D9		1
#else if(DIRECT3D_VERSION >= 0x0800)
	#define VIMD3D8		1
#endif

//-------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////

//VisuMotion D3D object
typedef void*	VIMD3D;

//Version of library
#define VIMD3DLIB_VERSION	1000

//-------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////
// import / export

// Dynamic linking against ViMD3DLib.dll
#ifndef VIM_D3D_API
#define VIM_D3D_API extern

//-------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////

// First call to the library
#ifdef VIMD3D9
	#define ViMD3D_InitLib ViMD3D_InitLib9
#else
	#define ViMD3D_InitLib ViMD3D_InitLib8
#endif
	BOOL ViMD3D_InitLib(TCHAR* szError, UINT version = VIMD3DLIB_VERSION);

// Retrieve information about the current display. Returns FALSE if 3d display or VisuMotion Configurator is not present.
typedef BOOL	(*FN_ViMD3D_GetInfo)(DWORD&, DWORD&, DWORD&, DWORD&, DWORD&, DWORD&, DWORD&);
VIM_D3D_API FN_ViMD3D_GetInfo ViMD3D_GetInfo;


// Initialize VIMD3D
#ifdef VIMD3D9
	#define ViMD3D_Create ViMD3D_Create9
	typedef BOOL	(*FN_ViMD3D_Create9)(IDirect3DDevice9*, VIMD3D*);
	VIM_D3D_API FN_ViMD3D_Create9 ViMD3D_Create9;
#elif defined(VIMD3D8)
	#define ViMD3D_Create ViMD3D_Create8
	typedef BOOL	(*FN_ViMD3D_Create8)(IDirect3DDevice8*, VIMD3D*);
	VIM_D3D_API FN_ViMD3D_Create8 ViMD3D_Create8;
#endif 



// Release VIMD3D
#ifdef VIMD3D9
	#define ViMD3D_Release ViMD3D_Release9
	typedef BOOL	(*FN_ViMD3D_Release9)(VIMD3D);
	VIM_D3D_API FN_ViMD3D_Release9 ViMD3D_Release9;
#elif defined(VIMD3D8)
	#define ViMD3D_Release ViMD3D_Release8
	typedef BOOL	(*FN_ViMD3D_Release8)(VIMD3D);
	VIM_D3D_API FN_ViMD3D_Release8 ViMD3D_Release8;
#endif



// Raster all rendered views:
/*
This function uses texture stage 0 & 1 and changes the blending mode and the depth test.
If the device was created with D3DCREATE_PUREDEVICE, the fuction won't be able to restore
the original state. Also make sure, that all vertex/pixel shaders and effects are deactivated
before calling it.
*/

#ifdef VIMD3D9
	#define ViMD3D_RasterTextures ViMD3D_RasterTextures9
	typedef BOOL	(*FN_ViMD3D_RasterTextures9)(VIMD3D, DWORD, DWORD, DWORD, DWORD, DWORD, IDirect3DTexture9**);
	VIM_D3D_API FN_ViMD3D_RasterTextures9 ViMD3D_RasterTextures9;

	#define ViMD3D_RasterTexturesUV ViMD3D_RasterTexturesUV9
	typedef BOOL	(*FN_ViMD3D_RasterTexturesUV9)(VIMD3D, DWORD, DWORD, DWORD, DWORD, DWORD, IDirect3DTexture9**, float*);
	VIM_D3D_API FN_ViMD3D_RasterTexturesUV9 ViMD3D_RasterTexturesUV9;

#elif defined(VIMD3D8)
	#define ViMD3D_RasterTextures ViMD3D_RasterTextures8
	typedef BOOL	(*FN_ViMD3D_RasterTextures8)(VIMD3D, DWORD, DWORD, DWORD, DWORD, DWORD, IDirect3DTexture8**);
	VIM_D3D_API FN_ViMD3D_RasterTextures8 ViMD3D_RasterTextures8;

	#define ViMD3D_RasterTexturesUV ViMD3D_RasterTexturesUV8
	typedef BOOL	(*FN_ViMD3D_RasterTexturesUV8)(VIMD3D, DWORD, DWORD, DWORD, DWORD, DWORD, IDirect3DTexture8**, float*);
	VIM_D3D_API FN_ViMD3D_RasterTexturesUV8 ViMD3D_RasterTexturesUV8;

#endif


////////////
// Utilities

// Calculate 3d focalpoint and 3d translation from 3d nearplane and 3d farplane 
typedef BOOL	(*FN_ViMD3Dutil_GetCamFocalTranslation)(float&, float&, float, float, float, float, float, DWORD, DWORD);
VIM_D3D_API FN_ViMD3Dutil_GetCamFocalTranslation ViMD3Dutil_GetCamFocalTranslation;


// Create a left hand projection matrix
// fovy = Field of view in the y direction, in radians.
typedef BOOL	(*FN_ViMD3Dutil_MatrixPerspectiveFovLH)(D3DMATRIX*, float, float, float,float, float, float, DWORD, DWORD);
VIM_D3D_API FN_ViMD3Dutil_MatrixPerspectiveFovLH ViMD3Dutil_MatrixPerspectiveFovLH;


// fovy = Field of view in the y direction, in radians.
typedef BOOL	(*FN_ViMD3Dutil_MatrixPerspectiveFovRH)(D3DMATRIX*, float, float, float,float, float, float, DWORD, DWORD);
VIM_D3D_API FN_ViMD3Dutil_MatrixPerspectiveFovRH ViMD3Dutil_MatrixPerspectiveFovRH;


// Applies a stereo modification to a given matrix. This function is less accurate then MatrixPerspectiveFovLH/MatrixPerspectiveFovRH.
typedef BOOL	(*FN_ViMD3Dutil_MatrixModify)(D3DMATRIX*, float, float, float, float, DWORD, DWORD);
VIM_D3D_API FN_ViMD3Dutil_MatrixModify ViMD3Dutil_MatrixModify;

//////////
// License

// Retrieve current license
typedef BOOL	(*FN_ViMD3D_GetLicense)(char*);
VIM_D3D_API FN_ViMD3D_GetLicense ViMD3D_GetLicense;


#endif

//-------------------------------------------------------------------

#endif _VIM_D3DLIB_

//-------------------------------------------------------------------
//EOF