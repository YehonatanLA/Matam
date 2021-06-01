#include <stdbool.h>
#include <stdlib.h>
#include "chessGame.h"
#include "chessTournament.h"
#include "chessCopyFreeCompareFunctions.h"

#define NO_WINNER (-1)

#define EMPTY_LOSSES (-1)
struct tournament_t {
    char const *location;
    int max_games_per_player;
    Map games;
    int winner_id;
    Map players;
    int players_amount;

};

/** The function updates the winner of the tournament based on the first element in the players_in_tournament.
 * Returns CHESS_OUT_OF_MEMORY if allocation failed, CHESS_SUCCESS otherwise.*/
static ChessResult finishTournamentEndTournament(Map players_in_tournament, Tournament tournament) ;

/** The function gets the candidate players to be the tournament winner, and removes any player not having
 * the maximum score. Returns CHESS_OUT_OF_MEMORY if allocation failed, otherwise return CHESS_SUCCESS.*/
static ChessResult filterPlayersByScore(Map players_in_tournament);

/** The function gets the candidate players to be the tournament winner, and removes any player not having
 * the minimum loss amount. Returns CHESS_OUT_OF_MEMORY if allocation failed, otherwise return CHESS_SUCCESS.*/
static ChessResult filterPlayersByLosses(Map players_in_tournament);

/** The function gets the candidate players to be the tournament winner, and removes any player not having
 * the maximum win amount. Returns CHESS_OUT_OF_MEMORY if allocation failed, otherwise return CHESS_SUCCESS.*/
static ChessResult filterPlayersByWins(Map players_in_tournament);

Tournament tournamentCreate(const char *location, int max_games_per_player) {
    Tournament tournament = malloc(sizeof(struct tournament_t));
    if (!tournament) {
        return NULL;
    }

    tournament->games = mapCreate(copyGame, copyKeyInt, freeGame, freeKeyInt, compareInts);
    if (!(tournament->games)) {
        free(tournament);
        return NULL;
    }
    tournament->players = mapCreate(copyPlayer, copyKeyInt, freePlayer, freeKeyInt, compareInts);
    if (!(tournament->players)) {
        mapDestroy(tournament->games);
        free(tournament);
        return NULL;
    }
    tournament->location = location;
    tournament->max_games_per_player = max_games_per_player;
    tournament->winner_id = NO_WINNER;
    tournament->players_amount = 0;
    return tournament;
}

