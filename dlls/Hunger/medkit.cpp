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

#define MEDKIT_DRAW_SEQUENCE_DURATION	(16.0 / 30.0)
#define MEDKIT_HEAL_SEQUENCE_DURATION	(72.0 / 30.0)

enum medkit_e {
	MEDKIT_IDLE = 0,
	MEDKIT_LONGIDLE,
	MEDKIT_LONGUSE,
	MEDKIT_SHORTUSE,
	MEDKIT_HOLSTER,
	MEDKIT_DRAW,
};

LINK_ENTITY_TO_CLASS(weapon_th_medkit, CMedkit);

void CMedkit::Spawn()
{
	Precache();
	m_iId = WEAPON_MEDKIT;
	SET_MODEL(ENT(pev), "models/w_tfc_medkit.mdl");

	m_iDefaultAmmo = MEDKIT_DEFAULT_GIVE;

	m_flSoundDelay = 0;

	FallInit();// get ready to fall down.
}


void CMedkit::Precache(void)
{
	PRECACHE_MODEL("models/v_tfc_medkit.mdl");
	PRECACHE_MODEL("models/w_tfc_medkit.mdl");
	PRECACHE_MODEL("models/p_tfc_medkit.mdl");

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("items/medshot4.wav");
	PRECACHE_SOUND("items/medshotno1.wav");

	m_usMedkit = PRECACHE_EVENT(1, "events/medkit.sc");
}

int CMedkit::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hornets";
	p->iMaxAmmo1 = MEDKIT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 4;
	p->iId = m_iId = WEAPON_MEDKIT;
	p->iWeight = MEDKIT_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}

int CMedkit::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CMedkit::Deploy()
{
	BOOL bResult = DefaultDeploy("models/v_tfc_medkit.mdl", "models/p_tfc_medkit.mdl", MEDKIT_DRAW, "medkit");

	if (bResult)
	{
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + MEDKIT_DRAW_SEQUENCE_DURATION;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
	}

	return bResult;
}

void CMedkit::Holster(int skiplocal /*= 0*/)
{
	// Cancel heal sound.
	m_flSoundDelay = 0;

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(MEDKIT_HOLSTER);
}

void CMedkit::PrimaryAttack(void)
{
	if (m_pPlayer->pev->health >= m_pPlayer->pev->max_health)
		return;

	if (m_pPlayer->ammo_hornets <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = GetNextAttackDelay(1.0);
		return;
	} 
	
	PLAYBACK_EVENT(0, m_pPlayer->edict(), m_usMedkit);

	m_flNextPrimaryAttack = GetNextAttackDelay(MEDKIT_HEAL_SEQUENCE_DURATION);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + MEDKIT_HEAL_SEQUENCE_DURATION;

	m_flSoundDelay = gpGlobals->time + (38.0 / 30.0);
}

void CMedkit::WeaponIdle(void)
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flSoundDelay != 0 && m_flSoundDelay <= gpGlobals->time)
	{
		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		if ( m_pPlayer->TakeHealth(gSkillData.medkitHeal, DMG_GENERIC) )
		{
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
		}

		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/medshot4.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(90, 100));

		m_flSoundDelay = 0;
	}

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	SendWeaponAnim(MEDKIT_IDLE, 1);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 36.0 / 30.0;
}

BOOL CMedkit::PlayEmptySound(void)
{
	if (m_iPlayEmptySound)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "items/medshotno1.wav", 0.8, ATTN_NORM);
		m_iPlayEmptySound = 0;
		return 0;
	}
	return 0;
}
