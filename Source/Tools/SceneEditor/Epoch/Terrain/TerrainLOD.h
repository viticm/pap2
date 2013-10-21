// File: TerrainLOD.h
// Desc:

#ifndef TERRAINLOD_H
#define TERRAINLOD_H

#include"..\Common\Common.h"
#include"..\Maths\Vector3F.h"
#include"..\Maths\Camera.h"

#include<vector>
using std::vector;

namespace Terrain
{

class DLLAPI TerrainLOD  // This should be a singleton
{
public:
	enum
	{
		DEF_NUM_RANKS = 4,
		DEF_RANK_0 = 50,
		DEF_RANK_1 = 150,
		DEF_RANK_2 = 300,
		DEF_RANK_3 = 500
	};

private:
	vector<float> m_Ranks;
	vector<unsigned int> m_Levels;

private:
	TerrainLOD(const TerrainLOD& tlod) {}
	TerrainLOD& operator = (const TerrainLOD& tlod) { return *this; }

public:
	TerrainLOD() {}
	~TerrainLOD() {}

	void ReadConfig(const Common::String& sConfigFilename, unsigned int MaxLOD);
	void ReadConfig(Common::INIFile& sConfigFile, unsigned int MaxLOD);
	unsigned int CalculateLevelOfDetail(const Maths::Camera& camera, const Maths::Vector3F& vPoint) const;
};

}  // namespace

#endif