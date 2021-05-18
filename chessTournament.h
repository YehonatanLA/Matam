#ifndef EX1_CHESSTOURNAMENT_H
#define EX1_CHESSTOURNAMENT_H

#include "chessPlayer.h"

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


/** Functions gets the location of a tournament. */
char* getLocation(Tournament tournament);

/** Functions gets the max games per player of a tournament. */
int getMaxGamesPerPlayer(Tournament tournament);

/** Functions returns true if the tournament ended, false otherwise.*/
bool hasTournamentEnded(Tournament tournament, TournamentResult *result);

/** Function that returns the map of games from the tournament.*/
Map getGames(Tournament tournament);

/** Function to be used for copying an int as a key. */
static MapKeyElement copyKeyInt(MapKeyElement n);

/** Function to be used for freeing an int as a key.  */
static void freeKeyInt(MapKeyElement n);

/** Function to be used for comparing to ints and returns the difference. */
static int compareInts(MapKeyElement n1, MapKeyElement n2);

static void *copyGame(MapDataElement game);

/** Function to be used to free a Game struct. */
static void freeGame(MapDataElement game);

/** Sets the ended boolean sign to true, thus signaling for future checks that the tournament ended.*/
TournamentResult tournamentEndedSign(Tournament tournament);


#endif //EX1_CHESSTOURNAMENT_H
