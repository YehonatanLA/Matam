#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "chessGame.h"
#include "chessTournament.h"

#define NO_WINNER (-1)

struct tournament_t {
    char const *location;
    int max_games_per_player;
    Map games;
    int winner_id;

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
        return NULL;
    }

    tournament->games = mapCreate(copyGame, copyKeyInt, freeGame, freeKeyInt, compareInts);
    if (!(tournament->games)) {
        free(tournament);
        return NULL;
    }
    tournament->location = location;
    tournament->max_games_per_player = max_games_per_player;
    tournament->winner_id = NO_WINNER;
    return tournament;
}

const char  *getLocation(Tournament tournament) {
    return tournament->location;
}

int getMaxGamesPerPlayer(Tournament tournament) {
    return tournament->max_games_per_player;
}

bool hasTournamentEnded(Tournament tournament) {
    return tournament->winner_id != NO_WINNER;
}

Map getGames(Tournament tournament) {
    return tournament->games;
}
yer2_id){
    Game game;
    int first_id, second_id;
    MAP_FOREACH(int*, iterator, tournament->games){
        game = mapGet(tournament->games, (MapKeyElement) iterator);
        first_id = getFirstPlayerId(game);
        second_id = getSecondPlayerId(game);
        if((player1_id == first_id || player1_id == first_id) && (player1_id == second_id || player2_id == second_id)){
            free(iterator);
            return true;
        }
        free(iterator);

    }
    return false;
}

bool isTournamentEmpty(Tournament tournament){
    return mapGetSize(tournament->games) <= 0;
}

void updatePlayerStatistics(Map players_map, Tournament tournament){
    Game game;
    int player1_id, player2_id;
    Player player1, player2;
    MAP_FOREACH(MapKeyElement , iterator, tournament->games){
        game = mapGet(tournament->games, iterator);
        player1_id = getFirstPlayerId(game);
        player2_id = getSecondPlayerId(game);
        Winner winner = getWinner(game);
        player1 = mapGet(players_map, (MapKeyElement) &player1_id);
        player2 = mapGet(players_map, (MapKeyElement) &player2_id);

        switch(winner){
            case FIRST_PLAYER:
                decPLayerWins(player1);
                decPlayerLosses(player2);
            case SECOND_PLAYER:
                decPLayerWins(player2);
                decPlayerLosses(player1);
            default:
                decPlayerTies(player1);
                decPlayerLosses(player2);

        }
        free(iterator);
    }
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



