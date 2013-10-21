#pragma once

#include "KG3DModelSpeedTree.h"


class KG3DRepresentSpeedTree
{
public:
	////////////////////////////////////////////////
	// prepare / flush render
	static void  PrepareRender( D3DXMATRIX matrix, UINT uOpt, KG3DMaterial* pMaterial, KG3DModel* pSpeedTree );
	static void  FlushRender( );

	
protected:
	////////////////////////////////////////////////
	// render
	//static void renderCrown( KG3DMaterial* pMaterial, KG3DModelSpeedTree* pTreeFirst );
	//static void renderBody( KG3DModelSpeedTree* pTreeFirst );


	////////////////////////////////////////////////
	// pre / post render
	static void  preRender( KG3DMaterial* pMaterial );
	static void  postRender( KG3DMaterial* pMaterial );


protected:
	////////////////////////////////////////////////
	// member
	typedef std::map< KG3DMaterial*, KG3DModelSpeedTree* >	SpeedTreeMap_t;


	static SpeedTreeMap_t	m_SpeedTreeMap;
	static DWORD			m_dwSpecular;
	static D3DXMATRIX		m_matSave;

};
