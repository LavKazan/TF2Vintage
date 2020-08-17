#include "cbase.h"
#include "econ_item_view.h"
#include "econ_item_system.h"
#include "activitylist.h"
#include "attribute_types.h"

#ifdef CLIENT_DLL
#include "dt_utlvector_recv.h"
#else
#include "dt_utlvector_send.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_ATTRIBUTES_SENT 20

#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE( CAttributeList, DT_AttributeList )
	RecvPropUtlVectorDataTable( m_Attributes, MAX_ATTRIBUTES_SENT, DT_EconItemAttribute )
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE( CAttributeList, DT_AttributeList )
	SendPropUtlVectorDataTable( m_Attributes, MAX_ATTRIBUTES_SENT, DT_EconItemAttribute )
END_SEND_TABLE()
#endif

BEGIN_DATADESC_NO_BASE( CAttributeList )
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE( CEconItemView, DT_ScriptCreatedItem )
	RecvPropInt( RECVINFO( m_iItemDefinitionIndex ) ),
	RecvPropInt( RECVINFO( m_iEntityQuality ) ),
	RecvPropInt( RECVINFO( m_iEntityLevel ) ),
	RecvPropInt( RECVINFO( m_iTeamNumber ) ),
	RecvPropBool( RECVINFO( m_bOnlyIterateItemViewAttributes ) ),
	RecvPropDataTable( RECVINFO_DT( m_AttributeList ), 0, &REFERENCE_RECV_TABLE( DT_AttributeList ) ),
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE( CEconItemView, DT_ScriptCreatedItem )
	SendPropInt( SENDINFO( m_iItemDefinitionIndex ) ),
	SendPropInt( SENDINFO( m_iEntityQuality ) ),
	SendPropInt( SENDINFO( m_iEntityLevel ) ),
	SendPropInt( SENDINFO( m_iTeamNumber ) ),
	SendPropBool( SENDINFO( m_bOnlyIterateItemViewAttributes ) ),
	SendPropDataTable( SENDINFO_DT( m_AttributeList ), &REFERENCE_SEND_TABLE( DT_AttributeList ) ),
END_SEND_TABLE()
#endif

