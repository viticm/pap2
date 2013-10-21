#ifndef CASE_GAME_WORLD_K_RL_DOODAD_FRAME_DATA_H
#define CASE_GAME_WORLD_K_RL_DOODAD_FRAME_DATA_H

class KDoodad;

class KRLDoodadFrameData
{
private:
	struct KFrameData
	{
		int	nX;
		int nY;
		int	nZ;
		float fFaceDirection;
	};

public:
	KRLDoodadFrameData();

	void UpdateData();

public:
	KDoodad* m_pDoodad;

	KFrameData	m_Current;
	KFrameData	m_Previous;
};

#endif // CASE_GAME_WORLD_K_RL_DOODAD_FRAME_DATA_H