//========== Copyright � 2008, Valve Corporation, All rights reserved. ========
//
// Purpose:
//
//=============================================================================

#ifndef VSCRIPT_SHARED_H
#define VSCRIPT_SHARED_H

#include "vscript/ivscript.h"

#if defined( _WIN32 )
#pragma once
#endif

// #define VMPROFILE 1

#ifdef VMPROFILE

#define VMPROF_START float debugStartTime = Plat_FloatTime();
#define VMPROF_SHOW( funcname, funcdesc  ) DevMsg("***VSCRIPT PROFILE***: %s %s: %6.4f milliseconds\n", (##funcname), (##funcdesc), (Plat_FloatTime() - debugStartTime)*1000.0 );

#else // !VMPROFILE

#define VMPROF_START
#define VMPROF_SHOW

#endif // VMPROFILE

extern IScriptVM *g_pScriptVM;

const char *VScriptCutDownString( const char *str );
HSCRIPT VScriptCompileScript( const char *pszScriptName, bool bWarnMissing = false );
bool VScriptRunScript( const char *pszScriptName, HSCRIPT hScope, bool bWarnMissing = false );
inline bool VScriptRunScript( const char *pszScriptName, bool bWarnMissing = false ) { return VScriptRunScript( pszScriptName, NULL, bWarnMissing ); }

#define DECLARE_ENT_SCRIPTDESC()													ALLOW_SCRIPT_ACCESS() virtual ScriptClassDesc_t *GetScriptDesc()

#define _IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className )								template <> ScriptClassDesc_t *GetScriptDesc<className>( className * ); ScriptClassDesc_t *className::GetScriptDesc()  { return ::GetScriptDesc( this ); }	

#define BEGIN_ENT_SCRIPTDESC( className, baseClass, description )					_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ) BEGIN_SCRIPTDESC( className, baseClass, description )
#define BEGIN_ENT_SCRIPTDESC_ROOT( className, description )							_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ) BEGIN_SCRIPTDESC_ROOT( className, description )
#define BEGIN_ENT_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )	_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ) BEGIN_SCRIPTDESC_NAMED( className, baseClass, scriptName, description )
#define BEGIN_ENT_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )		_IMPLEMENT_ENT_SCRIPTDESC_ACCESSOR( className ) BEGIN_SCRIPTDESC_ROOT_NAMED( className, scriptName, description )	

// Only allow scripts to create entities during map initialization
bool IsEntityCreationAllowedInScripts( void );

#endif // VSCRIPT_SHARED_H
