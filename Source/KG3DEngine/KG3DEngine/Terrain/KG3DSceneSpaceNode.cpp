////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSpaceNode.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-12 14:18:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneSpaceNode.h"

#include "KG3DGraphicsTool.h"	//g_cGraphicsTool
#include "KG3DScene.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSceneSpaceNode::KG3DSceneSpaceNode()
	:m_uHandle(0)
	,m_accessProxy(m_setSpaceNodeChild)
{
	
	m_lpSpaceNodeParent = NULL;
	//m_bEnable = 0;
	//m_bVisble = 0;
}

KG3DSceneSpaceNode:: ~KG3DSceneSpaceNode()
{
	for (set<KG3DSceneSpaceNode*>::iterator i = m_setSpaceNodeChild.begin();
		i != m_setSpaceNodeChild.end(); ++i)
		(*i)->m_lpSpaceNodeParent = NULL;

	DisConnectTwoNode(m_lpSpaceNodeParent, this);

	if (g_cGraphicsTool.GetCurScene())
		g_cGraphicsTool.GetCurScene()->UnRegisterSpaceNode(this);

	m_uHandle = 0;
	m_lpSpaceNodeParent = NULL;
	m_setSpaceNodeChild.clear();
}

BOOL KG3DSceneSpaceNode::IsPositionInSpace(D3DXVECTOR3& vPos)
{
	return FALSE;
}

HRESULT KG3DSceneSpaceNode::ConnectTwoNodes(KG3DSceneSpaceNode* pParent,KG3DSceneSpaceNode* pChild)
{
	pParent->m_setSpaceNodeChild.insert(pChild);
	pChild->m_lpSpaceNodeParent = pParent;
	return S_OK;
}

HRESULT KG3DSceneSpaceNode::DisConnectTwoNode(KG3DSceneSpaceNode* pParent,KG3DSceneSpaceNode* pChild)
{
	if (pParent && pChild->m_lpSpaceNodeParent == pParent)
	{
		pParent->m_setSpaceNodeChild.erase(pChild);
		pChild->m_lpSpaceNodeParent = NULL;
	}
	return S_OK;
}


BOOL KG3DSceneSpaceNode::ProcessPosition(KG3DSceneSpaceNode** ppNode,D3DXVECTOR3& vPos,BOOL bTryParentNode)
{
	if (IsPositionInSpace(vPos))//如果位置在当前的空间中，就尝试是否在当前空间的子结点中
	{
		set<KG3DSceneSpaceNode*>::iterator i = m_setSpaceNodeChild.begin();
		set<KG3DSceneSpaceNode*>::iterator iend = m_setSpaceNodeChild.end();
		while (i!=iend)
		{
			KG3DSceneSpaceNode* pNode = *i;

			if(pNode->ProcessPosition(ppNode,vPos,FALSE))
			{
				return TRUE;
			}
			i++;
		}
		//////////////////////////////////////////////////////////////////////////
		//如果在子空间中没有发现，就返回自己
		(*ppNode) = this;
		return TRUE;
	}
	else if(bTryParentNode)
	{
		if (m_lpSpaceNodeParent)//尝试父节点
		{
			return m_lpSpaceNodeParent->ProcessPosition(ppNode,vPos,TRUE);
		}
	}
	return FALSE;
}

KG3DSceneSpaceNode* KG3DSceneSpaceNode::m_spTopVisableNode = NULL;

BOOL KG3DSceneSpaceNode::IsPositionsInSpace(D3DXVECTOR3* pVers,int nCount)
{
	return FALSE;
}

HRESULT KG3DSceneSpaceNode::CalcFrustum( const KG3DFrustum& Frustum, DWORD dwLookMode )
{
	vector<KG3DFrustum> vFrustum;
	vFrustum.push_back(Frustum);
	return CalcFrustum(vFrustum, dwLookMode);
}

ULONG KG3DSceneSpaceNode::GetHandle()
{
	return m_uHandle;
}

HRESULT KG3DSceneSpaceNode::SetHandle( ULONG handle )
{
	//KGLOG_CHECK_ERROR(0 == m_uHandle && _T("只能在接入Scene的时候设一次，不能随便设，否则Scene里面的指针就是错的"));
	m_uHandle = handle;
	return S_OK;
}

KG3DSceneSpaceNode* KG3DSceneSpaceNode::GetParent()
{

	return m_lpSpaceNodeParent;
}

KG3DSceneSpaceNode::TypeAccessProxy& KG3DSceneSpaceNode::GetChildrenAccessProxy()
{
	return m_accessProxy;
}

const TypeFrustums& KG3DSceneSpaceNode::GetFrustums()
{
	return m_vFrustum;
}

HRESULT KG3DSceneSpaceNode::ClearFrustums()
{
	m_vFrustum.clear();
	return S_OK;
}

VOID KG3DSceneSpaceNode::SetFrustums(const TypeFrustums& frustumInput )
{
	m_vFrustum = frustumInput;
}

HRESULT KG3DSceneSpaceNode::InsertFrustum( KG3DFrustum& frustrum )
{
	m_vFrustum.push_back(frustrum);
	return S_OK;
}