/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include <inttypes.h>

#include "bg_public.h"
#include "g_local.h"
#include "game/bg_public.h"
#include "qcommon/game_version.h"

//
// Cvar callbacks
//

//[JAPRO - Serverside - jcinfo update]
static void CVU_Flipkick(void) {
	if (g_flipKick.integer > 2) {//3
		jcinfo.integer |= JAPRO_CINFO_FLIPKICK;
		jcinfo.integer |= JAPRO_CINFO_FIXSIDEKICK;
	}
	else if (g_flipKick.integer == 2) {//1
		jcinfo.integer |= JAPRO_CINFO_FLIPKICK;
		jcinfo.integer &= ~JAPRO_CINFO_FIXSIDEKICK;
	}
	else if (g_flipKick.integer == 1) {//1
		jcinfo.integer |= JAPRO_CINFO_FLIPKICK;
		jcinfo.integer &= ~JAPRO_CINFO_FIXSIDEKICK;
	}
	else {//0
		jcinfo.integer &= ~JAPRO_CINFO_FLIPKICK;
		jcinfo.integer &= ~JAPRO_CINFO_FIXSIDEKICK;
	}
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_Roll(void) {
	if (g_fixRoll.integer > 2) {
		jcinfo.integer |= JAPRO_CINFO_FIXROLL3;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL2;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL1;
	}
	else if (g_fixRoll.integer == 2) {
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL3;
		jcinfo.integer |= JAPRO_CINFO_FIXROLL2;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL1;
	}
	else if (g_fixRoll.integer == 1) {
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL3;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL2;
		jcinfo.integer |= JAPRO_CINFO_FIXROLL1;
	}
	else {
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL3;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL2;
		jcinfo.integer &= ~JAPRO_CINFO_FIXROLL1;
	}
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

/*
static void CVU_YDFA(void) {
	g_tweakYellowDFA.integer ?
		(jcinfo.integer |= JAPRO_CINFO_YELLOWDFA) : (jcinfo.integer &= ~JAPRO_CINFO_YELLOWDFA);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}
*/

static void CVU_Headslide(void) {
	g_slideOnPlayer.integer ?
		(jcinfo.integer |= JAPRO_CINFO_HEADSLIDE) : (jcinfo.integer &= ~JAPRO_CINFO_HEADSLIDE);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_PlayerCollision(void) {
	g_fixPlayerCollision.integer ?
		(jcinfo2.integer |= JAPRO_CINFO2_FIXPLAYERCOLLISION) : (jcinfo2.integer &= ~JAPRO_CINFO2_FIXPLAYERCOLLISION);
	trap->Cvar_Set("jcinfo2", va("%i", jcinfo2.integer));
}

static void CVU_Unlagged(void) {
	if (g_unlagged.integer & UNLAGGED_PROJ_NUDGE)//if 1 or 3 or 7
		jcinfo.integer |= JAPRO_CINFO_UNLAGGEDPROJ;
	else
		jcinfo.integer &= ~JAPRO_CINFO_UNLAGGEDPROJ;
	if (g_unlagged.integer & UNLAGGED_HITSCAN)//if 2 or 6?
		jcinfo.integer |= JAPRO_CINFO_UNLAGGEDHITSCAN;
	else
		jcinfo.integer &= ~JAPRO_CINFO_UNLAGGEDHITSCAN;
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_Bhop(void) {
	if (g_onlyBhop.integer > 1) {
		jcinfo.integer &= ~JAPRO_CINFO_BHOP1;
		jcinfo.integer |= JAPRO_CINFO_BHOP2;
	}
	else if (g_onlyBhop.integer == 1) {
		jcinfo.integer |= JAPRO_CINFO_BHOP1;
		jcinfo.integer &= ~JAPRO_CINFO_BHOP2;
	}
	else {
		jcinfo.integer &= ~JAPRO_CINFO_BHOP1;
		jcinfo.integer &= ~JAPRO_CINFO_BHOP2;
	}
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

/*
static void CVU_FastGrip(void) {
	g_fastGrip.integer ?
		(jcinfo.integer |= JAPRO_CINFO_FASTGRIP) : (jcinfo.integer &= ~JAPRO_CINFO_FASTGRIP);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}


static void CVU_SpinBackslash(void) {
	g_spinBackslash.integer ?
		(jcinfo.integer |= JAPRO_CINFO_BACKSLASH) : (jcinfo.integer &= ~JAPRO_CINFO_BACKSLASH);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_EasyBackslash(void) {
	g_easyBackslash.integer ?
		(jcinfo.integer |= JAPRO_CINFO_EASYBACKSLASH) : (jcinfo.integer &= ~JAPRO_CINFO_EASYBACKSLASH);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_SpinRDFA(void) {
	g_spinRedDFA.integer ?
		(jcinfo.integer |= JAPRO_CINFO_REDDFA) : (jcinfo.integer &= ~JAPRO_CINFO_REDDFA);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}
*/

static void CVU_TweakJetpack(void) {
	g_tweakJetpack.integer ?
		(jcinfo.integer |= JAPRO_CINFO_JETPACK) : (jcinfo.integer &= ~JAPRO_CINFO_JETPACK);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_ScreenShake(void) {
	g_screenShake.integer ?
		(jcinfo.integer |= JAPRO_CINFO_SCREENSHAKE) : (jcinfo.integer &= ~JAPRO_CINFO_SCREENSHAKE);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

void CVU_TweakWeapons(void) {
	(g_tweakWeapons.integer & WT_PSEUDORANDOM_FIRE) ?
		(jcinfo.integer |= JAPRO_CINFO_PSEUDORANDOM_FIRE) : (jcinfo.integer &= ~JAPRO_CINFO_PSEUDORANDOM_FIRE);
	(g_tweakWeapons.integer & WT_STUN_LG) ?
		(jcinfo.integer |= JAPRO_CINFO_LG) : (jcinfo.integer &= ~JAPRO_CINFO_LG);
	(g_tweakWeapons.integer & WT_STUN_SHOCKLANCE) ?
		(jcinfo.integer |= JAPRO_CINFO_SHOCKLANCE) : (jcinfo.integer &= ~JAPRO_CINFO_SHOCKLANCE);
	(g_tweakWeapons.integer & WT_ALLOW_GUNROLL) ?
		(jcinfo.integer |= JAPRO_CINFO_GUNROLL) : (jcinfo.integer &= ~JAPRO_CINFO_GUNROLL);
	(g_tweakWeapons.integer & WT_PROJ_SNIPER) ?
		(jcinfo.integer |= JAPRO_CINFO_PROJSNIPER) : (jcinfo.integer &= ~JAPRO_CINFO_PROJSNIPER);
	(g_tweakWeapons.integer & WT_TRIBES) ?
		(jcinfo2.integer |= JAPRO_CINFO2_WTTRIBES) : (jcinfo2.integer &= ~JAPRO_CINFO2_WTTRIBES);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
	trap->Cvar_Set("jcinfo2", va("%i", jcinfo2.integer));
}

void CVU_TweakSaber(void) {
	(g_tweakSaber.integer & ST_ALLOW_ROLLCANCEL) ?
		(jcinfo.integer |= JAPRO_CINFO_ROLLCANCEL) : (jcinfo.integer &= ~JAPRO_CINFO_ROLLCANCEL);
	(g_tweakSaber.integer & ST_NO_REDCHAIN) ?
		(jcinfo.integer |= JAPRO_CINFO_NOREDCHAIN) : (jcinfo.integer &= ~JAPRO_CINFO_NOREDCHAIN);
	(g_tweakSaber.integer & ST_SPINREDDFA) ?
		(jcinfo.integer |= JAPRO_CINFO_REDDFA) : (jcinfo.integer &= ~JAPRO_CINFO_REDDFA);
	(g_tweakSaber.integer & ST_EASYBACKSLASH) ?
		(jcinfo.integer |= JAPRO_CINFO_EASYBACKSLASH) : (jcinfo.integer &= ~JAPRO_CINFO_EASYBACKSLASH);
	(g_tweakSaber.integer & ST_SPINBACKSLASH) ?
		(jcinfo.integer |= JAPRO_CINFO_BACKSLASH) : (jcinfo.integer &= ~JAPRO_CINFO_BACKSLASH);
	(g_tweakSaber.integer & ST_FIXYELLOWDFA) ?
		(jcinfo.integer |= JAPRO_CINFO_YELLOWDFA) : (jcinfo.integer &= ~JAPRO_CINFO_YELLOWDFA);
	(g_tweakSaber.integer & ST_JK2LUNGE) ?
		(jcinfo.integer |= JAPRO_CINFO_JK2LUNGE) : (jcinfo.integer &= ~JAPRO_CINFO_JK2LUNGE);
	(g_tweakSaber.integer & ST_JK2RDFA) ?
		(jcinfo.integer |= JAPRO_CINFO_JK2DFA) : (jcinfo.integer &= ~JAPRO_CINFO_JK2DFA);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

void CVU_TweakForce(void) {
	(g_tweakForce.integer & FT_FORCECOMBO) ?
		(jcinfo.integer |= JAPRO_CINFO_FORCECOMBO) : (jcinfo.integer &= ~JAPRO_CINFO_FORCECOMBO);
	(g_tweakForce.integer & FT_FASTGRIP) ?
		(jcinfo.integer |= JAPRO_CINFO_FASTGRIP) : (jcinfo.integer &= ~JAPRO_CINFO_FASTGRIP);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));

	//have to do this on startup somehow?
	if (g_tweakForce.integer & FT_FIXLINEDRAIN) {
		AddRemap("gfx/effects/redLine", "gfx/effects/drainedadd", level.time * 0.001f);
		trap->SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}
	else {
		AddRemap("gfx/effects/drainedadd", "gfx/effects/redLine", level.time * 0.001f);
		trap->SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}
}

static void CVU_LegDangle(void) {
	!g_LegDangle.integer ?
		(jcinfo.integer |= JAPRO_CINFO_NOLEGDANGLE) : (jcinfo.integer &= ~JAPRO_CINFO_NOLEGDANGLE);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_Jawarun(void) {
	(g_emotesDisable.integer & (1 << E_JAWARUN)) ?
		(jcinfo.integer |= JAPRO_CINFO_NOJAWARUN) : (jcinfo.integer &= ~JAPRO_CINFO_NOJAWARUN);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_HighFPS(void) {
	g_fixHighFPSAbuse.integer ?
		(jcinfo.integer |= JAPRO_CINFO_HIGHFPSFIX) : (jcinfo.integer &= ~JAPRO_CINFO_HIGHFPSFIX);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

static void CVU_Grapple(void) {
	g_allowGrapple.integer > 1 ?
		(jcinfo.integer |= JAPRO_CINFO_JAPLUSGRAPPLE) : (jcinfo.integer &= ~JAPRO_CINFO_JAPLUSGRAPPLE);
	trap->Cvar_Set("jcinfo", va("%i", jcinfo.integer));
}

/*
static void CVU_Grapple(void) {
		if ( sscanf( g_grapple.string, "%d %d %d %d %d %d", &grapple.enable, &grapple.tarzanStyle, &grapple.hookspeed, &grapple.pullspeed, &grapple.strength, &grapple.flood ) != 6 ) {
			//Defaults
			grapple.enable = qfalse;
			grapple.tarzanStyle = qfalse;
			grapple.hookspeed = 2400;
			grapple.pullspeed = 800;
			grapple.strength = 20;
			grapple.flood = 500;
		}
}
*/

void RemoveRabbit(void) {
	int i;
	gclient_t	*cl;
	gentity_t	*ent;

	Team_ReturnFlag(TEAM_FREE);

	for (i=0;  i<level.numPlayingClients; i++) {
		cl = &level.clients[level.sortedClients[i]];

		cl->ps.powerups[PW_NEUTRALFLAG] = 0;
	}

	for (i = 0; i < level.num_entities; i++) {
		ent = &g_entities[i];
		if (ent->inuse && (ent->s.eType == ET_ITEM) && (ent->item->giTag == PW_NEUTRALFLAG) && (ent->item->giType == IT_TEAM)) {
			G_FreeEntity( ent );
			return;
		}
	}
}

static void RemoveRedBlueFlags(void) {
	int i;
	gclient_t	*cl;
	gentity_t	*ent;
	qboolean flagsRemoved = 0;

	Team_ReturnFlag(TEAM_RED);
	Team_ReturnFlag(TEAM_BLUE);

	for (i = 0; i<level.numPlayingClients; i++) {
		cl = &level.clients[level.sortedClients[i]];

		cl->ps.powerups[PW_REDFLAG] = 0;
		cl->ps.powerups[PW_BLUEFLAG] = 0;
	}

	for (i = 0; i < level.num_entities; i++) {
		ent = &g_entities[i];
		if (ent->inuse && (ent->s.eType == ET_ITEM) && ((ent->item->giTag == PW_REDFLAG) || (ent->item->giTag == PW_BLUEFLAG)) && (ent->item->giType == IT_TEAM)) {
			G_FreeEntity(ent);
			flagsRemoved++;
		}
		if (flagsRemoved > 1)
			return;
	}
}

qboolean G_FreeAmmoEntity( gitem_t *item );
static void RemoveWeaponsFromMap(void) {
	int i;
	gentity_t	*ent;
	int wDisable = g_weaponDisable.integer;

	for (i = 0; i < level.num_entities; i++) {
		ent = &g_entities[i];

		if (ent->inuse && ent->item) {
			if ((ent->item->giType == IT_WEAPON) && wDisable & (1 << ent->item->giTag)) {
				ent->think = 0;
				ent->nextthink = 0;
				ent->s.eFlags |= EF_NODRAW;
				//ent->s.eFlags |= EF_DROPPEDWEAPON; //sad hack
				ent->r.svFlags |= SVF_NOCLIENT;
				ent->r.contents = 0;
				//ent->inuse = qfalse;
			}
			else if ((ent->item->giType == IT_AMMO) && wDisable && G_FreeAmmoEntity(ent->item)) {
				ent->think = 0;
				ent->nextthink = 0;
				ent->s.eFlags |= EF_NODRAW;
				//ent->s.eFlags |= EF_DROPPEDWEAPON; //sad hack
				ent->r.svFlags |= SVF_NOCLIENT;
				ent->r.contents = 0;
				//ent->inuse = qfalse;
			}
		}
	}
}

static void SpawnWeaponsInMap(void) {
	int i;
	gentity_t	*ent;
	int wDisable = g_weaponDisable.integer;

	for (i = 0; i < level.num_entities; i++) {
		ent = &g_entities[i];

		if (ent->inuse && ent->item) { //eh?
			if ((ent->item->giType == IT_WEAPON) && !(wDisable & (1 << ent->item->giTag))) {
				ent->think = FinishSpawningItem;
				ent->nextthink = level.time + FRAMETIME * 2;
				ent->s.eFlags &= ~EF_NODRAW;
				//ent->s.eFlags &= ~EF_DROPPEDWEAPON; //sad hack
				ent->r.svFlags &= ~SVF_NOCLIENT;
				//ent->r.contents = CONTENTS_TRIGGER;
				//ent->inuse = qtrue;
				//trap->Print("spawning wep\n");
				//ent->spawnedBefore = qfalse;
			}
			else if (ent->item->giType == IT_AMMO && !(wDisable && G_FreeAmmoEntity(ent->item))) {
				ent->think = FinishSpawningItem;
				ent->nextthink = level.time + FRAMETIME * 2;
				ent->s.eFlags &= ~EF_NODRAW;
				//ent->s.eFlags &= ~EF_DROPPEDWEAPON; //sad hack
				ent->r.svFlags &= ~SVF_NOCLIENT;
				//ent->r.contents = CONTENTS_TRIGGER;
				//ent->inuse = qtrue;
			}
		}
	}
}

static void RemoveWeaponsFromPlayer(gentity_t *ent) {
	int disallowedWeaps = g_weaponDisable.integer & ~g_startingWeapons.integer;

	ent->client->ps.stats[STAT_WEAPONS] &= ~disallowedWeaps; //Subtract disallowed weapons from current weapons.

	if (ent->client->ps.stats[STAT_WEAPONS] <= 0)
		ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_MELEE); //HMM? asdf

	if (!(ent->client->ps.stats[STAT_WEAPONS] & (1 >> ent->client->ps.weapon))) { //If our weapon selected does not appear in our weapons list
		ent->client->ps.weapon = WP_MELEE; //who knows why this does shit even if our current weapon is fine.
	}
}

void CVU_WeaponDisable( void ) {
	int i;
	gentity_t *ent;

	for (i=0 ; i < level.numConnectedClients ; i++) { //For each player, call removeweapons, and addweapons.
		ent = &g_entities[level.sortedClients[i]];
		if (ent->inuse && ent->client && !ent->client->sess.raceMode) {
			RemoveWeaponsFromPlayer(ent);
		}
	}
	RemoveWeaponsFromMap();
	SpawnWeaponsInMap();
}

void CVU_ForceDisable( void ) {
	int i;
	gentity_t *ent;

	for (i=0 ; i < level.numConnectedClients ; i++) {
		ent = &g_entities[level.sortedClients[i]];
		if (ent->inuse && ent->client && !ent->client->sess.raceMode) {
			WP_InitForcePowers( ent );
		}
	}
}

void GiveClientWeapons(gclient_t *client);
void CVU_StartingWeapons( void ) {
	int i;
	gentity_t *ent;

	for (i=0 ; i < level.numConnectedClients ; i++) { //For each player, call removeweapons, and addweapons.
		ent = &g_entities[level.sortedClients[i]];
		if (ent->inuse && ent->client && !ent->client->sess.raceMode) {
			RemoveWeaponsFromPlayer(ent);
			GiveClientWeapons(ent->client);
		}
	}
}

//Need to do all this shit again for items, but lazy.
void GiveClientItems(gclient_t *client);
void CVU_StartingItems( void ) {
	int i;
	gentity_t *ent;

	for (i=0 ; i < level.numConnectedClients ; i++) { //For each player, call removeweapons, and addweapons.
		ent = &g_entities[level.sortedClients[i]];
		if (ent->inuse && ent->client && !ent->client->sess.raceMode) {
			//RemoveItemsFromPlayer(ent);
			//Problem here is that thers no g_itemDisable cmd, so we have to parse all the other item individual disable cmds...
			//Maybe just let them keep their old items until they die, oh well.
			GiveClientItems(ent->client);
		}
	}
}

void CVU_GunGame(void) {
	int i;
	gentity_t* ent;

	for (i = 0; i < level.numConnectedClients; i++) { //For each player, call removeweapons, and addweapons.
		ent = &g_entities[level.sortedClients[i]];
		if (ent->inuse && ent->client && !ent->client->sess.raceMode) {
			//RemoveItemsFromPlayer(ent);
			//Problem here is that thers no g_itemDisable cmd, so we have to parse all the other item individual disable cmds...
			//Maybe just let them keep their old items until they die, oh well.
			GiveClientWeapons(ent->client);
		}
	}
}

qboolean G_CallSpawn( gentity_t *ent );
void CVU_Rabbit( void ) {
	//1 = Normal rabbit
	//2 = Sniper Rabbit
	//3 = Rabbit where you gain points for holding it
	//4 = one flag CTF (our base to score)
	//5 = one flag CTF (their base to score)
	//if (g_neutralFlag.integer) { //
		gentity_t	*ent;

		if ((g_neutralFlag.integer != 1 && g_neutralFlag.integer != 2 && g_neutralFlag.integer != 3) && (level.gametype == GT_FFA || level.gametype == GT_TEAM)) {
			RemoveRabbit();
		}
		else if (((g_neutralFlag.integer == 1 || g_neutralFlag.integer == 2 || g_neutralFlag.integer == 3) && (level.gametype == GT_FFA || level.gametype == GT_TEAM))) {
			//Remove and spawn neutral flag
			RemoveRabbit(); //Delete the current flag first
			ent = G_Spawn(qtrue);

			ent->classname = "team_CTF_neutralflag";
			VectorCopy(level.neutralFlagOrigin, ent->s.origin);

			if (!G_CallSpawn(ent))
				G_FreeEntity( ent );
		}
		else if (((g_neutralFlag.integer == 4 || g_neutralFlag.integer == 5 || g_neutralFlag.integer == 6) && (level.gametype == GT_CTF))) { //One Flag CTF
			//Remove and spawn neutral flag.  Remove CTF flags

			RemoveRabbit(); //Delete the current flag first
			RemoveRedBlueFlags();

			ent = G_Spawn(qtrue);
			ent->classname = "team_CTF_neutralflag";
			VectorCopy(level.neutralFlagOrigin, ent->s.origin);

			if (!G_CallSpawn(ent))
				G_FreeEntity(ent);
		}
		else if (((g_neutralFlag.integer != 4 && g_neutralFlag.integer != 5 && g_neutralFlag.integer != 6) && (level.gametype == GT_CTF))) { //turned off neutral flag in standard CTF gametype
			//Remove neutral flag.  Remove and add CTF flags.
			RemoveRabbit(); //Delete the current flag first
			RemoveRedBlueFlags();

			ent = G_Spawn(qtrue);
			ent->classname = "team_CTF_redflag";
			VectorCopy(level.redFlagOrigin, ent->s.origin);

			if (!G_CallSpawn(ent))
				G_FreeEntity(ent);

			ent = G_Spawn(qtrue);
			ent->classname = "team_CTF_blueflag";
			VectorCopy(level.blueFlagOrigin, ent->s.origin);

			if (!G_CallSpawn(ent))
				G_FreeEntity(ent);
		}

	//}
	//else {
		//RemoveRabbit();
	//}
}

static void CVU_RaceMode(void) {
	g_raceMode.integer ? //1 is forced, 2 allows /race cmd
		(jcinfo2.integer |= JAPRO_CINFO2_RACEMODE) : (jcinfo2.integer &= ~JAPRO_CINFO2_RACEMODE);
	trap->Cvar_Set("jcinfo2", va("%i", jcinfo2.integer));
}

static void CVU_SaberSwitch(void) {
	g_allowSaberSwitch.integer ?
		(jcinfo2.integer |= JAPRO_CINFO2_SABERSWITCH) : (jcinfo2.integer &= ~JAPRO_CINFO2_SABERSWITCH);
	trap->Cvar_Set("jcinfo2", va("%i", jcinfo2.integer));
}

static void CVU_ShowHealth(void) {
	int i, health = 0;
	if (g_showHealth.integer)
		health = 100;

	for (i=0; i<MAX_CLIENTS; i++) {
		if (!g_entities[i].inuse)
			continue;
		if (!g_entities[i].client)
			continue;
		if (g_entities[i].client->sess.raceMode)
			continue;
		g_entities[i].maxHealth = g_entities[i].s.maxhealth = g_entities[i].s.health = health;
		//Com_Printf("Setting max health for him %s %i\n", g_entities[i].client->pers.netname, health);
	}
}

static void CVU_TribesClass(void) {
	int i;
	char model[MAX_QPATH] = { 0 }, userinfo[MAX_INFO_STRING] = { 0 };
	gentity_t	*ent;

	for (i = 0; i<MAX_CLIENTS; i++) {
		ent = &g_entities[i];
		if (!ent->inuse)
			continue;
		if (!ent || !ent->client)
			continue;
		if (ent->client->sess.raceMode)
			continue;
		if (ent->client->sess.sessionTeam == TEAM_SPECTATOR)
			continue;

		trap->GetUserinfo(i, userinfo, sizeof(userinfo));
		Q_strncpyz(model, Info_ValueForKey(userinfo, "model"), sizeof(model));

		if (g_tribesMode.integer == 1) {
			if (!ent->client->pers.tribesClass) {
				if (ent->health > 0)
					G_Kill(ent);
			}
			DetectTribesClass(ent, model);
		}
		else if (ent->client->pers.tribesClass) {
			ent->client->pers.tribesClass = 0;
			if (ent->health > 0)
				G_Kill(ent);
		}
	}
}

static void CVU_Registration(void) {
	g_allowRegistration.integer ?
		(jcinfo2.integer |= JAPRO_CINFO2_REGISTRATION) : (jcinfo2.integer &= ~JAPRO_CINFO2_REGISTRATION);
	trap->Cvar_Set("jcinfo2", va("%i", jcinfo2.integer));
}

static void CVU_Cosmetics(void) {
		char	msg[1024-128] = {0};
		if (g_validateCosmetics.integer) {
			int		i;

			for (i=0; i<MAX_COSMETIC_UNLOCKS; i++) {
				char *tmpMsg = NULL;
				if (!cosmeticUnlocks[i].active)
					continue;

				tmpMsg = va("%i:%s:%i:%i\n", cosmeticUnlocks[i].bitvalue, cosmeticUnlocks[i].mapname, cosmeticUnlocks[i].style, cosmeticUnlocks[i].duration); //probably have to replace the \n with something so it doesnt flood console of old japro clients
				if (strlen(msg) + strlen(tmpMsg) >= sizeof( msg)) {
					trap->SendServerCommand( -1, va("cosmetics \"%s\"", msg));
					msg[0] = '\0';
				}
				Q_strcat(msg, sizeof(msg), tmpMsg);
			}
		}
		trap->SendServerCommand(-1, va("cosmetics \"%s\"", msg));
}

static void UpdateLegacyFixesConfigstring( legacyFixes_t legacyFix, qboolean enabled ) {
	char sLegacyFixes[32];
	trap->GetConfigstring(CS_LEGACY_FIXES, sLegacyFixes, sizeof(sLegacyFixes));

	uint32_t legacyFixes = strtoul(sLegacyFixes, NULL, 0);
	if (enabled) {
		legacyFixes |= (1 << legacyFix);
	} else {
		legacyFixes &= ~(1 << legacyFix);
	}
	trap->SetConfigstring(CS_LEGACY_FIXES, va("%" PRIu32, legacyFixes));
}

static void CVU_FixSaberMoveData(void) {
	BG_FixSaberMoveData();
	UpdateLegacyFixesConfigstring(LEGACYFIX_SABERMOVEDATA, g_fixSaberMoveData.integer);
}

static void CVU_FixRunWalkAnims(void) {
	UpdateLegacyFixesConfigstring(LEGACYFIX_RUNWALKANIMS, g_fixRunWalkAnims.integer);
}

static void CVU_FixWeaponAttackAnim(void) {
	BG_FixWeaponAttackAnim();
	UpdateLegacyFixesConfigstring(LEGACYFIX_WEAPONATTACKANIM, g_fixWeaponAttackAnim.integer);
}

//
// Cvar table
//

typedef struct cvarTable_s {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	void		(*update)( void );
	uint32_t	cvarFlags;
	qboolean	trackChange; // announce if value changes
} cvarTable_t;

#define XCVAR_DECL
	#include "g_xcvar.h"
#undef XCVAR_DECL

static const cvarTable_t gameCvarTable[] = {
	#define XCVAR_LIST
		#include "g_xcvar.h"
	#undef XCVAR_LIST
};
static const size_t gameCvarTableSize = ARRAY_LEN( gameCvarTable );

void G_RegisterCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	for ( i=0, cv=gameCvarTable; i<gameCvarTableSize; i++, cv++ ) {
		trap->Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
		if ( cv->update )
			cv->update();
	}
}

void G_UpdateCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	for ( i=0, cv=gameCvarTable; i<gameCvarTableSize; i++, cv++ ) {
		if ( cv->vmCvar ) {
			int modCount = cv->vmCvar->modificationCount;
			trap->Cvar_Update( cv->vmCvar );
			if ( cv->vmCvar->modificationCount != modCount ) {
				if ( cv->update )
					cv->update();

				if ( cv->trackChange )
					trap->SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
			}
		}
	}
}
