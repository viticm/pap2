////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTextureHelpers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-22 16:38:46
//  Comment     : 
//
/*
用于处理贴图，纯手工的采样。
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DTEXTUREHELPERS_H_
#define _INCLUDE_KG3DTEXTUREHELPERS_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DTexture;

namespace KG3D_CUSTOM_HELPERS
{
	class TextureSampler	//自定义的贴图采样器，可以放大缩小贴图
	{
	private:
		typedef std::pair<int, int> PixelPos;
		typedef std::pair<int, int> TexSize;
		typedef std::pair<float, float> ScaleGroup;
		BOOL m_bResult;
	public:
		enum{TEX_MAX_SIZE = 4096,};
		TextureSampler( KG3DTexture* pTex	//源
			, DWORD* pdwResultBuffer		//目标
			, INT nResultBufferSize			//目标Buffer大小，必须能放下nTargetWidth*nTargetHeight的内容
			, INT nSampleTimes				//采样次数
			, INT nTargetWidth				//目标宽
			, INT nTargetHeight				//目标高
			, BOOL bSampleAlpha = FALSE);	//把KG3DTexture采样到pdwResultBuffer中去
		BOOL SampleDWORDArrayEx(DWORD* pTexPixels	//源
			, INT nTexWidth					//源宽
			, INT nTexHeight				//源高
			, DWORD* pdwResultBuffer		//目标
			, INT nResultBufferSize			//目标Buffer大小，必须能放下nTargetWidth*nTargetHeight的内容
			, INT nSampleTimes
			, INT nTargetWidth
			, INT nTargetHeight
			, BOOL bSampleAlpha = FALSE);
		operator BOOL() const{return m_bResult;}
	private:
		/*
		inline PixelPos CalculateSamplePixelPos(PixelPos TargetPos, PixelPos SampleGroupPointPos, ScaleGroup SrcToDesXY, INT nSampleTimes)///fSampleGap
		{
		INT nBeginX = TargetPos.first * SrcToDesXY.first;
		INT nBeginY = TargetPos.second * SrcToDesXY.second;
		INT nPointX = nBeginX + 1*SrcToDesXY.first/nSampleTimes*SampleGroupPointPos.second;
		INT nPointY = nBeginY + 1*SrcToDesXY.second/nSampleTimes*SampleGroupPointPos.second;
		return PixelPos(nPointX, nPointY);
		}
		*/
		///上面是完整版本，下面是加速版本
		inline PixelPos CalculateSamplePixelPos(PixelPos SampleGroupTopLeftPos
			, PixelPos SampleGroupPointPos
			, ScaleGroup SampleGap)///SampleGap = SrcToDes/nSampleTimes
		{
			return PixelPos(SampleGroupTopLeftPos.first + (int)(1 * SampleGap.first * (float)SampleGroupPointPos.first)
				, SampleGroupTopLeftPos.second + (int)(1 * SampleGap.second * (float)SampleGroupPointPos.second));
		}
		inline PixelPos CalculateFirstPixelPosInSampleGroup(PixelPos TargetPos, ScaleGroup SrcToDesXY)
		{
			return PixelPos((int)((float)TargetPos.first*SrcToDesXY.first), (int)((float)TargetPos.second*SrcToDesXY.second));
		}
		inline D3DCOLOR GetColorAtPos(D3DCOLOR* pPixels, TexSize TSize, PixelPos Pos)
		{
			_ASSERTE(Pos.first >=0 && Pos.second >= 0 
				&&Pos.first < TSize.first && Pos.second < TSize.second);
			return pPixels[TSize.first * Pos.second + Pos.first];
		}
		inline void SetColorAtPos(D3DCOLOR* pDes, INT nDesSize, INT nPos, D3DCOLOR color)
		{
			_ASSERTE(nPos >= 0 && nPos < nDesSize);
			pDes[nPos] = color ;
		}
		
	};
	class TextureToGrayArray	//把RGBA四色数组变到灰度数组
	{
	private:
		BOOL m_bResult;
	public:
		TextureToGrayArray(DWORD* pdwSource, INT nArraySize, UCHAR* pResult, INT nResultArraySize);///这种输出的是UCHAR的
		TextureToGrayArray(DWORD* pdwSource, INT nArraySize, DWORD* pResult, INT nResultArraySize, BOOL bAlphaTo255 = TRUE);///这种输出的是全DWORD的
		operator BOOL() const{return m_bResult;}
	};

	HRESULT TextureTexToDWORDBuffer(KG3DTexture* pTexture, DWORD* pdwArray, INT nInputArraySize);	//把贴图中的像素拷贝到DWORD数组中
	HRESULT TextureDWORDBufferToTex(KG3DTexture* pTexture, DWORD* pdwArray, INT nInputArraySize);	//把一堆DWORD压入Texture
	BOOL TextureGrayBufferToDWORDBuffer(UCHAR* pGray, INT nGrayArraySize, DWORD* pColor, INT nColorArraySize);	//把灰度数组变成XRGB数组
};

#endif //_INCLUDE_KG3DTEXTUREHELPERS_H_
