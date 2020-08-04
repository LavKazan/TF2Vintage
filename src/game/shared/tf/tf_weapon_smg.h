//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef TF_WEAPON_SMG_H
#define TF_WEAPON_SMG_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFSMG C_TFSMG
#define CTFSMG_Primary C_TFSMG_Primary
#define CTFSMG_Charged C_TFSMG_Charged
#endif

//=============================================================================
//
// TF Weapon Sub-machine gun.
//
class CTFSMG : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFSMG, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

// Server specific.
//#ifdef GAME_DLL
//	DECLARE_DATADESC();
//#endif

	CTFSMG() {}
	~CTFSMG() {}

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_SMG; }

private:

	CTFSMG( const CTFSMG & ) {}
};

class CTFSMG_Primary : public CTFSMG
{
public:
	DECLARE_CLASS( CTFSMG_Primary, CTFSMG );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
};

class CTFSMG_Charged : public CTFSMG
{
public:
	DECLARE_CLASS( CTFSMG_Charged, CTFSMG );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_CHARGED_SMG; }
	virtual bool	HasChargeBar( void );
	virtual const char* GetEffectLabelText( void )			{ return "#TF_SmgCharge"; }
	virtual float	GetEffectBarProgress( void );
	virtual void	SecondaryAttack( void );
	virtual void	ItemBusyFrame( void );
};

#endif // TF_WEAPON_SMG_H