const char *getLocation(Tournament tournament) {
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

int getNumberOfGames(Tournament tournament) {
    return mapGetSize(tournament->games);
}

int getNumberOfPlayers(Tournament tournament) {
    return tournament->players_amount;
}

bool isTournamentEmpty(Tournament tournament) {
    return mapGetSize(tournament->games) <= 0;
}

void updatePlayerStatistics(Map players_map, Tournament tournament) {
    Game game;
    int player1_id, player2_id;
    Player player1, player2;

    MAP_FOREACH(MapKeyElement, iterator, tournament->games) {
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

Winner getTournamentWinner(Tournament tournament) {
    return tournament->winner_id;
}


void tournamentDestroy(Tournament tournament)
{
    if (tournament) {
        if(tournament->games){
            mapDestroy(tournament->games);
        }
        if(tournament->players){
            mapDestroy(tournament->players);
        }
        free(tournament);
    }
}

double getAverageGameTime(Tournament tournament) {
    // If a tournament ended, there must be at least one game.
    int sum_play_time = 0, game_amount = 0;
    Game game = NULL;
    MAP_FOREACH(MapKeyElement, game_id, tournament->games) {
        game = (Game) mapGet(tournament->games, game_id);
        sum_play_time += getGameTime(game);
        game_amount++;
        free(game_id);
    }
    return sum_play_time / (double) game_amount;
}


int findLongestGameTime(Tournament tournament) {
    // If a tournament ended, there must be at least one game.
    int max_play_time = 0, curr_game_time;
    Game game = NULL;
    MAP_FOREACH(MapKeyElement, game_id, tournament->games) {
        game = (Game) mapGet(tournament->games, game_id);
        curr_game_time = getGameTime(game);
        if (curr_game_time > max_play_time) {
            max_play_time = curr_game_time;
        }
        free(game_id);
    }
    return max_play_time;
}

void changeGamesChessRemovePlayer(Map players_map, Tournament tournament, int player_id){
    Game game = NULL;
    Player player = NULL;
    bool player_in_game = false;
    int first_player_id, second_player_id;

    MAP_FOREACH(MapKeyElement, game_id, tournament->games) {
        game = (Game) mapGet(tournament->games, game_id);
        second_player_id = getSecondPlayerId(game);
        first_player_id = getFirstPlayerId(game);
        if (first_player_id == player_id) {
            player_in_game = true;
            player = (Player) mapGet(players_map, (MapKeyElement) &second_player_id);
            technicalWinChessRemovePlayer(player, game, SECOND_PLAYER);
            technicalWinTournamentPlayer(tournament, game, SECOND_PLAYER);


        } else if (second_player_id == player_id) {
            player_in_game = true;
            player = (Player) mapGet(players_map, (MapKeyElement) &first_player_id);
            technicalWinChessRemovePlayer(player, game, FIRST_PLAYER);
            technicalWinTournamentPlayer(tournament, game, FIRST_PLAYER);

        }
        free(game_id);

    }
    if(player_in_game){
            removePlayerFromTournament(tournament, player_id);
            player_in_game = false;

    }
}



void technicalWinTournamentPlayer(Tournament tournament, Game game, Winner change_statistics_enum) {
    int first_player_id = getFirstPlayerId(game), second_player_id = getSecondPlayerId(game);
    Winner winner = getWinner(game);
    if (change_statistics_enum == FIRST_PLAYER) {
        Player player1 = (Player) mapGet(tournament->players, (MapKeyElement) &first_player_id);
        technicalWinChessRemovePlayer(player1, game, winner);
    }
    if (change_statistics_enum == SECOND_PLAYER) {
        Player player2 = (Player) mapGet(tournament->players, (MapKeyElement) &second_player_id);
        technicalWinChessRemovePlayer(player2, game, winner);
    }


}

Player handleTournamentPlayerAddGame(Tournament tournament,int player_id, ChessResult *player_tournament_result){

    Player player = NULL;
    if (!mapContains(tournament->players, (MapKeyElement) &player_id)) {
        player = playerCreate();
        tournament->players_amount++;
        if (player == NULL) {
            *player_tournament_result = CHESS_OUT_OF_MEMORY;
            return NULL;
        }

        MapResult result = mapPut(tournament->players, (MapKeyElement) &player_id, (MapDataElement) player);

        switch (result) {
            case MAP_NULL_ARGUMENT:
                freePlayer(player);
                *player_tournament_result = CHESS_NULL_ARGUMENT;
                return NULL;
            case MAP_OUT_OF_MEMORY:
                freePlayer(player);
                *player_tournament_result = CHESS_OUT_OF_MEMORY;
                return NULL;
            default:
                break;
        }

    }
    *player_tournament_result = CHESS_SUCCESS;
    freePlayer(player);
    player = (Player) mapGet(tournament->players, (MapKeyElement) &player_id);
    return player;


}

ChessResult tournamentEndTournament(Tournament tournament) {
    Map players_in_tournament = getTournamentPlayers(tournament);
    if (players_in_tournament == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    ChessResult score_result = filterPlayersByScore(players_in_tournament);
    if(score_result != CHESS_SUCCESS){
        mapDestroy(players_in_tournament);
        return score_result;
    }
    if (mapGetSize(players_in_tournament) == 1) {
        return finishTournamentEndTournament(players_in_tournament, tournament);
    }
    ChessResult losses_result = filterPlayersByLosses(players_in_tournament);
    if(losses_result != CHESS_SUCCESS){
        mapDestroy(players_in_tournament);
        return CHESS_OUT_OF_MEMORY;
    }
    if (mapGetSize(players_in_tournament) == 1) {
        return finishTournamentEndTournament(players_in_tournament, tournament);
    }
    ChessResult win_result = filterPlayersByWins(players_in_tournament);
    if(win_result != CHESS_SUCCESS){
        mapDestroy(players_in_tournament);
        return CHESS_OUT_OF_MEMORY;
    }
    return finishTournamentEndTournament(players_in_tournament, tournament);
}

Map getTournamentPlayers(Tournament tournament) {
    return mapCopy(tournament->players);
}


bool gameAlreadyExists(Tournament tournament, int player1_id, int player2_id) {

    if(!mapContains(tournament->players,(MapKeyElement) &player1_id) ||
        !mapContains(tournament->players,(MapKeyElement) &player2_id)){
        return false;
    }
    for(int game_id = 1; game_id <= mapGetSize(tournament->games); game_id++){
        Game game = (Game) mapGet(tournament->games, (MapKeyElement)&game_id);
        if (checkGameWasPlayed(game, player1_id, player2_id) == true) {
            return true;
        }

    }
    return false;
}


static ChessResult filterPlayersByScore(Map players_in_tournament) {
    //get max score
    Player player = NULL;
    int score, maxScore = 0;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        free(ptr_player_id);
        ptr_player_id = NULL;
        score = chessPlayerCalculateScoreForTournament(player);
        maxScore = maxScore > score ? maxScore : score;
    }
    //filter by score
    Map tmp = mapCopy(players_in_tournament);
    MapResult map_remove_result;
    if(tmp == NULL){
        //memory error
        mapDestroy(tmp);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(int*, ptr_player_id, tmp) {
        player = (Player) mapGet(tmp, (MapKeyElement) ptr_player_id);
        score = chessPlayerCalculateScoreForTournament(player);
        if (maxScore > score) {
            map_remove_result = mapRemove(players_in_tournament, (MapKeyElement) ptr_player_id);
            if(map_remove_result != MAP_SUCCESS){
                free(ptr_player_id);
                mapDestroy(tmp);
                return handleMapRemoveError(map_remove_result);
            }
            free(ptr_player_id);
            ptr_player_id = NULL;

        }
        else {
            free(ptr_player_id);
            ptr_player_id = NULL;
        }

    }
    mapDestroy(tmp);
    return CHESS_SUCCESS;
}

static ChessResult filterPlayersByLosses(Map players_in_tournament)
{
    //get min losses
    Player player = NULL;
    int losses, minLosses = EMPTY_LOSSES;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament)
    {
        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        free(ptr_player_id);
        ptr_player_id = NULL;
        losses = getPlayerLosses(player);
        minLosses = minLosses == EMPTY_LOSSES ? minLosses : losses;
        minLosses = minLosses > losses ? minLosses : losses;
    }
    //filter by losses
    Map tmp = mapCopy(players_in_tournament);
    MapResult map_remove_result;

    if(tmp == NULL){
        //memory error
        mapDestroy(tmp);
        return CHESS_OUT_OF_MEMORY;
    }
    MAP_FOREACH(int*, ptr_player_id, tmp) {
        player = (Player) mapGet(tmp, (MapKeyElement) ptr_player_id);
        losses = getPlayerLosses(player);
        if (minLosses < losses) {
            map_remove_result = mapRemove(players_in_tournament, (MapKeyElement) ptr_player_id);
            if(map_remove_result != MAP_SUCCESS){
                free(ptr_player_id);
                mapDestroy(tmp);
                return handleMapRemoveError(map_remove_result);
            }
            free(ptr_player_id);
            ptr_player_id = NULL;

        }
        else {
            free(ptr_player_id);
            ptr_player_id = NULL;
        }
    }
    mapDestroy(tmp);
    return CHESS_SUCCESS;
}

static ChessResult filterPlayersByWins(Map players_in_tournament) {
    //get max wins
    Player player = NULL;
    int wins, maxWins = 0;
    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {

        player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
        free(ptr_player_id);
        ptr_player_id = NULL;
        wins = getPLayerWins(player);
        maxWins = maxWins < wins ? wins : maxWins;
    }
    //filter by wins
    Map tmp = mapCopy(players_in_tournament);
    MapResult map_remove_result;

    if (tmp == NULL) {
        //memory error
        mapDestroy(tmp);
        return CHESS_OUT_OF_MEMORY;
    }
        MAP_FOREACH(int*, ptr_player_id, tmp) {
            player = (Player) mapGet(players_in_tournament, (MapKeyElement) ptr_player_id);
            wins = getPLayerWins(player);
            if (maxWins > wins) {
                map_remove_result = mapRemove(players_in_tournament, (MapKeyElement) ptr_player_id);
                if(map_remove_result != MAP_SUCCESS){
                    free(ptr_player_id);
                    mapDestroy(tmp);
                    return handleMapRemoveError(map_remove_result);
                }
                free(ptr_player_id);
                ptr_player_id = NULL;
            }

            else {
                free(ptr_player_id);
                ptr_player_id = NULL;
            }

        }
        mapDestroy(tmp);
        return CHESS_SUCCESS;

}

ChessResult addGameToTournament(Tournament tournament, Game game)
{
    if (!tournament->games) {
        return CHESS_OUT_OF_MEMORY;
    }

    int game_amount = mapGetSize(tournament->games) + 1;
    MapResult add_game_result = mapPut(tournament->games, (MapKeyElement) &game_amount, (MapDataElement) game);
    if (add_game_result != MAP_SUCCESS) {
        switch (add_game_result) {
            case MAP_NULL_ARGUMENT:
                return CHESS_NULL_ARGUMENT;
            case MAP_OUT_OF_MEMORY:
                return CHESS_OUT_OF_MEMORY;
            default:
                return CHESS_SUCCESS;
        }

    }
    return CHESS_SUCCESS;

}


bool playerExceededGames(Tournament tournament, int player_id)
{
    Player player = NULL;
    if(!mapContains(tournament->players, (MapKeyElement) &player_id)){
        return false;
    }
    player = (Player) mapGet(tournament->players, (MapKeyElement) &player_id);
    if (tournament->max_games_per_player == getAmountOfGames(player)) {
        return true;
    }
    return false;

}


void removePlayerFromTournament(Tournament tournament, int player_id)
{
        mapRemove(tournament->players, (MapKeyElement) &player_id);
}

static ChessResult finishTournamentEndTournament(Map players_in_tournament, Tournament tournament)
{
    int *winner_id = (int *) mapGetFirst(players_in_tournament);
    if (winner_id == NULL) {
        mapDestroy(players_in_tournament);
        return CHESS_OUT_OF_MEMORY;
    }
    tournament->winner_id = *winner_id;
    free(winner_id);
    winner_id = NULL;
    mapDestroy(players_in_tournament);
    return CHESS_SUCCESS;
}


ChessResult handleMapRemoveError(MapResult result)
{
    switch (result) {
        case MAP_NULL_ARGUMENT:
            return CHESS_NULL_ARGUMENT;
        case MAP_ITEM_DOES_NOT_EXIST:
            return CHESS_TOURNAMENT_NOT_EXIST;
        default:
            break;
    }
    return CHESS_SUCCESS;
}