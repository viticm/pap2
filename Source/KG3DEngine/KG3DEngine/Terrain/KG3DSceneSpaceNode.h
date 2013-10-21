////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSpaceNode.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-12 14:17:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENESPACENODE_H_
#define _INCLUDE_KG3DSCENESPACENODE_H_

enum {
	SceneNodeType_None,
	SceneNodeType_Pvs,
	SceneNodeType_OutDoor
};

#include "KG3DFrustum.h"

////////////////////////////////////////////////////////////////////////////////
typedef std::vector<KG3DFrustum>	TypeFrustums;

class KG3DSceneSpaceNode//空间分割节点
{
public:
	typedef KGCH::TFrontAccessProxy<KG3DSceneSpaceNode*>	TypeAccessProxy;	

	virtual DWORD GetSceneNodeType() { return SceneNodeType_None; }
	virtual ULONG	GetHandle();
	virtual HRESULT SetHandle(ULONG handle);//只能在接入Scene的时候设一次，不能随便设，否则Scene里面的指针就是错的

	virtual KG3DSceneSpaceNode* GetParent();
	virtual TypeAccessProxy&	GetChildrenAccessProxy();//遍历子节点的时候的时候用这个

	virtual const TypeFrustums& GetFrustums();
	virtual HRESULT ClearFrustums();
	virtual HRESULT InsertFrustum(KG3DFrustum& frustrum);

	virtual BOOL IsPositionInSpace(D3DXVECTOR3& vPos);
	virtual BOOL IsPositionsInSpace(D3DXVECTOR3* pVers,int nCount);
	virtual HRESULT CalcFrustum(const TypeFrustums& vInputFrustum, DWORD dwLookMode) = 0;
	virtual HRESULT CalcFrustum(const KG3DFrustum& Frustum, DWORD dwLookMode);

	BOOL ProcessPosition(KG3DSceneSpaceNode** ppNode,D3DXVECTOR3& vPos,BOOL bTryParentNode);

	static HRESULT ConnectTwoNodes(KG3DSceneSpaceNode* pParent,KG3DSceneSpaceNode* pChild);
	static HRESULT DisConnectTwoNode(KG3DSceneSpaceNode* pParent,KG3DSceneSpaceNode* pChild);	

	static KG3DSceneSpaceNode* GetTopVisableNode()  { return m_spTopVisableNode; }
	static void SetTopVisableNode(KG3DSceneSpaceNode* pNode) { m_spTopVisableNode = pNode; }
	static KG3DSceneSpaceNode* m_spTopVisableNode;

    virtual LPCTSTR GetNodeName()                            { return TEXT("Null"); }

	//set<KG3DSceneSpaceNode*>* GetChildNodeSet() { return &m_setSpaceNodeChild; }//用GetChildrenAccessProxy

	KG3DSceneSpaceNode();
	virtual ~KG3DSceneSpaceNode();
protected:	
	VOID	SetFrustums(const TypeFrustums& frustumsInput);	
private:
	UINT m_uHandle;//句柄
	//BOOL m_bVisble;//没有检查到应用
	//BOOL m_bEnable;//该空间节点是否有效//这个变量也没有检查到引用
	KG3DSceneSpaceNode* m_lpSpaceNodeParent;

	set<KG3DSceneSpaceNode*>m_setSpaceNodeChild;	//外面用GetChildrenAccessProxy遍历

	typedef set<KG3DSceneSpaceNode*> TypeCon;
	KGCH::TFrontAccessProxySolid<TypeCon>	m_accessProxy;

	vector<KG3DFrustum> m_vFrustum;
	//vector<KG3DFrustum> m_vParentFrustum;//这个变量是用来计算Portal系列裁剪的时候用的，可以变成临时变量，不需要作为成员
};

#endif //_INCLUDE_KG3DSCENESPACENODE_H_
