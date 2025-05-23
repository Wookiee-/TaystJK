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

// cg_playerstate.c -- this file acts on changes in a new playerState_t
// With normal play, this will be done after local prediction, but when
// following another player or playing back a demo, it will be checked
// when the snapshot transitions like all the other entities

#include "cg_local.h"

/*
==============
CG_CheckAmmo

If the ammo has gone low enough to generate the warning, play a sound
==============
*/
void CG_CheckAmmo( void ) {
#if 0
	int		i;
	int		total;
	int		previous;
	int		weapons;

	// see about how many seconds of ammo we have remaining
	weapons = cg.snap->ps.stats[ STAT_WEAPONS ];
	total = 0;
	for ( i = WP_BRYAR_PISTOL; i < WP_NUM_WEAPONS ; i++ ) {
		if ( ! ( weapons & ( 1 << i ) ) ) {
			continue;
		}
		switch ( i )
		{
		case WP_BRYAR_PISTOL:
		case WP_CONCUSSION:
		case WP_BRYAR_OLD:
		case WP_BLASTER:
		case WP_DISRUPTOR:
		case WP_BOWCASTER:
		case WP_REPEATER:
		case WP_DEMP2:
		case WP_FLECHETTE:
		case WP_ROCKET_LAUNCHER:
		case WP_THERMAL:
		case WP_TRIP_MINE:
		case WP_DET_PACK:
		case WP_EMPLACED_GUN:
			total += cg.snap->ps.ammo[weaponData[i].ammoIndex] * 1000;
			break;
		default:
			total += cg.snap->ps.ammo[weaponData[i].ammoIndex] * 200;
			break;
		}
		if ( total >= 5000 ) {
			cg.lowAmmoWarning = 0;
			return;
		}
	}

	previous = cg.lowAmmoWarning;

	if ( total == 0 ) {
		cg.lowAmmoWarning = 2;
	} else {
		cg.lowAmmoWarning = 1;
	}

	if (cg.snap->ps.weapon == WP_SABER)
	{
		cg.lowAmmoWarning = 0;
	}

	// play a sound on transitions
	if ( cg.lowAmmoWarning != previous ) {
		trap->S_StartLocalSound( cgs.media.noAmmoSound, CHAN_LOCAL_SOUND );
	}
#endif
	//disabled silly ammo warning stuff for now
}

/*
==============
CG_DamageFeedback
==============
*/
void CG_DamageFeedback( int yawByte, int pitchByte, int damage ) {
	float		left, front, up;
	float		kick;
	int			health;
	float		scale;
	vec3_t		dir;
	vec3_t		angles;
	float		dist;
	float		yaw, pitch;

	// show the attacking player's head and name in corner
	cg.attackerTime = cg.time;

	// the lower on health you are, the greater the view kick will be
	health = cg.snap->ps.stats[STAT_HEALTH];
	if ( health < 40 ) {
		scale = 1;
	} else {
		scale = 40.0 / health;
	}
	kick = damage * scale;

	if (kick < 5)
		kick = 5;
	if (kick > 10)
		kick = 10;

	// if yaw and pitch are both 255, make the damage always centered (falling, etc)
	if ( yawByte == 255 && pitchByte == 255 ) {
		cg.damageX = 0;
		cg.damageY = 0;
		cg.v_dmg_roll = 0;
		cg.v_dmg_pitch = -kick;
	} else {
		// positional
		pitch = pitchByte / 255.0 * 360;
		yaw = yawByte / 255.0 * 360;

		angles[PITCH] = pitch;
		angles[YAW] = yaw;
		angles[ROLL] = 0;

		AngleVectors( angles, dir, NULL, NULL );
		VectorSubtract( vec3_origin, dir, dir );

		front = DotProduct (dir, cg.refdef.viewaxis[0] );
		left = DotProduct (dir, cg.refdef.viewaxis[1] );
		up = DotProduct (dir, cg.refdef.viewaxis[2] );

		dir[0] = front;
		dir[1] = left;
		dir[2] = 0;
		dist = VectorLength( dir );
		if ( dist < 0.1 ) {
			dist = 0.1f;
		}

		cg.v_dmg_roll = kick * left;

		cg.v_dmg_pitch = -kick * front;

		if ( front <= 0.1 ) {
			front = 0.1f;
		}
		cg.damageX = -left / front;
		cg.damageY = up / dist;
	}

	// clamp the position
	if ( cg.damageX > 1.0 ) {
		cg.damageX = 1.0;
	}
	if ( cg.damageX < - 1.0 ) {
		cg.damageX = -1.0;
	}

	if ( cg.damageY > 1.0 ) {
		cg.damageY = 1.0;
	}
	if ( cg.damageY < - 1.0 ) {
		cg.damageY = -1.0;
	}

	// don't let the screen flashes vary as much
	if ( kick > 10 ) {
		kick = 10;
	}
	cg.damageValue = kick;
	cg.v_dmg_time = cg.time + DAMAGE_TIME;
	cg.damageTime = cg.snap->serverTime;
}




