////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : GraphicsStruct.h
//  Version     : 
//  Creator     : 
//  Create Date : 
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_GRAPHICSSTRUCT_H_
#define _INCLUDE_GRAPHICSSTRUCT_H_

#include "KG3DRect.h"
#include "KG3DBBox.h"

VOID BoxToRect(const BBox& box, FRECT& rect);
////////////////////////////////////////////////////////////////////////////////

//#include "KG3DStructs.h"
class KG3DModel;



/*
struct LocalBBox {

    / *
    A1 front_top_left       0
    B1 front_top_right		1
    C1 back_top_right		2
    D1 back_top_left        3
    A2 front_bottom_left    4
    B2 front_bottom_right	5
    C2 back_bottom_right    6
    D2 back_bottom_left		7
    * /

    D3DXVECTOR3 vPos[8];

    operator D3DXVECTOR3*() {
        return vPos;
    }
    
    float Height() const {
        return D3DXVec3Length(&(vPos[0] - vPos[4]));
    }

    float Width() const {
        return D3DXVec3Length(&(vPos[0] - vPos[3]));
    }

    float Length() const {
        return D3DXVec3Length(&(vPos[0] - vPos[1]));
    }

    D3DXVECTOR3 Center() const {
        return (vPos[3] + vPos[5]) * 0.5f;
    };
};
*/



namespace GraphicsStruct
{
	struct RenderStateAlpha
	{
		DWORD dwBackAlphaRef;
		DWORD dwBackAlphaTestEnable;
		DWORD dwBackAlphaBlendEnable;
		DWORD dwBackSrcBlend;
		DWORD dwBackDestlend;
		DWORD dwBackAlphaFuc;

		RenderStateAlpha(DWORD AlphaRef,DWORD AlphaTestEnable,
			DWORD AlphaBlendEnable,DWORD SrcBlend,DWORD DestBlend,DWORD AlphaFuc = D3DPCMPCAPS_GREATEREQUAL);
		~RenderStateAlpha();
	};
	
	struct RenderStateCullMode
	{
		DWORD dwBackCullModel;
		RenderStateCullMode(DWORD dwCullMode);
		~RenderStateCullMode();
	};
	struct RenderState
	{
		D3DRENDERSTATETYPE eStateType;
		DWORD dwBackRenderState;
		RenderState(D3DRENDERSTATETYPE eType,DWORD dwState);
		~RenderState();
	};
	struct TextureStageState 
	{
		DWORD m_dwStage;
		D3DTEXTURESTAGESTATETYPE m_eStateType;
		DWORD m_dwBackTexStageState;
		TextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE eType, DWORD dwState);
		~TextureStageState();
	};
	struct SamplerState 
	{
		DWORD m_dwSampler;
		D3DSAMPLERSTATETYPE	m_eStateType;
		DWORD m_dwBackSamplerState;
		SamplerState(DWORD dwSampler, D3DSAMPLERSTATETYPE eType, DWORD dwValue);
		~SamplerState();
	private:
		BOOL m_bHasModify;
	};
};


//////////////////////////////////////////////////////////////////////////
// 
//  Sphere BBox Use To Camera Obstructd Check
//
//

enum 
{
    CROSS_INSIDE,
    CROSS_TANGENCY,
    CROSS_PIERCE
};

struct ObstructdInfo
{
    DWORD       ObsType;
    D3DXVECTOR3 vUpdate;
    D3DXVECTOR3 vOffset;
};

///请不要在这里放会涉及引擎非接口类的类，因为这个文件是被SceneEditor使用的，如果涉及
///引擎的非接口类，会引发连锁编译，这样DLL分离就没有意义了
///KG3DCameraSphere被移动到KG3DGraphicesStructEx.h了（在Helper下面）

#endif //_INCLUDE_GRAPHICSSTRUCT_H_
