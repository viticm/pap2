// File: BlendMode.cpp
// Desc:

#include"BlendMode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Render
{

bool operator == (const TextureLayerBlendModeEx& a, const TextureLayerBlendModeEx& b)
{
	if(a.m_BlendType != b.m_BlendType)
	{
		return false;
	}

	if(a.m_BlendOperation == b.m_BlendOperation &&
		a.m_Source1 == b.m_Source1 &&
		a.m_Source2 == b.m_Source2 &&
		a.m_Factor == b.m_Factor)
	{
		if(a.m_BlendType == TLBT_COLOR)
		{
			if(a.m_ColorArg1 == b.m_ColorArg1 &&
				a.m_ColorArg2 == b.m_ColorArg2)
			{
				return true;
			}
		}
		else  // a.m_BlendType == TLBT_ALPHA
		{
			if(a.m_AlphaArg1 == b.m_AlphaArg1 &&
				a.m_AlphaArg2 == b.m_AlphaArg2)
			{
				return true;
			}
		}
	}

	return false;
}

bool operator != (const TextureLayerBlendModeEx& a, const TextureLayerBlendModeEx& b)
{
	if(a.m_BlendType != b.m_BlendType)
	{
		return true;
	}

	if(a.m_BlendOperation == b.m_BlendOperation &&
		a.m_Source1 == b.m_Source1 &&
		a.m_Source2 == b.m_Source2 &&
		a.m_Factor == b.m_Factor)
	{
		if(a.m_BlendType == TLBT_COLOR)
		{
			if(a.m_ColorArg1 == b.m_ColorArg1 &&
				a.m_ColorArg2 == b.m_ColorArg2)
			{
				return false;
			}
		}
		else  // a.m_BlendType == TLBT_ALPHA
		{
			if(a.m_AlphaArg1 == b.m_AlphaArg1 &&
				a.m_AlphaArg2 == b.m_AlphaArg2)
			{
				return false;
			}
		}
	}

	return true;
}

}  // namespace