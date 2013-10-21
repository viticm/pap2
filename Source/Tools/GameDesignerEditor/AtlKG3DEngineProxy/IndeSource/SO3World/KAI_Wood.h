////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAI_Wood.h
//  Version     : 1.0
//  Creator     : Zhu Chengmin
//  Comment     : 木桩型AI:打不还手,骂不还口,见人不追,挨揍不跑
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KAI_WOOD_
#define _KAI_WOOD_

#include "KAIBase.h"

class KAI_Wood : public KAIBase
{
#ifdef _SERVER
public:
	virtual void Activate(void);
	virtual void OnIdle();
	virtual void OnWander();
	virtual void OnPartol();
	virtual void OnAlert();
	virtual void OnFollow();
	virtual void OnPursuit();
	virtual void OnKeepAway();
	virtual void OnEscape();
	virtual void OnReturn();
	virtual void OnWait();
	virtual void OnAttacked(KCharacter* pAttacker, int nDamage);
	virtual void OnPathResult(BOOL bSucceed);
#endif //_SERVER
};

#endif //_KAI_PLAYER_
