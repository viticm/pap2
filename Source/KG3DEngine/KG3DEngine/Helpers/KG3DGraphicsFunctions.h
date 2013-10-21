////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGraphicsFunctions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-31 10:37:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DGRAPHICSFUNCTIONS_H_
#define _INCLUDE_KG3DGRAPHICSFUNCTIONS_H_

////////////////////////////////////////////////////////////////////////////////
class IEKG3DSceneOutputWnd;
class KG3DFrustum;

HRESULT ComputeFrustrumBySelectionRect(RECT rectSel, IEKG3DSceneOutputWnd& wndOut, KG3DFrustum& frustum);

#endif //_INCLUDE_KG3DGRAPHICSFUNCTIONS_H_
