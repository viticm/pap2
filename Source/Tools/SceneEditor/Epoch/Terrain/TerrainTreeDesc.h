// File: TerrainTreeDesc.h
// Desc: Corresponding to .ttd file

#ifndef TERRAINTREEDESC_H
#define TERRAINTREEDESC_H

#include"..\Common\Common.h"

#include<vector>
using std::vector;

namespace Terrain
{

const float SPAN_DEF = 256.f;  // This value is corresponding to the original size of terrain-textures
const float VARIANT_DEF = 10.f;
const float FACTOR_DEF = 1.f;
const unsigned int DEF_TERRAIN_COLOR = 0xFF0000FF;

class DLLAPI TerrainTreeDesc
{
public:
	Common::String m_TTDFilename;  // TTD filename for opening the file which stores infos we need; 扩展名暂定为.ttd，意为Terrain Tree Description

	unsigned int m_FNs;  // 每行所包含的FieldNode的数目，必须是2的n次方，n >= 0。m_FNs的值最大为256
	unsigned int m_QLs;  // 每个FieldNode在横向(或纵向)上所包含的QuadLeaf的数目，必须是2的n次方，n >= 0。m_QLs的值最大为64(这是可视判断算法的最佳表现)
	float m_XSpan;  // 每个BlockNode叶结点在X轴上的跨度(in World Space)
	float m_ZSpan;  // 每个BlockNode叶结点在Z轴上的跨度(in World Space)
	float m_Variant;  // 高度容差
	float m_Factor;  // 高度修正, can NOT be 0!!
	unsigned int m_MaxLOD;  // 最大LOD等级。该数值表示距离镜头最近处地形的二元三角树的树深，值越大，地形精细度越高；但若为0，则忠实再现地形原貌(可用于Debug)
	unsigned int m_TerrainBaseColor;  // 无贴图时的颜色

	typedef vector<Common::String> TextureFilenameVector;
	typedef vector<Common::String> AlphaMapFilenameVector;

	TextureFilenameVector m_Textures;
	AlphaMapFilenameVector m_AlphaMaps;
	Common::String m_LightMapFilename;

public:
	TerrainTreeDesc(): m_TTDFilename(), m_FNs(0), m_QLs(0), m_XSpan(SPAN_DEF), m_ZSpan(SPAN_DEF), m_Variant(0.f), m_Factor(FACTOR_DEF), m_MaxLOD(0), m_Textures(), m_AlphaMaps(), m_LightMapFilename() {}
	TerrainTreeDesc(const Common::String& sTTDFilename): m_TTDFilename(sTTDFilename), m_FNs(0), m_QLs(0), m_XSpan(SPAN_DEF), m_ZSpan(SPAN_DEF), m_Variant(0.f), m_Factor(FACTOR_DEF), m_MaxLOD(0), m_TerrainBaseColor(DEF_TERRAIN_COLOR), m_Textures(), m_AlphaMaps(), m_LightMapFilename() {}
	TerrainTreeDesc(const TerrainTreeDesc& ttd): m_TTDFilename(ttd.m_TTDFilename), m_FNs(ttd.m_FNs), m_QLs(ttd.m_QLs), m_XSpan(ttd.m_XSpan), m_ZSpan(ttd.m_ZSpan), m_Variant(ttd.m_Variant), m_Factor(ttd.m_Factor), m_MaxLOD(ttd.m_MaxLOD), m_TerrainBaseColor(ttd.m_TerrainBaseColor), m_Textures(ttd.m_Textures), m_AlphaMaps(ttd.m_AlphaMaps), m_LightMapFilename(ttd.m_LightMapFilename) {}
	TerrainTreeDesc& operator = (const TerrainTreeDesc& ttd);

	void GetName(Common::String& sName) const { sName = m_TTDFilename; }
	void SetName(const Common::String& sName);

	void ReadFile();
};

}  // namespace

#endif