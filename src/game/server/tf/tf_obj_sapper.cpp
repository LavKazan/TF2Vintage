//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Slowly damages the object it's attached to
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tf_player.h"
#include "tf_team.h"
#include "tf_gamerules.h"
#include "tf_obj.h"
#include "tf_obj_sentrygun.h"
#include "tf_obj_teleporter.h"
#include "tf_obj_sapper.h"
#include "ndebugoverlay.h"
#include "tf_gamestats.h"

// ------------------------------------------------------------------------ //

#define SAPPER_MINS				Vector(0, 0, 0)
#define SAPPER_MAXS				Vector(1, 1, 1)

const char *g_sapperModel = "models/buildables/sapper_placed.mdl";
const char *g_sapperModelPlacement = "models/buildables/sapper_placement.mdl";

const char *g_sapperModelSD = "models/buildables/sd_sapper_placed.mdl";
const char *g_sapperModelPlacementSD = "models/buildables/sd_sapper_placement.mdl";

#define SAPPER_MODEL_SENTRY_1	"models/buildables/sapper_sentry1.mdl"
#define SAPPER_MODEL_SENTRY_2	"models/buildables/sapper_sentry2.mdl"
#define SAPPER_MODEL_SENTRY_3	"models/buildables/sapper_sentry3.mdl"
#define SAPPER_MODEL_TELEPORTER	"models/buildables/sapper_teleporter.mdl"
#define SAPPER_MODEL_DISPENSER	"models/buildables/sapper_dispenser.mdl"

#define SAPPER_MODEL_SENTRY_1_PLACEMENT		"models/buildables/sapper_placement_sentry1.mdl"
#define SAPPER_MODEL_SENTRY_2_PLACEMENT		"models/buildables/sapper_placement_sentry2.mdl"
#define SAPPER_MODEL_SENTRY_3_PLACEMENT		"models/buildables/sapper_placement_sentry3.mdl"
#define SAPPER_MODEL_TELEPORTER_PLACEMENT	"models/buildables/sapper_placement_teleporter.mdl"
#define SAPPER_MODEL_DISPENSER_PLACEMENT	"models/buildables/sapper_placement_dispenser.mdl"

BEGIN_DATADESC( CObjectSapper )
	DEFINE_THINKFUNC( SapperThink ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CObjectSapper, DT_ObjectSapper)
END_SEND_TABLE();

LINK_ENTITY_TO_CLASS(obj_attachment_sapper, CObjectSapper);
PRECACHE_REGISTER(obj_attachment_sapper);

ConVar	obj_sapper_health( "obj_sapper_health", "100", FCVAR_NONE, "Sapper health" );
ConVar	obj_sapper_amount( "obj_sapper_amount", "25", FCVAR_NONE, "Amount of health inflicted by a Sapper object per second" );

