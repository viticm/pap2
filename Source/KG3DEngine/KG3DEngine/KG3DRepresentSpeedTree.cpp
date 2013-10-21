#include "stdafx.h"
#include "KG3DRepresentSpeedTree.h"
#include "KG3DEngineManager.h"
#include "KG3DGraphicsTool.h"
#include "KG3DMesh.h"
#include "KG3DScene.h"
#include "KG3DModelTable.h"
#include "KG3DMeshTable.h"
#include "KG3DMaterialTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//KG3DRepresentSpeedTree::SpeedTreeMap_t	KG3DRepresentSpeedTree::m_SpeedTreeMap;
//DWORD KG3DRepresentSpeedTree::m_dwSpecular = 0;
//D3DXMATRIX	KG3DRepresentSpeedTree::m_matSave;

//
//void  KG3DRepresentSpeedTree::PrepareRender( D3DXMATRIX matrix, UINT uOpt, KG3DMaterial* pMaterial, KG3DModel* pSpeedTree )
//{
//	// check valid
//	KG3DModelSpeedTree* pTreeInsert = (KG3DModelSpeedTree*)( pSpeedTree );
//
//	if ( ( NULL == pSpeedTree )
//		|| ( MESHTYPE_SPEEDTREE != pSpeedTree->GetType() )
//		|| ( NULL != pTreeInsert->GetRenderNext() )		// 如果已经放置到预渲染队列
//		|| ( pTreeInsert == pTreeInsert->GetRenderNext() ) )	// 链表尾元素链接指针指向自己
//	{
//		return ;
//	}
//
//
//	// insert
//	SpeedTreeMap_t::iterator iterMap = m_SpeedTreeMap.find( pMaterial );
//	
//	if ( iterMap == m_SpeedTreeMap.end() )
//	{
//		std::pair<SpeedTreeMap_t::iterator, bool> pairMap;
//
//		pairMap = m_SpeedTreeMap.insert( SpeedTreeMap_t::value_type( pMaterial, pTreeInsert ) );
//		if ( pairMap.second )
//		{
//			iterMap = pairMap.first;
//		}
//
//	}
//
//	if ( iterMap != m_SpeedTreeMap.end() )
//	{
//#ifdef _DEBUG
//		KG3DModelSpeedTree* pTreeFirst = iterMap->second;
//		
//		if ( NULL == pTreeFirst )
//		{
//			assert( NULL != pTreeFirst );
//		}
//#endif
//
//		// 因为先插入了一次，这里再次插入一次，减少一次判断
//		pTreeInsert->SetRenderNext( iterMap->second );
//		pTreeInsert->SetRenderMatrix( uOpt, matrix );
//		iterMap->second = pTreeInsert;
//
//	}
//
//}
//
//void  KG3DRepresentSpeedTree::FlushRender( )
//{
//	// 保存矩阵
//	g_pd3dDevice->GetTransform(D3DTS_WORLD, &m_matSave);
//
//
//	// 渲染队列
//	GraphicsStruct::RenderStateAlpha Alpha(0x90,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
//	SpeedTreeMap_t::iterator iterMap = m_SpeedTreeMap.begin( );
//	
//	for ( ; iterMap != m_SpeedTreeMap.end( ); ++ iterMap )
//	{
//		// 材质队列是否为空
//		KG3DModelSpeedTree* pTreeFirst = iterMap->second;
//
//		if ( NULL == pTreeFirst )
//		{
//			continue ;
//		}
//
//
//		//renderBody( pTreeFirst );
//		//renderCrown( iterMap->first, pTreeFirst );
//
//	}
//
//
//	g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_matSave);
//	m_SpeedTreeMap.clear();
//}

////////////////////////////////////////////////
// render
//void KG3DRepresentSpeedTree::renderCrown( KG3DMaterial* pMaterial, KG3DModelSpeedTree* pTreeFirst )
//{
//	// 准备材质
//	preRender( pMaterial );
//
//
//	// 渲染队列
//	KG3DModelSpeedTree* pSpeedTree = pTreeFirst; 
//	D3DXMATRIX matWorld;
//	UINT Option;
//
//	for ( ; ; )
//	{
//		pSpeedTree->GetRenderMatrix( Option, matWorld );
//		matWorld *= m_matSave;
//
//		g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld );
//		pSpeedTree->RenderCrown( Option );
//
//
//		pTreeFirst = pSpeedTree;
//		pSpeedTree = pSpeedTree->GetRenderNext();
//
//		pTreeFirst->SetRenderNext( NULL );
//		if ( pTreeFirst == pSpeedTree )
//		{
//			break ;
//		}
//
//	}
//
//
//
//	// 回复材质
//	postRender( pMaterial );
//
//}

//void KG3DRepresentSpeedTree::renderBody( KG3DModelSpeedTree* pTreeFirst )
//{
//	// 渲染队列
//	KG3DModelSpeedTree* pSpeedTree = pTreeFirst; 
//	D3DXMATRIX matWorld;
//	UINT Option;
//
//	for ( ; ; )
//	{
//		pSpeedTree->GetRenderMatrix( Option, matWorld );
//		matWorld *= m_matSave;
//
//		g_pd3dDevice->SetTransform( D3DTS_WORLD,&matWorld );
//		pSpeedTree->RenderBody( Option );
//
//		pTreeFirst = pSpeedTree;
//		pSpeedTree = pSpeedTree->GetRenderNext();
//
//		if ( pTreeFirst == pSpeedTree )
//		{
//			break ;
//		}
//
//	}
//
//}


////////////////////////////////////////////////
// pre / post render
//void  KG3DRepresentSpeedTree::preRender( KG3DMaterial* pMaterial )
//{
//	if( pMaterial)
//		pMaterial->SetMaterial(0, 1.0f, 0, NULL);
//
//	g_pd3dDevice->GetRenderState(D3DRS_SPECULARENABLE,&m_dwSpecular);
//	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE);
//
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_CURRENT);
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
//	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
//}
//
//void  KG3DRepresentSpeedTree::postRender( KG3DMaterial* pMaterial )
//{
//	if(pMaterial)
//		pMaterial->RestoreMaterial(0);
//	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,m_dwSpecular);
//
//}
