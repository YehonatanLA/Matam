#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "chessGame.h"
#include "chessTournament.h"
#include "chessCopyFreeCompareFunctions.h"

#define NO_WINNER (-1)

struct tournament_t {
    char *location;
    int max_games_per_player;
    Map games;
    int winner_id;
    bool ended;

};

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

void *copyGame(MapDataElement game) {
    if (!game) {
        return NULL;
    }
    Game new_game = createGame(getFirstPlayerId((Game) game), getSecondPlayerId((Game) game), getWinner((Game) game),
                               getGameTime((Game) game));
    if (!new_game) {
        return NULL;
    }
    return new_game;
}

void freeGame(MapDataElement game) {
    free(game);
}


Tournament tournamentCreate(const char *location, int max_games_per_player) {
    Tournament tournament = malloc(sizeof(Tournament));
    if (!tournament) {
        free((char *) location);
        return NULL;
    }

    tournament->location = malloc(sizeof(char) * strlen(location));
    if (!(tournament->location)) {
        free(tournament);
        free((char *) location);
        return NULL;
    }

    tournament->games = mapCreate(copyGame, copyKeyInt, freeGame, freeKeyInt, compareInts);
    if (!(tournament->games)) {
        free(tournament->location);
        free(tournament);
        free((char *) location);
        return NULL;
    }
    tournament->location = strcpy(tournament->location, location);
    tournament->max_games_per_player = max_games_per_player;
    tournament->ended = false;
    tournament->winner_id = NO_WINNER;
    return tournament;
    /*Tournament temp = copyTournament(tournament);
    freeTournament(tournament);
    return temp;*/
}

char *getLocation(Tournament tournament) {
    return tournament->location;
}

int getMaxGamesPerPlayer(Tournament tournament) {
    return tournament->max_games_per_player;
}

bool hasTournamentEnded(Tournament tournament) {
    return tournament->ended;
}

Map getGames(Tournament tournament) {
    return tournament->games;
}

TournamentResult tournamentEndedSign(Tournament tournament) {
    tournament->ended = true;
    return TOURNAMENT_SUCCESS;
}


/*

int calculatePointsOfPlayer(Tournament tournament, int player_id) {
    if (!tournament) {
        return NO_WINNER;
    }

    int counter = 0, size = mapGetSize(tournament->games);
    MAP_FOREACH(int*, game_key, tournament->games) {
        Game game = mapGet(tournament->games, (MapKeyElement) game_key);
        ///free(game_key)??
        size--;
        if (!IsPlay(game, player_id)) {
            continue;
        }
        switch (getWinner(game)) {
            case FIRST_PLAYER:
                counter += player_id == getFirstPlayerId(game) ? 2 : 0;
                break;
            case SECOND_PLAYER:
                counter += player_id == getSecondPlayerId(game) ? 2 : 0;
                break;
            case DRAW:
                counter++;
                break;
            default:
                break;
        }

    }
    if (size > 0) {
        ///memory error
    }
    return counter;
}
*/