#define SAPPER_THINK_CONTEXT		"SapperThink"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectSapper::CObjectSapper()
{
	m_iHealth = GetBaseHealth();
	SetMaxHealth( GetBaseHealth() );

	UseClientSideAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::UpdateOnRemove()
{
	if ( ReverseBuildingConstruction() )
		StopSound( "Weapon_sd_sapper.Timer" );	// Red-Tape Recorder
	else
		StopSound( "Weapon_Sapper.Timer" );		// Standard Sapper


	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::Spawn()
{
	SetModel( GetSapperModelName( SAPPER_MODEL_PLACEMENT ) );

	m_takedamage = DAMAGE_YES;
	m_iHealth = GetBaseHealth();

	SetType( OBJ_ATTACHMENT_SAPPER );

	BaseClass::Spawn();

	Vector mins = SAPPER_MINS;
	Vector maxs = SAPPER_MAXS;
	CollisionProp()->SetSurroundingBoundsType( USE_SPECIFIED_BOUNDS, &mins, &maxs );

    m_fObjectFlags.Set( m_fObjectFlags | OF_ALLOW_REPEAT_PLACEMENT );

	SetSolid( SOLID_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::Precache()
{
	int iModelIndex = PrecacheModel( GetSapperModelName( SAPPER_MODEL_PLACED ) );
	PrecacheGibsForModel( iModelIndex );

	PrecacheModel( GetSapperModelName( SAPPER_MODEL_PLACEMENT ) );
	PrecacheScriptSound( "Weapon_Sapper.Plant" );
	PrecacheScriptSound( "Weapon_Sapper.Timer" );
	PrecacheScriptSound( "Weapon_sd_sapper.Timer" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::FinishedBuilding( void )
{
	BaseClass::FinishedBuilding();

	if ( GetParentObject() )
	{
		GetParentObject()->OnAddSapper();
	}

	EmitSound( "Weapon_Sapper.Plant" );

	// start looping "Weapon_Sapper.Timer", killed when we die
	if ( ReverseBuildingConstruction() )
		EmitSound( "Weapon_sd_sapper.Timer" );	// Red-Tape Recorder
	else
		EmitSound( "Weapon_Sapper.Timer" );		// Standard Sapper

	m_flSapperDamageAccumulator = 0;
	m_flLastThinkTime = gpGlobals->curtime;

	SetContextThink( &CObjectSapper::SapperThink, gpGlobals->curtime + 0.1, SAPPER_THINK_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: Change our model based on the object we are attaching to
//-----------------------------------------------------------------------------
void CObjectSapper::SetupAttachedVersion( void )
{
	CBaseObject *pObject = dynamic_cast<CBaseObject *>( m_hBuiltOnEntity.Get() );

	Assert( pObject );

	if ( !pObject )
	{
		DestroyObject();
		return;
	}

	if ( IsPlacing() )
	{
		SetModel( GetSapperModelName( SAPPER_MODEL_PLACEMENT ) );
	}	

	BaseClass::SetupAttachedVersion();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::OnGoActive( void )
{
	// set new model
	CBaseObject *pObject = dynamic_cast<CBaseObject *>( m_hBuiltOnEntity.Get() );

	Assert( pObject );

	if ( !pObject )
	{
		DestroyObject();
		return;
	}

	SetModel( GetSapperModelName( SAPPER_MODEL_PLACED ) );

	UTIL_SetSize( this, SAPPER_MINS, SAPPER_MAXS );
	SetSolid( SOLID_NONE );

	BaseClass::OnGoActive();
}

const char *CObjectSapper::GetSapperModelName( SapperModel_t iModelType )
{
	// Live gets builder model here for sapper model overrides.

	if ( iModelType == SAPPER_MODEL_PLACEMENT )
	{
		if ( ReverseBuildingConstruction() != 0)
			return g_sapperModelPlacementSD;	
		else
			return g_sapperModelPlacement;
	}

	if ( ReverseBuildingConstruction() != 0)
		return g_sapperModelSD;	
	else
		return g_sapperModel;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::DetachObjectFromObject( void )
{
	if ( GetParentObject() )
	{
		GetParentObject()->OnRemoveSapper();
	}

	BaseClass::DetachObjectFromObject();
}

//-----------------------------------------------------------------------------
// Purpose: Slowly destroy the object I'm attached to
//-----------------------------------------------------------------------------
void CObjectSapper::SapperThink( void )
{
	if ( !GetTeam() )
		return;

	CBaseObject *pObject = GetParentObject();
	if ( !pObject )
	{
		DestroyObject();
		return;
	}

	SetNextThink( gpGlobals->curtime + 0.1, SAPPER_THINK_CONTEXT );

	// Don't bring objects back from the dead
	if ( !pObject->IsAlive() || pObject->IsDying() )
		return;

	// how much damage to give this think?
	float flTimeSinceLastThink = gpGlobals->curtime - m_flLastThinkTime;
	float flDamageToGive = ( flTimeSinceLastThink ) * obj_sapper_amount.GetFloat();
	
	CTFPlayer *pOwner = pObject->GetBuilder();
	if (pOwner)
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER(pOwner, flDamageToGive, mult_sapper_damage);


	// add to accumulator
	m_flSapperDamageAccumulator += flDamageToGive;

	int iDamage = (int)m_flSapperDamageAccumulator;

	m_flSapperDamageAccumulator -= iDamage;

	CTakeDamageInfo info;
	info.SetDamage( iDamage );
	info.SetAttacker( this );
	info.SetInflictor( this );
	info.SetDamageType( DMG_CRUSH );

	pObject->TakeDamage( info );

	m_flLastThinkTime = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectSapper::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( info.GetDamageCustom() != TF_DMG_WRENCH_FIX )
	{
		if ( !info.GetAttacker() )
			return 0;

		int nDamageAppliesToSapper = 0;
		CALL_ATTRIB_HOOK_INT_ON_OTHER( info.GetAttacker(), nDamageAppliesToSapper, set_dmg_apply_to_sapper );
		if( nDamageAppliesToSapper == 0 )
			return 0;
	}

	if( info.GetDamageType() & DMG_CRUSH )
		return BaseClass::OnTakeDamage( info );

	if ( !GetParentObject() )
		return BaseClass::OnTakeDamage( info );

	if( !(info.GetDamageType() & DMG_PLASMA ) )
	{
		CTakeDamageInfo newInfo = info;
		newInfo.AddDamageType( DMG_PLASMA );

		CObjectTeleporter *pTeleporter = dynamic_cast<CObjectTeleporter *>( GetParentObject() );
		if ( pTeleporter )
		{
			CObjectTeleporter *pSibling = pTeleporter->GetMatchingTeleporter();
			if ( pSibling && pSibling->HasSapper() )
			{
				for ( int i=0; i<pSibling->GetNumObjectsOnMe(); ++i )
				{
					CBaseObject *pObject = pSibling->GetBuildPointObject( i );
					if ( !pObject || !pObject->IsHostileUpgrade() )
						continue;

					pObject->TakeDamage( newInfo );
				}
			}
		}
	}

	return BaseClass::OnTakeDamage( info );
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectSapper::Killed( const CTakeDamageInfo &info )
 {
	// If the sapper is removed by someone other than builder, award bonus points.
	CTFPlayer *pScorer = ToTFPlayer( TFGameRules()->GetDeathScorer( info.GetAttacker(), info.GetInflictor(), this ) );
	if (pScorer)
	{
			CBaseObject *pObject = GetParentObject();
			if ( pScorer->GetTeamNumber() != pObject->GetTeamNumber() )
			{
				return;
			}

			// Don't award bonus points if this is a map-placed object
			if ( pObject && pObject->GetBuilder() && pScorer != pObject->GetBuilder() )
			{
				// Bonus points.
				IGameEvent *event_bonus = gameeventmanager->CreateEvent( "player_bonuspoints" );
				if ( event_bonus )
				{
					event_bonus->SetInt( "player_entindex", pObject->GetBuilder()->entindex() );
					event_bonus->SetInt( "source_entindex", pScorer->entindex() );
					event_bonus->SetInt( "points", 1 );

					gameeventmanager->FireEvent( event_bonus );
				}

				CTF_GameStats.Event_PlayerAwardBonusPoints( pScorer, this, 1 );
			}
	}
	
	BaseClass::Killed(info);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectSapper::GetBaseHealth( void )
{
	int iBaseHealth = obj_sapper_health.GetInt();

	CTFPlayer *pPlayer = GetOwner();
	if ( !pPlayer )
		return iBaseHealth;

	CALL_ATTRIB_HOOK_INT_ON_OTHER( pPlayer, iBaseHealth, mult_sapper_health );

	return iBaseHealth;
}

float CObjectSapper::ReverseBuildingConstruction( void )
{
	CTFPlayer *pPlayer = GetOwner();
	if ( !pPlayer )	
		return 0;
	
	float flReverseSpeed = 0;
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pPlayer, flReverseSpeed, sapper_degenerates_buildings );

	return flReverseSpeed;
}