#define FIND_ELEMENT(map, key, val)			\
		unsigned int index = map.Find(key);	\
		if (index != map.InvalidIndex())	\
			val = map.Element(index)


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemView::CEconItemView()
{
	Init( -1 );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemView::CEconItemView( CEconItemView const &other )
{
	Init( -1 );
	*this = other;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemView::CEconItemView( int iItemID )
{
	Init( iItemID );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::Init( int iItemID )
{
	SetItemDefIndex( iItemID );
	m_AttributeList.Init();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::SetItemDefIndex( int iItemID )
{
	m_iItemDefinitionIndex = iItemID;
	//m_pItemDef = GetItemSchema()->GetItemDefinition( m_iItemDefinitionIndex );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetItemDefIndex( void ) const
{
	return m_iItemDefinitionIndex;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::SetItemQuality( int nQuality )
{
	m_iEntityQuality = nQuality;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetItemQuality( void ) const
{
	return m_iEntityQuality;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::SetItemLevel( int nLevel )
{
	m_iEntityLevel = nLevel;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetItemLevel( void ) const
{
	return m_iEntityLevel;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemDefinition *CEconItemView::GetStaticData( void ) const
{
	return GetItemSchema()->GetItemDefinition( m_iItemDefinitionIndex );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CEconItemView::GetWorldDisplayModel( int iClass/* = 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();
	const char *pszModelName = NULL;

	if ( pStatic )
	{
		pszModelName = pStatic->model_world;
		

		// Assuming we're using same model for both 1st person and 3rd person view.
		if ( !pszModelName[0] && pStatic->attach_to_hands == 1 )
		{
			pszModelName = pStatic->model_player;
		}
		if (pStatic->model_player_per_class[iClass][0] != '\0' && pStatic->attach_to_hands == 1)
			pszModelName = pStatic->model_player_per_class[iClass];
	}

	return pszModelName;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CEconItemView::GetPlayerDisplayModel( int iClass/* = 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		if ( pStatic->model_player_per_class[iClass][0] != '\0' )
			return pStatic->model_player_per_class[iClass];

		return pStatic->model_player;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char* CEconItemView::GetEntityName()
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->item_class;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CEconItemView::IsCosmetic()
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, "is_cosmetic", result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetAnimationSlot( void )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->anim_slot;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetItemSlot( void )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->item_slot;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
Activity CEconItemView::GetActivityOverride( int iTeamNumber, Activity actOriginalActivity )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		int iOverridenActivity = ACT_INVALID;

		PerTeamVisuals_t *pVisuals = pStatic->GetVisuals( iTeamNumber );
		if( pVisuals )
		{
			FIND_ELEMENT( pVisuals->animation_replacement, actOriginalActivity, iOverridenActivity );
		}

		if ( iOverridenActivity != ACT_INVALID )
			return (Activity)iOverridenActivity;
	}

	return actOriginalActivity;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CEconItemView::GetActivityOverride( int iTeamNumber, const char *name )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		int iOriginalAct = ActivityList_IndexForName( name );
		int iOverridenAct = ACT_INVALID;

		PerTeamVisuals_t *pVisuals = pStatic->GetVisuals( iTeamNumber );
		if( pVisuals )
		{
			FIND_ELEMENT( pVisuals->animation_replacement, iOriginalAct, iOverridenAct );
		}

		if ( iOverridenAct != ACT_INVALID )
			return ActivityList_NameForIndex( iOverridenAct );
	}

	return name;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CEconItemView::GetSoundOverride( int iIndex, int iTeamNum /*= 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		PerTeamVisuals_t *pVisuals = pStatic->GetVisuals( iTeamNum );
		return pVisuals->aWeaponSounds[iIndex];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
unsigned int CEconItemView::GetModifiedRGBValue( bool bAlternate )
{
	static CSchemaAttributeHandle pAttrDef_Paint( "set item tint rgb" );
	static CSchemaAttributeHandle pAttrDef_Paint2( "set item tint rgb 2" );

	float flResult = 0;
	if ( pAttrDef_Paint )
	{
		float flPaintRGB = 0;
		CAttributeIterator_GetSpecificAttribute<unsigned int, float> iter( pAttrDef_Paint, &flPaintRGB );
		IterateAttributes( &iter );
		
		if ( iter.Found() )
		{
			if ( (unsigned int)flPaintRGB == 1 )
				return bAlternate ? 0x005885A2 : 0x00B8383B;

			if ( pAttrDef_Paint2 )
			{
				CAttributeIterator_GetSpecificAttribute<unsigned int, float> iter2( pAttrDef_Paint2, &flResult );
				IterateAttributes( &iter2 );

				if ( !iter2.Found() )
					flResult = flPaintRGB;
			}

			if ( !bAlternate )
				flResult = flPaintRGB;
		}
	}

	return (unsigned int)flResult;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetSkin( int iTeamNum, bool bViewmodel ) const
{
	if (iTeamNum <= TF_TEAM_COUNT)
	{
		//if !GetStaticData->GetVisuals( iTeamNum )->style.IsEmpty()
		PerTeamVisuals_t *pVisuals = GetStaticData()->GetVisuals( iTeamNum );
		if (pVisuals)
			return pVisuals->skin;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CEconItemView::HasCapability( const char* name )
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->capabilities, name, result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CEconItemView::HasTag( const char* name )
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, name, result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CEconItemView::AddAttribute( CEconItemAttribute *pAttribute )
{
	// Make sure this attribute exists.
	CEconAttributeDefinition *pAttribDef = pAttribute->GetStaticData();
	if ( pAttribDef )
		return m_AttributeList.SetRuntimeAttributeValue( pAttribDef, pAttribute->m_flValue );

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::SkipBaseAttributes( bool bSkip )
{
	m_bOnlyIterateItemViewAttributes = bSkip;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::IterateAttributes( IEconAttributeIterator *iter )
{
	m_AttributeList.IterateAttributes( iter );

	CEconItemDefinition *pStatic = GetStaticData();
	if ( pStatic && !m_bOnlyIterateItemViewAttributes )
	{
		pStatic->IterateAttributes( iter );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CEconItemView::GetExtraWearableModel( void ) const
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		// We have an extra wearable
		return pStatic->extra_wearable;
	}

	return "\0";
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemView &CEconItemView::operator=( CEconItemView const &rhs )
{
	m_iItemDefinitionIndex = rhs.m_iItemDefinitionIndex;
	m_iEntityQuality = rhs.m_iEntityQuality;
	m_iEntityLevel = rhs.m_iEntityLevel;
	m_iTeamNumber = rhs.m_iTeamNumber;

	m_AttributeList.CopyFrom( rhs.m_AttributeList );

#ifdef GAME_DLL
	m_bOnlyIterateItemViewAttributes = rhs.m_bOnlyIterateItemViewAttributes;
	m_iClassNumber = rhs.m_iClassNumber;
#endif

	return *this;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CAttributeList::CAttributeList()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CAttributeList::Init( void )
{
	m_Attributes.Purge();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemAttribute const *CAttributeList::GetAttribByID( int iNum )
{
	FOR_EACH_VEC( m_Attributes, i )
	{
		if ( m_Attributes[i].GetStaticData()->index == iNum )
			return &m_Attributes[i];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemAttribute const *CAttributeList::GetAttribByName( char const *szName )
{
	CEconAttributeDefinition *pDefinition = GetItemSchema()->GetAttributeDefinitionByName( szName );

	FOR_EACH_VEC( m_Attributes, i )
	{
		if ( m_Attributes[i].GetStaticData()->index == pDefinition->index )
			return &m_Attributes[i];
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CAttributeList::IterateAttributes( IEconAttributeIterator *iter )
{
	FOR_EACH_VEC( m_Attributes, i )
	{
		CEconAttributeDefinition const *pDefinition = m_Attributes[i].GetStaticData();
		attrib_data_union_t value;
		value.flVal = m_Attributes[i].m_flValue;

		if ( !pDefinition->type->OnIterateAttributeValue( iter, pDefinition, value ) )
			return;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CAttributeList::SetRuntimeAttributeValue( const CEconAttributeDefinition *pDefinition, float flValue )
{
	Assert( pDefinition );
	if ( pDefinition == nullptr )
		return false;

	FOR_EACH_VEC( m_Attributes, i )
	{
		CEconItemAttribute *pAttrib = &m_Attributes[i];
		if ( pAttrib->GetStaticData() == pDefinition )
		{
			pAttrib->m_flValue = flValue;
			m_pManager->OnAttributesChanged();
			return true;
		}
	}

	CEconItemAttribute attrib( pDefinition->index, flValue );
	m_Attributes[ m_Attributes.AddToTail() ] = attrib;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CAttributeList::RemoveAttribute( const CEconAttributeDefinition *pDefinition )
{
	FOR_EACH_VEC( m_Attributes, i )
	{
		if ( m_Attributes[i].GetStaticData() == pDefinition )
		{
			m_Attributes.Remove( i );
			m_pManager->OnAttributesChanged();
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CAttributeList::RemoveAttribByIndex( int iIndex )
{
	if( iIndex == m_Attributes.InvalidIndex() || iIndex > m_Attributes.Count() )
		return false;

	m_Attributes.Remove( iIndex );
	m_pManager->OnAttributesChanged();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CAttributeList::SetManager( CAttributeManager *pManager )
{
	m_pManager = pManager;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CAttributeList::NotifyManagerOfAttributeValueChanges( void )
{
	m_pManager->OnAttributesChanged();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CAttributeList &CAttributeList::operator=( CAttributeList const &rhs )
{
	m_Attributes.RemoveAll();
	m_Attributes = rhs.m_Attributes;
	m_pManager = nullptr;
	return *this;
}
