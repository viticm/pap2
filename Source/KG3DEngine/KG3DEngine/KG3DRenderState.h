#pragma once

//#include "engine/KG_SimpleVector.h"

enum enuStateBlockType
{
	STATE_BLOCK_RENDERSTATE,
	STATE_BLOCK_TEXTURESTATE,
	STATE_BLOCK_SAMPLERSTATE,
	STATE_BLOCK_FORCE_DWORD = 0xffffffff,
};

struct StateBlock
{
	DWORD m_dwType;
	union 
	{
		D3DRENDERSTATETYPE m_dwRenderState;
		struct
		{
			DWORD m_dwTextureStageIndex;
			D3DTEXTURESTAGESTATETYPE m_dwTextureState;
		};
		struct  
		{
			DWORD dwSampler;
			D3DSAMPLERSTATETYPE dwType;
		};
	};
	DWORD m_dwValue;
};


class KG3DRenderState
{
public:
	KG3DRenderState(void);
	~KG3DRenderState(void);
	HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value);
	HRESULT SetSamplerState(
		DWORD Sampler,
		D3DSAMPLERSTATETYPE Type,
		DWORD Value
		);
	void Restore();
private:
	std::vector<StateBlock> m_vecStates;	//用SimpleVector的目的是减少内存分配，但是vector的reserve有同样的作用，用vector就不用把SimpleVector的模板放在头文件那增加每个使用RenderState的东西的编译时间了
};

KG3DRenderState& g_GetRenderStateMgr();