/*
================
CG_Respawn

A respawn happened this snapshot
================
*/
void CG_Respawn( void ) {
	// no error decay on player movement
	cg.thisFrameTeleport = qtrue;

	// display weapons available
	cg.weaponSelectTime = cg.time;

	// select the weapon the server says we are using
	cg.weaponSelect = cg.snap->ps.weapon;
	CG_SetFireMode(cg.weaponSelect);
}

/*
==============
CG_CheckPlayerstateEvents
==============
*/
void CG_CheckPlayerstateEvents( playerState_t *ps, playerState_t *ops ) {
	int			i;
	int			event;
	centity_t	*cent;

	if ( ps->externalEvent && ps->externalEvent != ops->externalEvent ) {
		cent = &cg_entities[ ps->clientNum ];
		cent->currentState.event = ps->externalEvent;
		cent->currentState.eventParm = ps->externalEventParm;
		CG_EntityEvent( cent, cent->lerpOrigin );
	}

	cent = &cg_entities[ ps->clientNum ];
	// go through the predictable events buffer
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
		// if we have a new predictable event
		if ( i >= ops->eventSequence
			// or the server told us to play another event instead of a predicted event we already issued
			// or something the server told us changed our prediction causing a different event
			|| (i > ops->eventSequence - MAX_PS_EVENTS && ps->events[i & (MAX_PS_EVENTS-1)] != ops->events[i & (MAX_PS_EVENTS-1)]) ) {

			event = ps->events[ i & (MAX_PS_EVENTS-1) ];
			cent->currentState.event = event;
			cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
//JLF ADDED to hopefully mark events as player event
			cent->playerState = ps;
			CG_EntityEvent( cent, cent->lerpOrigin );

			cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

			cg.eventSequence++;
		}
	}
}

/*
==================
CG_CheckChangedPredictableEvents
==================
*/
void CG_CheckChangedPredictableEvents( playerState_t *ps ) {
	int i;
	int event;
	centity_t	*cent;

	cent = &cg_entities[ps->clientNum];
	for ( i = ps->eventSequence - MAX_PS_EVENTS ; i < ps->eventSequence ; i++ ) {
		//
		if (i >= cg.eventSequence) {
			continue;
		}
		// if this event is not further back in than the maximum predictable events we remember
		if (i > cg.eventSequence - MAX_PREDICTED_EVENTS) {
			// if the new playerstate event is different from a previously predicted one
			if ( ps->events[i & (MAX_PS_EVENTS-1)] != cg.predictableEvents[i & (MAX_PREDICTED_EVENTS-1) ] ) {

				event = ps->events[ i & (MAX_PS_EVENTS-1) ];
				cent->currentState.event = event;
				cent->currentState.eventParm = ps->eventParms[ i & (MAX_PS_EVENTS-1) ];
				CG_EntityEvent( cent, cent->lerpOrigin );

				cg.predictableEvents[ i & (MAX_PREDICTED_EVENTS-1) ] = event;

				if ( cg_showMiss.integer ) {
					trap->Print("WARNING: changed predicted event\n");
				}
			}
		}
	}
}

/*
==================
pushReward
==================
*/
#ifdef JK2AWARDS
static void pushReward(sfxHandle_t sfx, qhandle_t shader, int rewardCount) {
	if (cg.rewardStack < (MAX_REWARDSTACK-1)) {
		cg.rewardStack++;
		cg.rewardSound[cg.rewardStack] = sfx;
		cg.rewardShader[cg.rewardStack] = shader;
		cg.rewardCount[cg.rewardStack] = rewardCount;
	}
}
#endif

int cgAnnouncerTime = 0; //to prevent announce sounds from playing on top of each other

