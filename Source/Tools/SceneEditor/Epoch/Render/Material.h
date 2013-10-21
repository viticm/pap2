// File: Material.h
// Desc: 这时一个广义的概念

#ifndef MATERIAL_H
#define MATERIAL_H

#include"..\Common\String.h"
#include"BlendMode.h"
#include<vector>
using std::vector;

#include<d3d9types.h>

namespace Render
{
/*
class DLLAPI Material
{
private:
	D3DMATERIAL9 m_dxMaterial;

	class DLLAPI TextureLayer
	{
	public:
		enum TextureAddressMode
		{
			TAM_WRAP,
			TAM_MIRROR,
			TAM_CLAMP
		};

	public:
		TextureLayer(bool bLoadNow = true);
		TextureLayer(const Common::String& sTextureName, unsigned int TexCoordSet = 0, bool bLoadNow = true);
		TextureLayer(const TextureLayer& tl);
		TextureLayer& operator = (const TextureLayer& tl);
		~TextureLayer();

		void GetTextureName(Common::String& sTexName) const;
		void SetTextureName(const Common::String& sTexName);
	
		void SetAnimatedTextureName(const Common::String& sTexName, unsigned int NumOfFrames, float Duration);
		void SetAnimatesTextureName(const vector<Common::String>& svTexNames, float Duration);
		void GetTextureDimensions(unsigned int IndexOfFrame = 0, unsigned int& Width, unsigned int& Height) const;
		void SetCurrentFrameIndex(unsigned int IndexOfFrame);
		unsigned int GetCurrentFrameIndex() const;
		void GetTextureName(unsigned int IndexOfFrame) const;
		unsigned int GetNumOfFrames() const;
		unsigned int GetTextureCoordSet() const;
		void SetTextureCoordSet(unsigned int TexCoordSet);
		void SetTextureTransform(const D3DXMATRIX& mtx);

		void SetColorOperation(const TextureLayerBlendOperation op);
		void SetColorOperationEx(const TextureLayerBlendOperationEx opex, const TextureLayerBlendSource Source1 = TLBS_TEXTURE, const TextureLayerBlendSource Source2 = TLBS_CURRENT, D3DCOLOR Color1 = D3DCOLOR_XRGB(255, 255, 255), D3DCOLOR Color2 = D3DCOLOR_XRGB(255, 255, 255), float ManualBlendFactor = 0.f);
		void SetAlphaOperation(const TextureLayerBlendOperation opex, const TextureLayerBlendSource Source1 = TLBS_TEXTURE, const TextureLayerBlendSource Source2 = TLBS_CURRENT, float Arg1 = 1.f, float Arg2 = 1.f, float ManualBlendFactor = 0.f);
		TextureLayerBlendMode GetColorBlendMode() const;
		TextureLayerBlendMode GetAlphaBlendMode() const;

	private:
		enum
		{
			MAXNUM_FRAMES = 32
		};

		Common::String m_sNames[MAXNUM_FRAMES];
		unsigned int m_NumOfFrames;
		unsigned int m_CurrentIndexOfFrame;




	};

};
*/
}  // namespace

#endif