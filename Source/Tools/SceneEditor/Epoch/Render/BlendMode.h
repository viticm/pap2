// File: BlendMode.h
// Desc:

#ifndef BLENDMODE_H
#define BLENDMODE_H

#include"..\Common\DLLAPI.h"
#include<d3d9.h>

namespace Render
{

enum TextureLayerBlendType
{
	TLBT_COLOR,
	TLBT_ALPHA
};

enum TextureLayerBlendOperation  // Supported by both MultiTexture and MultiPass
{
	TLBO_REPLACE,  // replace all current color with texture
	TLBO_ADD,  // add color components together
	TLBO_MODULATE,  // multiply color components together
	TLBO_ALPHA_BLEND  // Blend based on texture alpha
};

enum TextureLayerBlendOperationEx
{
	TLBOx_SOURCE1,  // select source1
	TLBOx_SOURCE2,  // select source2
	TLBOx_MODULATE,  // multiply source1 and source2 together
	TLBOx_MODULATE_X2,  // as Modulate, but X2 afterwards
	TLBOx_MODULATE_X4,  // as modulate, but X4 afterwards
	TLBOx_ADD,  // Add source1 and source2 together
	TLBOx_ADD_SIGNED,  // As Add, but subtract 0.5
	TLBOx_ADD_SMOOTH,  // As add, but subtract product
	TLBOx_SUBTRACT,  // Subtract source2 from source1
	TLBOx_BLEND_DIFFUSE_ALPHA,  // source1 * diffuse_alpha + source2 * (1 - texture_alpha)
	TLBOx_BLEND_TEXTURE_ALPHA,  // source1 * texture_alpha + source2 * (1 - texture_alpha)
	TLBOx_BLEND_CURRENT_ALPHA,  // source1 * current_alpha + source2 * (1 - current_alpha), where current_alpha maybe come from previous stage
	TLBOx_BLEND_MANUAL_ALPHA,  // source1 * manual_alpha + source2 * (1 - manual_alpha)
	TLBOx_DOTPRODUCT  // dot product of source1 and source2
};

enum TextureLayerBlendSource
{
	TLBS_CURRENT,  // maybe come from previous stage
	TLBS_TEXTURE,
	TLBS_DIFFUSE,
	TLBS_SPECULAR,
	TLBS_MANUAL  // a color supplied manually
};

class DLLAPI TextureLayerBlendModeEx  // Wish all the graphic-cards can support all the blending operations.
{
public:
	TextureLayerBlendType m_BlendType;  // Blend color or alpha?
	TextureLayerBlendOperationEx m_BlendOperation;  // How to blend?
	TextureLayerBlendSource m_Source1;  // Source1
	TextureLayerBlendSource m_Source2;  // Source2

	D3DCOLOR m_ColorArg1;  // color value (float type) for manual source1
	D3DCOLOR m_ColorArg2;  // color value (float type) for manual source2
	float m_AlphaArg1;  // alpha value for manual source1
	float m_AlphaArg2;  // alpha value for manual source2
	float m_Factor;  // Manual blending factor
};

DLLAPI bool operator == (const TextureLayerBlendModeEx& a, const TextureLayerBlendModeEx& b);
DLLAPI bool operator != (const TextureLayerBlendModeEx& a, const TextureLayerBlendModeEx& b);

}  // namespace

#endif