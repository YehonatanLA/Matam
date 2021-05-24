#include <stdbool.h>
#include <stdlib.h>
#include "chessGame.h"
#include "chessTournament.h"
#include "chessCopyFreeCompareFunctions.h"

#define NO_WINNER (-1)

struct tournament_t {
    char const *location;
    int max_games_per_player;
    Map games;
    int winner_id;
    Map players;

};


static MapResult filterPlayersByScore(Map players_in_tournament);

static MapResult filterPlayersByLoses(Map players_in_tournament);

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
    tournament->players = mapCreate(copyPlayer, copyKeyInt, freePlayer, freeKeyInt, compareInts);
    if(!(tournament->players)){
        mapDestroy(tournament->games);
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

    return mapCopy(tournament->games);
}

void freeGames(Tournament tournament){
    mapDestroy(tournament->games);
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

        decreasePlayersStatistics(player1, player2, winner, getGameTime(game));

        free(iterator);
    }
}


void tournamentDestroy(Tournament tournament){
    if(tournament) {
        mapDestroy(tournament->games);
        mapDestroy(tournament->players);
        free(tournament);
    }
}


ChessResult addPlayerToTournament(Tournament tournament, Player player, int player_id){
    MapResult result = mapPut(tournament->players, (MapKeyElement) &player_id, player);
    if (result != CHESS_SUCCESS) {
        free(player);
        switch (result) {
            case MAP_NULL_ARGUMENT:
                return CHESS_NULL_ARGUMENT;
            case MAP_OUT_OF_MEMORY:
                tournamentDestroy(tournament);
                return CHESS_OUT_OF_MEMORY;
            default:
                return CHESS_SUCCESS;
        }
    }

}

ChessResult tournamentAddTournament(Tournament tournament){
        Map players_in_tournament = getTournamentPlayers(tournament);
    if (players_in_tournament == NULL) {
        ///memory error
    }
    ///func 2: fillInPlayersInTournamentForChessEndTournament - CANCEL
    ///func 3+4: filterPlayersByScore
    MapResult mapResult = filterPlayersByScore(players_in_tournament);
    if(mapGetSize(players_in_tournament) == 1){
        int* winner_id = (int*) mapGetFirst(players_in_tournament);
        tournament->winner_id = *winner_id;
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

///func 3+4
static MapResult filterPlayersByScore(Map players_in_tournament){
    //get max score
    Player player = NULL;
    double score, maxScore = 0;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        free(ptr_player_id);
        ptr_player_id = NULL;
        score = chessPlayerCalculateScoreForTournament(player);
        maxScore = maxScore > score ? maxScore : score;
    }
    MapResult result;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        score = chessPlayerCalculateScoreForTournament(player);
        if(maxScore > score) {
            result = mapRemove(players_in_tournament, (MapKeyElement) player);
            if(result != MAP_SUCCESS){
                return result;
            }
        }
        free(ptr_player_id);
        ptr_player_id = NULL;
    }
    return MAP_SUCCESS;
}
///func 5+6
static MapResult filterPlayersByLoses(Map players_in_tournament){
    //get min losses
    Player player = NULL;
    double losses, minLosses = 0;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        free(ptr_player_id);
        ptr_player_id = NULL;
        losses = getPlayerLosses(player);
        minLosses = minLosses < losses ? minLosses : losses;
    }
    MapResult result;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        losses = getPlayerLosses(player);
        if(maxScore > score) {
            result = mapRemove(players_in_tournament, (MapKeyElement) player);
            if(result != MAP_SUCCESS){
                return result;
            }
        }
        free(ptr_player_id);
        ptr_player_id = NULL;
    }
    return MAP_SUCCESS;
}

