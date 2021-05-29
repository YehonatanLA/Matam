
#ifndef EX1_CHESSGAME_H
#define EX1_CHESSGAME_H

#include <stdbool.h>
#include "chessSystem.h"

#define DELETED_PLAYER (-1)

typedef struct game_t *Game;

/** Functions returns the id of the first player of the game.Assumes the game is not NULL. */
int getFirstPlayerId(Game game);

/** Functions returns the id of the second player of the game. Assumes the game is not NULL.*/
int getSecondPlayerId(Game game);

/** Functions returns the id of the winner of the game. Assumes the game is not NULL.*/
Winner getWinner(Game game);

/** Functions returns the time of the game in seconds. Assumes the game is not NULL.*/
int getGameTime(Game game);

/** The function changes the winner in the game. */
void changeWinner(Game game, Winner new_winner);

/** Function that creates a new game. Returns NULL if allocation failed. */
Game createGame(int first_player_id, int second_player_id, Winner winner, int game_time_seconds);

/** Function creates a copy of the game and returns it.*/
Game createCopyGame(Game game);

/** Function checks if the two players have played in the game. Returns true if the player played in the game,
 * false otherwise.*/
bool checkGameWasPlayed(Game game, int player1_id, int player2_id);

/** The function deletes the player id of a player that was deleted. */
void deletePlayerIdFromGame(Game game, Winner winner);

bool IsPlay(Game game, int player_id);

#endif //EX1_CHESSGAME_H
