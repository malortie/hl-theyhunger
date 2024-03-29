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
#include "soundent.h"
#include "gamerules.h"

#define AP9_NUM_BURST_SHOTS		3
#define AP9_RELOAD_SEQUENCE_DURATION	(101.0 / 35.0)
#define AP9_SHOOT_SEQUENCE_DURATION		(23.0 / 22.0)

enum ap9_e
{
	AP9_IDLE = 0,
	AP9_RELOAD,
	AP9_DRAW,
	AP9_SHOOT1,
	AP9_SHOOT2,
	AP9_SHOOT3,
};

LINK_ENTITY_TO_CLASS(weapon_th_ap9, CAP9);

BOOL CAP9::IsBurstFiring() const
{
	return m_fInAttack > 0;
}

void CAP9::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_ap9.mdl");
	m_iId = WEAPON_AP9;

	m_iDefaultAmmo = AP9_DEFAULT_GIVE;

	m_fInAttack = 0;

	FallInit();// get ready to fall down.
}


void CAP9::Precache(void)
{
	PRECACHE_MODEL("models/v_ap9.mdl");
	PRECACHE_MODEL("models/w_ap9.mdl");
	PRECACHE_MODEL("models/p_ap9.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl");// brass shellTE_MODEL

	PRECACHE_MODEL("models/w_ap9clip.mdl");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/ap9_bolt.wav");
	PRECACHE_SOUND("weapons/ap9_clipin.wav");
	PRECACHE_SOUND("weapons/ap9_clipout.wav");
	PRECACHE_SOUND("weapons/ap9_fire.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usFireAP9 = PRECACHE_EVENT(1, "events/ap9.sc");
}

int CAP9::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "ap9";
	p->iMaxAmmo1 = AP9_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = AP9_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 2;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_AP9;
	p->iWeight = AP9_WEIGHT;

	return 1;
}

int CAP9::AddToPlayer(CBasePlayer *pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev);
		WRITE_BYTE(m_iId);
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CAP9::Deploy()
{
	BOOL bResult = DefaultDeploy("models/v_ap9.mdl", "models/p_ap9.mdl", AP9_DRAW, "ap9");

	if (bResult)
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 1.0;
	}

	return bResult;
}

void CAP9::Holster(int skiplocal /*= 0*/)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;

	// Cancel burst firing.
	m_fInAttack = 0;
}

void CAP9::PrimaryAttack(void)
{
	// Do not fire if we are firing in burst.
	if (IsBurstFiring())
		return;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(0.5);
		}
		return;
	}

	AP9Fire(0.07, 0.13, TRUE, FALSE);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + AP9_SHOOT_SEQUENCE_DURATION;
}

void CAP9::SecondaryAttack(void)
{
	// Do not fire if we are firing in burst.
	if (IsBurstFiring())
		return;

	// Do not fire if the clip is empty.
	if (m_iClip <= 0)
		return;

	// Set the attack state to the number of bullets to fire in burst.
	m_fInAttack = std::min(m_iClip, AP9_NUM_BURST_SHOTS);

	// To allow WeaponIdle() to update burst fire now.
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() - 0.1;
}

void CAP9::AP9Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim, BOOL fBurstShot)
{
	// Do not fire if the clip is empty.
	if (m_iClip <= 0)
		return;

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
	vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_AP9, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireAP9, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, fBurstShot, 0);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CAP9::Reload(void)
{
	if (m_pPlayer->ammo_ap9 <= 0)
		return;

	// Do not reload if we are firing in burst.
	if (IsBurstFiring())
		return;

	int iResult = DefaultReload(AP9_MAX_CLIP, AP9_RELOAD, AP9_RELOAD_SEQUENCE_DURATION);

	if (iResult)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + AP9_RELOAD_SEQUENCE_DURATION;
	}
}


void CAP9::WeaponIdle(void)
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Burst fire.
	if (IsBurstFiring())
	{
		AP9Fire(0.04, 0.05, FALSE, TRUE);

		m_fInAttack--;

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(0.5f);

		if (m_fInAttack > 0)
		{
			// Must burst again soon.
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.05;
		}
		else
		{
			// This is the last bullet, so set a longer delay to allow the firing animation to finish.
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + AP9_SHOOT_SEQUENCE_DURATION;
		}
	}
	else
	{
		SendWeaponAnim(AP9_IDLE);

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 11.0 / 12.0; // how long till we do this again.
	}
}


class CAP9Ammo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_ap9clip.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_ap9clip.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo(CBaseEntity *pOther)
	{
		int bResult = (pOther->GiveAmmo(AMMO_AP9_GIVE, "ap9", AP9_MAX_CARRY) != -1);
		if (bResult)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS(ammo_th_ap9, CAP9Ammo);
