#ifndef GAME_STATE__
#define GAME_STATE__

enum gameState {
	LOGIN_VIEW,
	LOBBY_VIEW,
	GAME_VIEW,
	END_VIEW,
	INVALID
};

extern gameState g_gameState;

#endif // !GAME_STATE__

#pragma once
