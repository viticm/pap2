#ifndef CASE_GAME_WOLRD_K_GAME_WORLD_CHARACTER_CONTROLLER_H
#define CASE_GAME_WOLRD_K_GAME_WORLD_CHARACTER_CONTROLLER_H

class KCharacter;

class KGameWorldCharacterController
{
public:
	KGameWorldCharacterController();

	void Attach(KCharacter* pCharacter);

    void Turn(int nTurn);

	void SetYaw(int nYaw);
	void SetPitch(int nPitch);
	void Move(int nLocalX, int nLocalY, int nLocalZ);

	void Run();
	void Walk();
	void Jump(int nLocalX, int nLocalY);

    int GetMoveInfo(int *pnForward, int *pnStrafeRight, int *pnRotationRight);

private:
	void UpdateMove();

private:
	KCharacter* m_pCharacter;

	int	m_nLocalX;
	int	m_nLocalY;
	int	m_nLocalZ;

    int m_nWalk;
    int m_nTurn;
};

#endif // CASE_GAME_WOLRD_K_GAME_WORLD_CHARACTER_CONTROLLER_H