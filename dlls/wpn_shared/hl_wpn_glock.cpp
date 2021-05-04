/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"

#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
#define GLOCK_MODEL_DEFAULT		"models/w_9mmhandgun.mdl"
#define GLOCK_MODEL_SILENCER	"models/w_silencer.mdl"

#define SILENCER_GROUP	2
#define SILENCER_OFF	0
#define SILENCER_ON		1
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
enum glock_e {
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );
LINK_ENTITY_TO_CLASS( weapon_9mmhandgun, CGlock );

int CGlock::GetViewModelBody(void) const
{
	return pev->body;
}

void CGlock::UpdateViewModelBody(void)
{
	if (m_fSilencerOn)
		pev->body = SILENCER_ON;
	else
		pev->body = SILENCER_OFF;
}

void CGlock::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_9mmhandgun"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_GLOCK;
#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	char* szModel = NULL;

	if (pev->body == SILENCER_ON)
	{
		szModel = GLOCK_MODEL_SILENCER;
		m_fSilencerOn = TRUE;
	}
	else
	{
		szModel = GLOCK_MODEL_DEFAULT;
		m_fSilencerOn = FALSE;
	}

	SET_MODEL(ENT(pev), szModel);

#else
	SET_MODEL(ENT(pev), "models/w_9mmhandgun.mdl");
#endif //  defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CGlock::Precache( void )
{
	PRECACHE_MODEL("models/v_9mmhandgun.mdl");
	PRECACHE_MODEL("models/w_9mmhandgun.mdl");
#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	PRECACHE_MODEL("models/w_silencer.mdl");
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	PRECACHE_MODEL("models/p_9mmhandgun.mdl");

	m_iShell = PRECACHE_MODEL ("models/shell.mdl");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND ("weapons/pl_gun1.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun2.wav");//silenced handgun
	PRECACHE_SOUND ("weapons/pl_gun3.wav");//handgun

	m_usFireGlock1 = PRECACHE_EVENT( 1, "events/glock1.sc" );
	m_usFireGlock2 = PRECACHE_EVENT( 1, "events/glock2.sc" );
}

int CGlock::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = GLOCK_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_GLOCK;
	p->iWeight = GLOCK_WEIGHT;

	return 1;
}

int CGlock::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CGlock::Deploy( )
{
	UpdateViewModelBody();

	return DefaultDeploy("models/v_9mmhandgun.mdl", "models/p_9mmhandgun.mdl", GLOCK_DRAW, "onehanded", UseDecrement(), GetViewModelBody());
}

#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
void CGlock::Holster(int skiplocal /*= 0*/)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(GLOCK_HOLSTER);
	SendWeaponAnim(GLOCK_HOLSTER, UseDecrement(), GetViewModelBody());

	m_fInAttack = 0;
}

BOOL CGlock::ShouldWeaponIdle(void)
{
	return m_fInAttack != 0;
}
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
void CGlock::SecondaryAttack( void )
{
#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	if (m_fInAttack != 0)
		return;

	if (!m_fSilencerOn)
	{
		// Add silencer.
		m_fInAttack = 1;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;

		SendWeaponAnim(GLOCK_HOLSTER, 1, SILENCER_OFF);
		SendWeaponAnim(GLOCK_HOLSTER, UseDecrement(), GetViewModelBody());

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(2.0);
	}
	else
	{
		// Remove silencer.
		m_fInAttack = 2;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.4; // 3.3

		SendWeaponAnim( GLOCK_ADD_SILENCER, 1, SILENCER_ON);
		SendWeaponAnim( GLOCK_ADD_SILENCER, UseDecrement(), GetViewModelBody());

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(3.4);
	}
#else
	GlockFire( 0.1, 0.2, FALSE );
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
}

