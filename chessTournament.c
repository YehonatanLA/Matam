#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "chessTournament.h"
#include "chessCopyFreeCompareFunctions.h"
#include "chessGame.h"

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

char* getLocation(Tournament tournament){
    if(!tournament){
        return NULL;
    }
    return tournament->location;
}

int getMaxGamesPerPlayer(Tournament tournament){
    if(!tournament){
        return TOURNAMENT_NULL_ARGUMENT;
    }
    return tournament->max_games_per_player;
}

bool hasTournamentEnded(Tournament tournament, TournamentResult *result){
    if(!tournament){
        *result = TOURNAMENT_NULL_ARGUMENT;
        return false;
    }
    return tournament->ended;
}

Map getGames(Tournament tournament){
    if(!tournament){
        return NULL;
    }
    return tournament->games;
}

TournamentResult tournamentEndedSign(Tournament tournament){
    if(!tournament){
        return TOURNAMENT_NULL_ARGUMENT;
    }
    tournament->ended = true;
    return TOURNAMENT_SUCCESS;
}


int calculateWinner(Tournament tournament){
    if(!tournament){
        return NO_WINNER;
    }
    int winner_id = 0, max_score = 0;
    MAP_FOREACH(int*, game_key, tournament->games){
        if(game_key == NULL){
            //out of memory
        }
        Game game = getGame
    }
}



