/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/**
 * @brief Globally accessible stuff
 * Contains implementation of synced and unsynced global stuff.
 */

#include "GlobalUnsynced.h"

#include "Player.h"
#include "PlayerHandler.h"
#include "Sim/Misc/TeamHandler.h"
#include "Sim/Misc/GlobalConstants.h" // for RANDINT_MAX
#include "Sim/Units/Unit.h" // required by CREG
#include "System/mmgr.h"
#include "System/Config/ConfigHandler.h"
#include "System/Exceptions.h"
#include "System/Util.h"
#include "System/creg/creg_cond.h"
#include "System/Sync/SyncTracer.h"

#include <SDL_timer.h>
#include <time.h>


/**
 * @brief global unsynced
 *
 * Global instance of CGlobalUnsynced
 */
CGlobalUnsynced* gu;

const float CGlobalUnsynced::reconnectSimDrawBalance = 0.15f;

CR_BIND(CGlobalUnsynced, );

CR_REG_METADATA(CGlobalUnsynced, (
	CR_MEMBER(modGameTime),
	CR_MEMBER(gameTime),
	CR_MEMBER(startTime),
	CR_MEMBER(myPlayerNum),
	CR_MEMBER(myTeam),
	CR_MEMBER(myAllyTeam),
	CR_MEMBER(spectating),
	CR_MEMBER(spectatingFullView),
	CR_MEMBER(spectatingFullSelect),
	CR_MEMBER(fpsMode),
	CR_MEMBER(usRandSeed),
	CR_RESERVED(64)
));

CGlobalUnsynced::CGlobalUnsynced()
{
	usRandSeed = time(NULL) % ((SDL_GetTicks() + 1) * 9007);

	simFPS = 0.0f;

	avgSimFrameTime = 0.0f;
	avgDrawFrameTime = 0.0f;

	modGameTime = 0;
	gameTime = 0;
	startTime = 0;

	myPlayerNum = 0;
	myTeam = 1;
	myAllyTeam = 1;
	myPlayingTeam = -1;
	myPlayingAllyTeam = -1;

	spectating           = false;
	spectatingFullView   = false;
	spectatingFullSelect = false;
	fpsMode = false;

	playerHandler = new CPlayerHandler();

	globalQuit = false;
}

CGlobalUnsynced::~CGlobalUnsynced()
{
	SafeDelete(playerHandler);
}



void CGlobalUnsynced::LoadFromSetup(const CGameSetup* setup)
{
	playerHandler->LoadFromSetup(setup);
}



/**
 * @return unsynced random integer
 *
 * Returns an unsynced random integer
 */
int CGlobalUnsynced::usRandInt()
{
	usRandSeed = (usRandSeed * 214013L + 2531011L);
	return (usRandSeed >> 16) & RANDINT_MAX;
}

/**
 * @return unsynced random float
 *
 * returns an unsynced random float
 */
float CGlobalUnsynced::usRandFloat()
{
	usRandSeed = (usRandSeed * 214013L + 2531011L);
	return float((usRandSeed >> 16) & RANDINT_MAX) / RANDINT_MAX;
}

/**
 * @return unsynced random vector
 *
 * returns an unsynced random vector
 */
float3 CGlobalUnsynced::usRandVector()
{
	float3 ret;
	do {
		ret.x = usRandFloat() * 2 - 1;
		ret.y = usRandFloat() * 2 - 1;
		ret.z = usRandFloat() * 2 - 1;
	} while (ret.SqLength() > 1);

	return ret;
}



void CGlobalUnsynced::SetMyPlayer(const int myNumber)
{
	myPlayerNum = myNumber;

#ifdef TRACE_SYNC
	tracefile.Initialize(myPlayerNum);
#endif

	const CPlayer* myPlayer = playerHandler->Player(myPlayerNum);

	myTeam = myPlayer->team;
	if (!teamHandler->IsValidTeam(myTeam)) {
		throw content_error("Invalid MyTeam in player setup");
	}

	myAllyTeam = teamHandler->AllyTeam(myTeam);
	if (!teamHandler->IsValidAllyTeam(myAllyTeam)) {
		throw content_error("Invalid MyAllyTeam in player setup");
	}

	spectating           = myPlayer->spectator;
	spectatingFullView   = myPlayer->spectator;
	spectatingFullSelect = myPlayer->spectator;

	if (!spectating) {
		myPlayingTeam = myTeam;
		myPlayingAllyTeam = myAllyTeam;
	}
}

CPlayer* CGlobalUnsynced::GetMyPlayer() {
	return (playerHandler->Player(myPlayerNum));
}