/*
==================
CG_CheckLocalSounds
==================
*/
void CG_CheckLocalSounds( playerState_t *ps, playerState_t *ops ) {
	int	highScore, reward;//JAPRO - Clientside - Hitsounds
#ifdef JK2AWARDS
	sfxHandle_t sfx;
#endif

	// don't play the sounds if the player just changed teams
	if ( ps->persistant[PERS_TEAM] != ops->persistant[PERS_TEAM] ) {
		return;
	}

	// hit changes
	if ( ps->persistant[PERS_HITS] > ops->persistant[PERS_HITS] ) {
//JAPRO - Clientside - Hitsounds Start
		switch (cg_hitsounds.integer)
		{
			case 0: default: break;
			case 1: trap->S_StartLocalSound(cgs.media.hitSound, CHAN_LOCAL_SOUND);	break;
			case 2: trap->S_StartLocalSound(cgs.media.hitSound2, CHAN_LOCAL_SOUND);	break;
			case 3:	trap->S_StartLocalSound(cgs.media.hitSound3, CHAN_LOCAL_SOUND);	break;
			case 4: trap->S_StartLocalSound(cgs.media.hitSound4, CHAN_LOCAL_SOUND);	break;
			case 5: break;
			case 6: break;
		}
		
	}
	else if ( ps->persistant[PERS_HITS] < ops->persistant[PERS_HITS] ) {
		if (cg_hitsounds.integer) trap->S_StartLocalSound( cgs.media.hitTeamSound, CHAN_LOCAL_SOUND );
	}
//JAPRO - Clientside - Hitsounds End

	// health changes of more than -3 should make pain sounds
	if (cg_oldPainSounds.integer)
	{
		if ( ps->stats[STAT_HEALTH] < (ops->stats[STAT_HEALTH] - 3))
		{
			if ( ps->stats[STAT_HEALTH] > 0 )
			{
				CG_PainEvent( &cg_entities[cg.predictedPlayerState.clientNum], ps->stats[STAT_HEALTH] );
			}
		}
	}

	// if we are going into the intermission, don't start any voices
	if ( cg.intermissionStarted || (cg.snap && cg.snap->ps.pm_type == PM_INTERMISSION) ) {
		return;
	}

#ifdef JK2AWARDS
	// reward sounds
	reward = qfalse;
	if (cg_drawRewards.integer) {
		if (ps->persistant[PERS_CAPTURES] != ops->persistant[PERS_CAPTURES]) {
			pushReward(cgs.media.captureAwardSound, cgs.media.medalCapture, ps->persistant[PERS_CAPTURES]);
			reward = qtrue;
			//Com_Printf("capture\n");

			if (ps->persistant[PERS_CAPTURES] > 1)
				Com_Printf("[skipnotify]Capture %i!\n", ps->persistant[PERS_CAPTURES]);
			else
				Com_Printf("[skipnotify]Capture!\n");
		}
		if (ps->persistant[PERS_IMPRESSIVE_COUNT] != ops->persistant[PERS_IMPRESSIVE_COUNT]) {
			if (cg_drawRewards.integer == 1) {
				sfx = cgs.media.impressiveSound;
				pushReward(sfx, cgs.media.medalImpressive, ps->persistant[PERS_IMPRESSIVE_COUNT]);
			}
			else if (cg_drawRewards.integer == 2) {
				sfx = cgs.media.impressiveSoundQ3;
				pushReward(sfx, cgs.media.medalImpressiveQ3, ps->persistant[PERS_IMPRESSIVE_COUNT]);
			}
			reward = qtrue;
			//Com_Printf("impressive\n");

			if (ps->persistant[PERS_IMPRESSIVE_COUNT] > 1)
				Com_Printf("[skipnotify]Impressive! %i\n", ps->persistant[PERS_IMPRESSIVE_COUNT]);
			else
				Com_Printf("[skipnotify]Impressive!\n");
		}
		if (ps->persistant[PERS_EXCELLENT_COUNT] != ops->persistant[PERS_EXCELLENT_COUNT]) {
			if (cg_drawRewards.integer == 1) {
				sfx = cgs.media.excellentSound;
				pushReward(sfx, cgs.media.medalExcellent, ps->persistant[PERS_EXCELLENT_COUNT]);
			}
			else if (cg_drawRewards.integer == 2) {
				sfx = cgs.media.excellentSoundQ3;
				pushReward(sfx, cgs.media.medalExcellentQ3, ps->persistant[PERS_EXCELLENT_COUNT]);
			}
			reward = qtrue;
			//Com_Printf("excellent\n");

			if (ps->persistant[PERS_IMPRESSIVE_COUNT] > 1)
				Com_Printf("[skipnotify]Ace! %i\n", ps->persistant[PERS_EXCELLENT_COUNT]);
			else
				Com_Printf("[skipnotify]Ace!\n");
		}
		if (ps->persistant[PERS_GAUNTLET_FRAG_COUNT] != ops->persistant[PERS_GAUNTLET_FRAG_COUNT]) {
			if (cg_drawRewards.integer == 1) {
				sfx = cgs.media.humiliationSound;
				pushReward(sfx, cgs.media.medalGauntlet, ps->persistant[PERS_GAUNTLET_FRAG_COUNT]);
			}
			else if (cg_drawRewards.integer == 2) {
				sfx = cgs.media.humiliationSoundQ3;
				pushReward(sfx, cgs.media.medalGauntletQ3, ps->persistant[PERS_GAUNTLET_FRAG_COUNT]);
			}
			reward = qtrue;
			//Com_Printf("gauntlet frag\n");
		}
		if (ps->persistant[PERS_DEFEND_COUNT] != ops->persistant[PERS_DEFEND_COUNT]) {
			pushReward(cgs.media.defendSound, cgs.media.medalDefend, ps->persistant[PERS_DEFEND_COUNT]);
			reward = qtrue;
			//Com_Printf("defend\n");

			if (ps->persistant[PERS_DEFEND_COUNT] > 1)
				Com_Printf("[skipnotify]Defense! %i\n", ps->persistant[PERS_DEFEND_COUNT]);
			else
				Com_Printf("[skipnotify]Defense!\n");
		}
		if (ps->persistant[PERS_ASSIST_COUNT] != ops->persistant[PERS_ASSIST_COUNT]) { //this one was disabled?
			pushReward(cgs.media.assistSound, cgs.media.medalAssist, ps->persistant[PERS_ASSIST_COUNT]);
			reward = qtrue;
			//Com_Printf("assist\n");

			if (ps->persistant[PERS_ASSIST_COUNT] > 1)
				Com_Printf("[skipnotify]Tactician! %i\n", ps->persistant[PERS_ASSIST_COUNT]);
			else
				Com_Printf("[skipnotify]Tactician!\n");
		}
		// if any of the player event bits changed
		if (cgs.serverMod != SVMOD_JAPLUS) {
			if (ps->persistant[PERS_PLAYEREVENTS] != ops->persistant[PERS_PLAYEREVENTS]) {
				if ((ps->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_DENIEDREWARD) !=
					(ops->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_DENIEDREWARD)) {
					trap->S_StartLocalSound( cgs.media.deniedSound, CHAN_ANNOUNCER );
				}
				else if ((ps->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_GAUNTLETREWARD) !=
					(ops->persistant[PERS_PLAYEREVENTS] & PLAYEREVENT_GAUNTLETREWARD)) {
					trap->S_StartLocalSound( cgs.media.humiliationSound, CHAN_ANNOUNCER );
				}
				reward = qtrue;
			}
		}
	}
#else
	reward = qfalse;
#endif
	// lead changes
	if (!reward && cgAnnouncerTime < cg.time) {
		//
		if ( !cg.warmup && cgs.gametype != GT_POWERDUEL ) {
			// never play lead changes during warmup and powerduel
			if ( ps->persistant[PERS_RANK] != ops->persistant[PERS_RANK] ) {
				if ( cgs.gametype < GT_TEAM) {
					/*
					if (  ps->persistant[PERS_RANK] == 0 ) {
						CG_AddBufferedSound(cgs.media.takenLeadSound);
						cgAnnouncerTime = cg.time + 3000;
					} else if ( ps->persistant[PERS_RANK] == RANK_TIED_FLAG ) {
						//CG_AddBufferedSound(cgs.media.tiedLeadSound);
					} else if ( ( ops->persistant[PERS_RANK] & ~RANK_TIED_FLAG ) == 0 ) {
						//rww - only bother saying this if you have more than 1 kill already.
						//joining the server and hearing "the force is not with you" is silly.
						if (ps->persistant[PERS_SCORE] > 0)
						{
							CG_AddBufferedSound(cgs.media.lostLeadSound);
							cgAnnouncerTime = cg.time + 3000;
						}
					}
					*/
				}
			}
		}
	}

	// timelimit warnings
	if ( cgs.timelimit > 0 && cgAnnouncerTime < cg.time ) {
		int		msec;

		msec = cg.time - cgs.levelStartTime;
		if ( !( cg.timelimitWarnings & 4 ) && msec > ( cgs.timelimit * 60 + 2 ) * 1000 ) {
			cg.timelimitWarnings |= 1 | 2 | 4;
			//trap->S_StartLocalSound( cgs.media.suddenDeathSound, CHAN_ANNOUNCER );
			//trap->S_StartLocalSound( cgs.media.oneFragSound, CHAN_ANNOUNCER ); //overtime 
			trap->S_StartLocalSound( cgs.media.tiedLeadSound, CHAN_ANNOUNCER );
		}
		else if ( !( cg.timelimitWarnings & 2 ) && msec > (cgs.timelimit - 1) * 60 * 1000 ) {
			cg.timelimitWarnings |= 1 | 2;
			trap->S_StartLocalSound( cgs.media.oneMinuteSound, CHAN_ANNOUNCER );
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.timelimit > 5 && !( cg.timelimitWarnings & 1 ) && msec > (cgs.timelimit - 5) * 60 * 1000 ) {
			cg.timelimitWarnings |= 1;
			trap->S_StartLocalSound( cgs.media.fiveMinuteSound, CHAN_ANNOUNCER );
			cgAnnouncerTime = cg.time + 3000;
		}
	}

	// fraglimit warnings
	if ( cgs.fraglimit > 0 && cgs.gametype < GT_CTF && cgs.gametype != GT_DUEL && cgs.gametype != GT_POWERDUEL && cgs.gametype != GT_SIEGE && cgAnnouncerTime < cg.time) {
		highScore = cgs.scores1;
		if ( cgs.gametype == GT_TEAM && cgs.scores2 > highScore )
			highScore = cgs.scores2;

		if ( !( cg.fraglimitWarnings & 4 ) && highScore == (cgs.fraglimit - 1) ) {
			cg.fraglimitWarnings |= 1 | 2 | 4;
			CG_AddBufferedSound(cgs.media.oneFragSound);
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.fraglimit > 2 && !( cg.fraglimitWarnings & 2 ) && highScore == (cgs.fraglimit - 2) ) {
			cg.fraglimitWarnings |= 1 | 2;
			CG_AddBufferedSound(cgs.media.twoFragSound);
			cgAnnouncerTime = cg.time + 3000;
		}
		else if ( cgs.fraglimit > 3 && !( cg.fraglimitWarnings & 1 ) && highScore == (cgs.fraglimit - 3) ) {
			cg.fraglimitWarnings |= 1;
			CG_AddBufferedSound(cgs.media.threeFragSound);
			cgAnnouncerTime = cg.time + 3000;
		}
	}
}

