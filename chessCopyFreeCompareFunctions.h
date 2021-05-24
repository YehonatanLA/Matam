#include "map.h"
#ifndef EX1_CHESSCOPYFREECOMPAREFUNCTIONS_H
#define EX1_CHESSCOPYFREECOMPAREFUNCTIONS_H


/** Function to be used for copying an int as a key. */
static MapKeyElement copyKeyInt(MapKeyElement n);

/** Function to be used for freeing an int as a key.  */
static void freeKeyInt(MapKeyElement n);

/** Function to be used for comparing to ints and returns the difference. */
static int compareInts(MapKeyElement n1, MapKeyElement n2);

/** Function to be used to free a Player struct. */
static void freePlayer(MapDataElement player);

/** Function to be used to copy a Game struct. */
static void *copyGame(MapDataElement game);

/** Function to be used to free a Game struct. */
static void freeGame(MapDataElement game);

/** Function to be used to copy a Tournament struct. */
static MapDataElement copyTournament(MapDataElement tournament);

/** Function to be used to free a Tournament struct. */
static void freeTournament(MapDataElement tournament);

/**Function to be used to copy a player struct. */
MapDataElement copyPlayer(MapDataElement player);


#endif //EX1_CHESSCOPYFREECOMPAREFUNCTIONS_H
