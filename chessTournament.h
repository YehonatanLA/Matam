#ifndef EX1_CHESSTOURNAMENT_H
#define EX1_CHESSTOURNAMENT_H

#include "chessPlayer.h"
#include "map.h"

typedef enum {
    TOURNAMENT_NULL_ARGUMENT,
    TOURNAMENT_SUCCESS

}TournamentResult;


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
const char* getLocation(Tournament tournament);

/** Functions gets the max games per player of a tournament. Assumes the tournament is not NULL.*/
int getMaxGamesPerPlayer(Tournament tournament);

/** Functions returns true if the tournament ended, false otherwise. Assumes the tournament is not NULL.*/
bool hasTournamentEnded(Tournament tournament);

/** Function that returns the map of games from the tournament. Assumes the tournament is not NULL.*/
Map getGames(Tournament tournament);

/** Function to be used for copying an int as a key. Assumes the tournament is not NULL.*/
static MapKeyElement copyKeyInt(MapKeyElement n);

/** Function to be used for freeing an int as a key. */
static void freeKeyInt(MapKeyElement n);

/** Function to be used for comparing to ints and returns the difference. */
static int compareInts(MapKeyElement n1, MapKeyElement n2);

static void *copyGame(MapDataElement game);

/** Function to be used to free a Game struct. */
static void freeGame(MapDataElement game);

/** Sets the ended boolean sign to true, thus signaling for future checks that the tournament ended.*/
TournamentResult tournamentEndedSign(Tournament tournament);

/** The function checks if the two players already played a game against each other in the tournament.
 * Assumes that the tournament isn't NULL and that the player_ids are valid.*/
bool sameGame(Tournament tournament, int player1_id,int player2_id);

/** Checks if the tournament is empty*/
bool isTournamentEmpty(Tournament tournament);

/** */
int calculatePointsOfPlayer(Tournament tournament, int player_id);

/** The function removes wins, losses, and ties from players for the games played in the tournament.
 * Assumes that the tournament and the players_map isn't empty or NULL. */
void updatePlayerStatistics(Map players_map, Tournament tournament);

#endif //EX1_CHESSTOURNAMENT_H
