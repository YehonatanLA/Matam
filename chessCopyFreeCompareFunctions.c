#include "chessCopyFreeCompareFunctions.h"
#include <stdlib.h>


/**
 * The function checks if a tournament location is valid. Returns if name is invalid.
 * @params -
 * location: the location name candidate.
 * @return -
 * true - if the locatin is invalid.
 * false - if the location is valid.
 * */
bool badLocationName(const char *location);

void findMaxLevel(Map player_map, const int *taken_ids, double *max_rank, int *min_id);

/**
 * The function checks if the player was written in the file or not.
 * @params -
 * taken_ids: the ids of the players that were written down in the file
 * (NOT_TAKEN value means they weren't written down yet).
 * current_id: the id of the current player.
 * @return -
 * false: if the player has already been written down in the file.
 * true: otherwise.
 * */
bool playerWritten(const int *taken_ids, int current_id, int size);



MapKeyElement copyKeyInt(MapKeyElement n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

void freeKeyInt(MapKeyElement n) {
    free(n);
}

int compareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}

MapDataElement copyPlayer(MapDataElement player) {
    if (!player) {
        return NULL;
    }
    Player new_player = (Player) malloc(sizeof(Player));
    if (new_player == NULL) {
        return NULL;
    }
    new_player->wins = ((Player) player)->wins;
    new_player->losses = ((Player) player)->losses;
    new_player->ties = ((Player) player)->ties;
    return (MapDataElement) new_player;
}

void freePlayer(MapDataElement player) {
    free(player);
}

void *copyGame(MapDataElement game) {
    if (!game) {
        return NULL;
    }
    Game new_game = (Game) malloc(sizeof(Game));
    if (!new_game) {
        return NULL;
    }
    new_game->player1_id = ((Game) game)->player1_id;
    new_game->player2_id = ((Game) game)->player2_id;
    new_game->winner = ((Game) game)->winner;
    new_game->game_time_seconds = ((Game) game)->game_time_seconds;
    return new_game;
}

void freeGame(MapDataElement game) {
    free(game);
}


