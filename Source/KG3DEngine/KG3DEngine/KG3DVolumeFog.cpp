////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DVolumeFog.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-20 10:25:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DVolumeFog.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DFogPlane& KG3DFogPlane::operator = (const KG3DFogPlane& fp)
{
    if (&fp != this)
    {
        m_VertexOffset  = fp.m_VertexOffset;
        m_NumOfVertices = fp.m_NumOfVertices;
    }

    return *this;
}

FogVertex* KG3DFogPlane::GetVertex(FogVertex* pFVBuffer, unsigned index) const
{
    if (!pFVBuffer)
    {
        return 0;
    }
    if (index >= m_NumOfVertices)  
    {
        return 0;
    }

    return pFVBuffer + m_VertexOffset + index;
}

HRESULT KG3DFogPlane::Render()
{
    return g_pd3dDevice->DrawPrimitive(
        D3DPT_TRIANGLEFAN, m_VertexOffset, m_NumOfVertices - 2
    );
}