void CGlock::PrimaryAttack( void )
{
#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	if (m_fInAttack != 0)
		return;

	if ( m_fSilencerOn )
	{
		GlockFire(0.01, 0.3, TRUE);
	}
	else
	{
		GlockFire(0.1, 0.2, FALSE);
	}
#else
	GlockFire( 0.01, 0.3, TRUE );
#endif
}

void CGlock::GlockFire( float flSpread , float flCycleTime, BOOL fUseAutoAim )
{
	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.2);
		}

		return;
	}

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	int flags;

#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	// silenced
	if (pev->body == 1)
	if (m_fSilencerOn)
	{
		m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;
	}
	else
	{
		// non-silenced
		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	}

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	
	if ( fUseAutoAim )
	{
		vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_9MM, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireGlock1, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, GetViewModelBody(), 0, (m_iClip == 0) ? 1 : 0, m_fSilencerOn);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}


void CGlock::Reload( void )
{
#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	if ( m_pPlayer->ammo_9mm <= 0 )
		return;

	int body = m_fSilencerOn ? SILENCER_ON : SILENCER_OFF;
	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload(GLOCK_MAX_CLIP, GLOCK_RELOAD, 1.5, body);
		iResult = DefaultReload(GLOCK_MAX_CLIP, GLOCK_RELOAD, 1.5, GetViewModelBody());
	else
		iResult = DefaultReload(GLOCK_MAX_CLIP, GLOCK_RELOAD_NOT_EMPTY, 1.5, body);
		iResult = DefaultReload(GLOCK_MAX_CLIP, GLOCK_RELOAD_NOT_EMPTY, 1.5, GetViewModelBody());

	if (iResult)
	{
		m_fInAttack = 0;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
#else
	if ( m_pPlayer->ammo_9mm <= 0 )
		 return;

	int iResult;

	if (m_iClip == 0)
		iResult = DefaultReload( 17, GLOCK_RELOAD, 1.5 );
	else
		iResult = DefaultReload( 17, GLOCK_RELOAD_NOT_EMPTY, 1.5 );

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
}



void CGlock::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );


	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

#if defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
	if (m_fInAttack != 0)
	{
		if (m_fInAttack == 1)
		{
			m_fSilencerOn = TRUE;
		}
		else if (m_fInAttack == 2)
		{
			m_fSilencerOn = FALSE;
		}

		int body = m_fSilencerOn ? SILENCER_ON : SILENCER_OFF;
		UpdateViewModelBody();

		SendWeaponAnim(GLOCK_DRAW, UseDecrement(), body);
		SendWeaponAnim(GLOCK_DRAW, UseDecrement(), GetViewModelBody());

		m_fInAttack = 0;

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	}
	else
	{
		// only idle if the slid isn't back
		if (m_iClip != 0)
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

			if (flRand <= 0.3 + 0 * 0.75)
			{
				iAnim = GLOCK_IDLE3;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
			}
			else if (flRand <= 0.6 + 0 * 0.875)
			{
				iAnim = GLOCK_IDLE1;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
			}
			else
			{
				iAnim = GLOCK_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
			}

			SendWeaponAnim(iAnim, 1, m_fSilencerOn ? 1 : 0);
	}
#else
	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		int iAnim;
		float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0.0, 1.0 );

		if (flRand <= 0.3 + 0 * 0.75)
		{
			iAnim = GLOCK_IDLE3;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 49.0 / 16;
		}
		else if (flRand <= 0.6 + 0 * 0.875)
		{
			iAnim = GLOCK_IDLE1;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;
		}
		else
		{
			iAnim = GLOCK_IDLE2;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 40.0 / 16.0;
		}
		SendWeaponAnim( iAnim, 1 );
	}
#endif // defined ( HUNGER_DLL ) || defined ( HUNGER_CLIENT_DLL )
}








class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_9mmclip.mdl");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_9mmclip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_glockclip, CGlockAmmo );
LINK_ENTITY_TO_CLASS( ammo_9mmclip, CGlockAmmo );















