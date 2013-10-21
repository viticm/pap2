////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DMeshBone.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-26 9:38:04
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMESHBONE_H_
#define _INCLUDE_KG3DMESHBONE_H_

#include "IEKG3DMeshBone.h"

class KG3DScene;
class KG3DModel;

class KG3DMeshBone : public IEKG3DMeshBone
{
public://for IE interface
    virtual HRESULT SetSelectedBone(const TCHAR cszName[]);
    virtual HRESULT GetSelBoneName(TCHAR szName[], INT nLen, DWORD *pdwType); 

public :
	void    SetAttachScene(KG3DScene* pScene);
    HRESULT Attach(KG3DModel* pModel, KG3DScene* pScene);
    HRESULT Render();
    HRESULT UpdateSelBone(BOOL bSelectedUpdate = FALSE);
    
    

private :
    HRESULT _CreateMesh();
    HRESULT AddSubMesh(D3DXVECTOR3 v1, D3DXVECTOR3 v2, TCHAR* szBoneName, DWORD dwType = 1);
           
private :
    struct _SubMeshInfo         
    {
        D3DXVECTOR3 vPosition[6];
        DWORD       dwIndex[24];
        TCHAR       szBoneName[128];
        DWORD       dwType;             // 0 : origin 1 : bone 2 : socket
    };
    vector<_SubMeshInfo> m_vecSubMeshInfo;

    LPD3DXMESH  m_pSysMesh;
    int         m_nSelSubIndex;
    KG3DModel*  m_pAttachModel;
    KG3DScene*  m_pAttachScene;

    DWORD       m_dwPrevTime;
    int         m_nSeledIndex;
    int         m_nBindIndex;

public :
    D3DXMATRIX  m_matWorld;

public :
    KG3DMeshBone(void);
    ~KG3DMeshBone(void);
};

#endif //_INCLUDE_KG3DMESHBONE_H_
