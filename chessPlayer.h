
#ifndef EX1_CHESSPLAYER_H
#define EX1_CHESSPLAYER_H

typedef struct player_t *Player;

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

/** Function returns the total play amount of a player. Assumes the player is not NULL.*/
int getPlayerPlayTime(Player player);

/** Function returns the amount of games a player played. Assumes the player is not NULL.   */
int getAmountOfGames(Player player);


/** The function decreases the player statistics of a game played,
 * based on the winner of the game and the time played.*/
void decreasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time);

/** The function updates the win, loss, and ties of the players based on the game. */
void increasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time);

/** The function changes the player statistics as a result of the removePlayer function. If that player
 * didn't win the game, the player will gain a win and the loss/tie will be removed.*/
void technicalWinChessRemovePlayer(Player player, Game game, Winner winner);

/** The funciton calculates and returns the score of a player in the tournament.
 * For every win the score is increased by two, and for every tie the score is increased by one.*/
int chessPlayerCalculateScoreForTournament(Player player);

#endif //EX1_CHESSPLAYER_H
