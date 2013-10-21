// File: TerrainTreeDesc.cpp
// Desc:

#include"TerrainTreeDesc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

TerrainTreeDesc& TerrainTreeDesc::operator = (const TerrainTreeDesc& ttd)
{
	if(&ttd != this)
	{
		m_TTDFilename = ttd.m_TTDFilename;

		m_FNs = ttd.m_FNs;
		m_QLs = ttd.m_QLs;
		m_XSpan = ttd.m_XSpan;
		m_ZSpan = ttd.m_ZSpan;
		m_Variant = ttd.m_Variant;
		m_Factor = ttd.m_Factor;
		m_MaxLOD = ttd.m_MaxLOD;
		m_TerrainBaseColor = ttd.m_TerrainBaseColor;

		m_Textures = ttd.m_Textures;
		m_AlphaMaps = ttd.m_AlphaMaps;
		m_LightMapFilename = ttd.m_LightMapFilename;
	}

	return *this;
}

void TerrainTreeDesc::SetName(const Common::String& sName)
{
	m_TTDFilename = sName;
	const char* p = m_TTDFilename.c_str();
}

void TerrainTreeDesc::ReadFile()
{
	Common::INIFile ttdf;
	ttdf.SetFilename(m_TTDFilename);
	ttdf.Open();

	Common::String sTemp;

	ttdf.GetINI("TreeInfo", "FNs", sTemp);  // FNs
	m_FNs = sTemp.IntValue();

	ttdf.GetINI("TreeInfo", "QLs", sTemp);  // QLs
	m_QLs = sTemp.IntValue();

	ttdf.GetINI("TreeInfo", "XSpan", sTemp);  // XSpan
	m_XSpan = sTemp.IntValue();

	ttdf.GetINI("TreeInfo", "ZSpan", sTemp);  // ZSpan
	m_ZSpan = sTemp.IntValue();

	ttdf.GetINI("TreeInfo", "Variant", sTemp);  // Variant
	m_Variant = sTemp.FloatValue();

	ttdf.GetINI("TreeInfo", "Factor", sTemp);  // Scalling Factor
	m_Factor = sTemp.FloatValue();

	ttdf.GetINI("TreeInfo", "MaxLOD", sTemp);  // MaxLOD
	m_MaxLOD = sTemp.IntValue();

	int r, g, b;
	ttdf.GetINI("TreeInfo", "TerrainBaseColor_R", sTemp);
	r = sTemp.IntValue();
	ttdf.GetINI("TreeInfo", "TerrainBaseColor_G", sTemp);
	g = sTemp.IntValue();
	ttdf.GetINI("TreeInfo", "TerrainBaseColor_B", sTemp);
	b = sTemp.IntValue();

	m_TerrainBaseColor = 0xFF000000 | (r << 16) | (g << 8) | b;

	/*
	// Vector of Texture Filenames
	m_Textures.clear();
	ttdf.GetINI("Textures", "NumOfTextures", sTemp);
	unsigned int NumOfItems = sTemp.IntValue();
	unsigned int i = 0;
	for(; i < NumOfItems; ++i)
	{
		ttdf.GetINI("Textures", Common::String((int)i), sTemp);
		m_Textures.push_back(sTemp);
	}

	// Vector of AlphaMap Filenames
	m_AlphaMaps.clear();
	ttdf.GetINI("AlphaMap", "NumOfAlphaMap", sTemp);
	NumOfItems = sTemp.IntValue();
	for(i = 0; i < NumOfItems; ++i)
	{
		ttdf.GetINI("AlphaMap", Common::String((int)i), sTemp);
		m_AlphaMaps.push_back(sTemp);
	}

	// LightMap Filename
	ttdf.GetINI("LightMap", "Filename", m_LightMapFilename);
	*/

	ttdf.Close();
}

}  // namespace