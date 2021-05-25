#include "chessCopyFreeCompareFunctions.h"
#include "chessPlayer.h"
#include "chessTournament.h"

#include <stdlib.h>


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
    // Copying a new player with player.h function.
    Player new_player = playerCopy((Player) player);
    if (!new_player) {
        return NULL;
    }
    return (MapDataElement) new_player;
}

void freePlayer(MapDataElement player) {
    free(player);
}


MapDataElement copyTournament(MapDataElement tournament) {
    if (!tournament) {
        return NULL;
    }
    Tournament new_tournament = tournamentCreate(getLocation((Tournament) tournament),
                                                 getMaxGamesPerPlayer((Tournament) tournament));
    if (!new_tournament) {
        return NULL;
    }
    return (MapDataElement) new_tournament;

}

void freeTournament(MapDataElement tournament) {
    if (tournament) {
        tournamentDestroy(tournament);
    }
}

void *copyGame(MapDataElement game) {
    if (!game) {
        return NULL;
    }
    Game new_game = (Game) malloc(sizeof(Game));
    if (!new_game) {
        return NULL;
    }
    copyGameFields(new_game, (Game) game);
    return new_game;
}

void freeGame(MapDataElement game) {
    free(game);
}


