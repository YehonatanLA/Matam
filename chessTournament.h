#ifndef EX1_CHESSTOURNAMENT_H
#define EX1_CHESSTOURNAMENT_H

#include "chessPlayer.h"
#include "map.h"
#include "chessCopyFreeCompareFunctions.h"

typedef enum {
    TOURNAMENT_NULL_ARGUMENT,
    TOURNAMENT_SUCCESS

} TournamentResult;


/** Type for representing a tournament that organizes games inside the chess system. */
typedef struct tournament_t *Tournament;

/**
 * The function creates a tournament struct.
 * @param location: the location of the tournament.
 * @param max_games_per_player: the maximum amount of games a player can play.
 *
 * @return -
 * NULL if the allocation did not succeed.
 * Otherwise the new tournament.
 * */
Tournament tournamentCreate(const char *location, int max_games_per_player);


/** Functions gets the location of a tournament. Assumes the tournament is not NULL.*/
const char *getLocation(Tournament tournament);

/** Functions gets the max games per player of a tournament. Assumes the tournament is not NULL.*/
int getMaxGamesPerPlayer(Tournament tournament);

/** Functions returns true if the tournament ended, false otherwise. Assumes the tournament is not NULL.*/
bool hasTournamentEnded(Tournament tournament);

/** Function that returns the map of games from the tournament. Assumes the tournament is not NULL.*/
Map getGames(Tournament tournament);

/** Sets the ended boolean sign to true, thus signaling for future checks that the tournament ended.*/
TournamentResult tournamentEndedSign(Tournament tournament);

/** The function checks if the two players already played a game against each other in the tournament.
 * Assumes that the tournament isn't NULL and that the player_ids are valid.*/
bool gameAlreadyExists(Tournament tournament, int player1_id, int player2_id);

/** Checks if the tournament is empty*/
bool isTournamentEmpty(Tournament tournament);

/** */
int calculatePointsOfPlayer(Tournament tournament, int player_id);

/**Function returns a copy of the players map.*/
Map getTournamentPlayers(Tournament tournament);

ChessResult tournamentEndTournament(Tournament tournament);

/** The function removes wins, losses, and ties from players for the games played in the tournament.
 * Assumes that the tournament and the players_map isn't empty or NULL. */
void updatePlayerStatistics(Map players_map, Tournament tournament);

/** The function destroys a tournament, deallocating all memory.*/
void tournamentDestroy(Tournament tournament);

/** The function adds a player to the player map inside the tournament. Returns CHESS_OUT_OF_MEMORY if
 * allocation failed, otherwise returns CHESS_SUCCESS. */
ChessResult addPlayerToTournament(Tournament tournament, Player player, int player_id);

/** Function returns the winner of a tournament. Assumes that the tournament ended.*/
Winner getTournamentWinner(Tournament tournament);

/** Function return the number of games in the tournament. Assumes the tournament ended.*/
int getNumberOfGames(Tournament tournament);

/** Function return the number of players in the tournament. Assumes the tournament ended.*/
int getNumberOfPlayers(Tournament tournament);

/** Function finds and returns the longest play time in the tournament. Assumes the tournament ended.*/
int findLongestGameTime(Tournament tournament);

/**Function calculates and returns the average game time. Assumes the tournament ended. */
double getAverageGameTime(Tournament tournament);

/** Function changes (if necessary) the player statistics in the player map inside tournament. */
void technicalWinTournamentPlayer(Tournament tournament, Game game, Winner change_statistics_enum);

void freeGames(Tournament tournament);

#endif //EX1_CHESSTOURNAMENT_H
