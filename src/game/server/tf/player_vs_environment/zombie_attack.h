//========= Copyright � Valve LLC, All rights reserved. =======================
//
// Purpose:		
//
// $NoKeywords: $
//=============================================================================
#ifndef TF_ZOMBIE_ATTACK_H
#define TF_ZOMBIE_ATTACK_H
#ifdef _WIN32
#pragma once
#endif

#include "NextBotBehavior.h"
#include "Path/NextBotPathFollow.h"

class CZombieAttack : public Action<CZombie>
{
	DECLARE_CLASS( CZombieAttack, Action<CZombie> )
public:
	virtual const char *GetName( void ) const;

	virtual ActionResult<CZombie> OnStart( CZombie *me, Action<CZombie> *priorAction ) override;
	virtual ActionResult<CZombie> Update( CZombie *me, float dt ) override;

	virtual EventDesiredResult<CZombie> OnContact( CZombie *me, CBaseEntity *other, CGameTrace *result = NULL ) override;
	virtual EventDesiredResult<CZombie> OnStuck( CZombie *me ) override;
	virtual EventDesiredResult<CZombie> OnOtherKilled( CZombie *me, CBaseCombatCharacter *victim, CTakeDamageInfo const &info ) override;

private:
	bool IsPotentiallyChaseable( CZombie *actor, CBaseCombatCharacter *other );
	void SelectVictim( CZombie *actor );
	
	PathFollower m_PathFollower;
	CHandle<CBaseCombatCharacter> m_hTarget;
	CountdownTimer m_attackTimer;
	CountdownTimer m_specialAttackTimer;
	CountdownTimer m_keepTargetDuration;
	CountdownTimer unk4;
};

#endif
