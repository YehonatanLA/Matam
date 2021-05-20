
#ifndef EX1_CHESSGAME_H
#define EX1_CHESSGAME_H

#include <stdbool.h>
#include "chessSystem.h"

typedef struct game_t *Game;

/** Functions returns the id of the first player of the game. */
int getFirstPlayerId(Game game);

/** Functions returns the id of the second player of the game. */
int getSecondPlayerId(Game game);

/** Functions returns the id of the winner of the game. */
Winner getWinner(Game game);

/** Functions returns the time of the game in seconds. */
int getGameTime(Game game);

/** Function that creates a new game. Returns NULL if allocation failed. */
Game createGame(int first_player_id, int second_player_id, Winner winner, int game_time_seconds);



bool IsPlay(Game game, int player_id);

#endif //EX1_CHESSGAME_H
