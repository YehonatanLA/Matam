
#ifndef EX1_CHESSPLAYER_H
#define EX1_CHESSPLAYER_H

typedef struct player_t *Player;

#define DELETED_PLAYER (-1)

#include "chessGame.h"

/** Function that creates a new player. return the player if succeeded, otherwise NULL.*/
Player playerCreate();

/** Function that copies a new player. Returns the copied player if succeeded, otherwise NULL.*/
Player playerCopy(Player player);

/** Function returns the win amount of a player. Assumes the player is not NULL.*/
int getPLayerWins(Player player);

/** Function returns the loss amount of a player. Assumes the player is not NULL.*/
int getPlayerLosses(Player player);

/** Function returns the tie amount of a player. Assumes the player is not NULL.*/
int getPlayerTies(Player player);

/** Function increases the win amount of a player. Assumes the player is not NULL.   */
void incPLayerWins(Player player);

/** Function increases the loss amount of a player. Assumes the player is not NULL.   */
void incPlayerLosses(Player player);

/** Function increases the tie amount of a player. Assumes the player is not NULL.   */
void incPlayerTies(Player player);

/** Function decreases the win amount of a player. Assumes the player is not NULL.   *//*

void decPLayerWins(Player player);

*/
/** Function decreases the loss amount of a player. Assumes the player is not NULL.   *//*

void decPlayerLosses(Player player);

*/
/** Function restarts the tie amount of a player. Assumes the player is not NULL.   *//*

void decPlayerTies(Player player);
*/

/** Function returns the total play amount of a player. Assumes the player is not NULL.*/
int getPlayerPlayTime(Player player);

/** Function adds to the total play time. */
/*
int addGameTime(Player player, int time);
*/

/** Function returns the amount of games a player played. Assumes the player is not NULL.   */
inline int getAmountOfGames(Player player);

/** Function changes the status of a player to deleted, and restarting all of his stats.
 * If the play_time argument is -1, then the player is deleted. */
void removePlayer(Player player);

/** The function decreases the player statistics of a game played,
 * based on the winner of the game and the time played.*/
void decreasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time);

/** The function updates the win, loss, and ties of the players based on the game. */
void increasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time);

void technicalWinChessRemovePlayer(Player player, Game game, Winner winner);

inline double chessPlayerCalculateScoreForTournament(Player player);

#endif //EX1_CHESSPLAYER_H