/*
===============
CG_TransitionPlayerState

===============
*/
extern int cg_dueltypes[MAX_CLIENTS];//JAPRO - Clientside - Fullforce Duels
void CG_TransitionPlayerState( playerState_t *ps, playerState_t *ops ) {
	// check for changing follow mode
	if ( ps->clientNum != ops->clientNum ) {
		cg.thisFrameTeleport = qtrue;
		// make sure we don't get any unwanted transition effects
		*ops = *ps;
	}

	// damage events (player is getting wounded)
	if ( ps->damageEvent != ops->damageEvent && ps->damageCount ) {
		CG_DamageFeedback( ps->damageYaw, ps->damagePitch, ps->damageCount );
	}

	// respawning
	if ( ps->persistant[PERS_SPAWN_COUNT] != ops->persistant[PERS_SPAWN_COUNT] ) {
		CG_Respawn();
	}

	if ( cg.mapRestart ) {
		CG_Respawn();
		cg.mapRestart = qfalse;
	}

	if ( cg.snap->ps.pm_type != PM_INTERMISSION
		&& ps->persistant[PERS_TEAM] != TEAM_SPECTATOR ) {
		CG_CheckLocalSounds( ps, ops );
	}

	if (ops->weapon != ps->weapon) { //shows weapon select when spectating
		cg.weaponSelect = ps->weapon;
		cg.weaponSelectTime = cg.time;
		CG_SetFireMode(cg.weaponSelect);
	}

	// check for going low on ammo
	CG_CheckAmmo();

	// run events
	CG_CheckPlayerstateEvents( ps, ops );

	if (cgs.serverMod == SVMOD_JAPRO && ps->duelInProgress) { 
		if (ps->stats[STAT_RACEMODE] && ps->stats[STAT_MOVEMENTSTYLE] == MV_COOP_JKA)
		{//hack to update our dueltype in co-op incase we vid_restarted
			cg_dueltypes[ps->clientNum] = 20;
			cg_dueltypes[ps->duelIndex] = 20;
		}
	}

	// smooth the ducking viewheight change
	if ( ps->viewheight != ops->viewheight ) {
		cg.duckChange = ps->viewheight - ops->viewheight;
		if ( cg_instantDuck.integer )
			cg.duckTime = cg.time - DUCK_TIME;
		else
			cg.duckTime = cg.time;
	}
}

