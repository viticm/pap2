// File: TerrainLOD.cpp
// Desc:

#include"TerrainLOD.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

void TerrainLOD::ReadConfig(const Common::String& sConfigFilename, unsigned int MaxLOD)
{
	m_Ranks.clear();
	m_Levels.clear();

	Common::INIFile inif;
	inif.SetFilename(sConfigFilename);
	inif.Open();

	Common::String sTemp;
	unsigned int NumOfRanks = 0;
	if(inif.GetINI("TerrainLOD", "NumOfRanks", sTemp))
	{
		NumOfRanks = sTemp.IntValue();
	}

	if(NumOfRanks == 0)
	{
		NumOfRanks = DEF_NUM_RANKS;
	}

	unsigned int i = 0, temp = 0;
	for(; i < NumOfRanks; ++i)
	{
		float rank = 0.f;
		if(inif.GetINI("TerrainLOD", Common::String((int)i), sTemp))
		{
			rank = sTemp.FloatValue();
		}
		else
		{
			rank = temp + DEF_RANK_0 * (i + 1);
		}

		m_Ranks.push_back(rank * rank);
		m_Levels.push_back(MaxLOD - (MaxLOD * i) / NumOfRanks);

		temp += DEF_RANK_0 * (i + 1);
	}

	m_Levels[NumOfRanks - 1] = 1;
}

void TerrainLOD::ReadConfig(Common::INIFile& sConfigFile, unsigned int MaxLOD)
{
	m_Ranks.clear();
	m_Levels.clear();

	sConfigFile.Open();

	Common::String sTemp;
	unsigned int NumOfRanks = 0;
	if(sConfigFile.GetINI("TerrainLOD", "NumOfRanks", sTemp))
	{
		NumOfRanks = sTemp.IntValue();
	}

	if(NumOfRanks == 0)
	{
		NumOfRanks = DEF_NUM_RANKS;
	}

	unsigned int i = 0, temp = 0;
	for(; i < NumOfRanks; ++i)
	{
		float rank = 0.f;
		if(sConfigFile.GetINI("TerrainLOD", Common::String((int)i), sTemp))
		{
			rank = sTemp.FloatValue();
		}
		else
		{
			rank = temp + DEF_RANK_0 * (i + 1);
		}

		m_Ranks.push_back(rank * rank);
		m_Levels.push_back(MaxLOD - (MaxLOD * i) / NumOfRanks);

		temp += DEF_RANK_0 * (i + 1);
	}

	m_Levels[NumOfRanks - 1] = 1;
}

unsigned int TerrainLOD::CalculateLevelOfDetail(const Maths::Camera& camera, const Maths::Vector3F& vPoint) const
{
	float SquareDistanceToViewpoint = (vPoint - camera.m_Location).Square();

	unsigned int NumOfRanks = m_Ranks.size();
	if(SquareDistanceToViewpoint > m_Ranks[NumOfRanks - 1])
	{
		return 0;
	}

	for(unsigned int i = 0; i < NumOfRanks; ++i)
	{
		float Rank = m_Ranks[i];
		unsigned int Level = m_Levels[i];

		if(SquareDistanceToViewpoint <= Rank)
		{
			return Level;
		}
	}

	return 0;
}

}  // namespace