#include "chessSystem.h"
#include "map.h"
#include "chessTournament.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "chessCopyFreeCompareFunctions.h"

#define CAPITAL_A 'A'
#define CAPITAL_Z 'Z'
#define SMALL_A 'a'
#define SMALL_Z 'z'
#define SPACE ' '
#define NO_WINNER (-1)
#define WIN_CALCULATION_CONST 6
#define LOSS_CALCULATION_CONST 10
#define TIES_CALCULATION_CONST 2
#define NOT_TAKEN (-1)


/*
 * * Struct chess_system
 * */
struct chess_system_t {
    Map tournaments;
    Map players;
};


/**
 * The function checks if a tournament location is valid. Returns if name is invalid.
 * @params -
 * location: the location name candidate.
 * @return -
 * true - if the location is invalid.
 * false - if the location is valid.
 * */
static bool badLocationName(const char *location);

void findMaxLevel(Map player_map, int* taken_ids, double *max_rank, int *min_id);

/** The function handles and return the correct chess error in case there is a map error from mapPut*/
static ChessResult handleMapPutError(MapResult result);


/** The function checks possible errors for addGame function. If there are no errors, return the tournament that*/
static ChessResult
checkErrorsAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner,
                   int play_time);

/** The function checks if the id of a player is in the ChessSystem, if not it will create a new player.*/
Player handlePlayerAddGame(ChessSystem chess, int player_id, Tournament tournament, ChessResult *chess_result,
                           bool *player_was_allocated);



/**The function checks if a player was deleted. Returns true if player was deleted, false otherwise.
 * Assumes that chess is not NULL and that the player is in the players' map*/
bool playerWasDeleted(ChessSystem chess, int player_id);



/** The function checks if there were no tournaments that ended. Returns true if there weren't, false otherwise.*/
bool noTournamentsEnded(ChessSystem chess);


/** The function checks if a player was deleted.*/
static bool playerWritten(const int *taken_ids, int current_id, int size);

/** Function deallocates memory from the 3 players in the paramaters, and removes player1 and player2 from
 * the players' map. */
static void
deallocatePlayers(ChessSystem chess, Tournament tournament, int first_player_id, int second_player_id,
                  bool first_player_allocated,
                  bool second_player_allocated, bool player1_tournament_allocated, bool player2_tournament_allocated);

/**The function checks handles possible initial errors in chessRemovePlayer. Returns CHESS_SUCCESS if
 * there were no errors, otherwise returns the error that was found.*/
ChessResult chessCheckArgumentForChessRemovePlayer(ChessSystem chess, int player_id);

/** The function checks for possible errors in funciton chessEndTournament and returns the error. If no error
 * was found return CHESS_SUCCESS.*/
static ChessResult checkArgumentsForChessEndTournament(ChessSystem chess, int tournament_id);



ChessSystem chessCreate() {
    ChessSystem chess = malloc(sizeof(*chess));
    if (!chess) {
        return NULL;
    }
    chess->tournaments = mapCreate(copyTournament, copyKeyInt, freeTournament, freeKeyInt, compareInts);
    if (!(chess->tournaments)) {
        free(chess);
        chess = NULL;
        return NULL;
    }
    chess->players = mapCreate(copyPlayer, copyKeyInt, freePlayer, freeKeyInt, compareInts);
    if (!(chess->players)) {
        mapDestroy(chess->tournaments);
        free(chess);
        chess = NULL;
        return NULL;
    }
    return chess;
}


void chessDestroy(ChessSystem chess)
{
    if (chess) {
        mapDestroy(chess->players);
        mapDestroy(chess->tournaments);
        free(chess);


        chess = NULL;
    }
}


ChessResult chessAddTournament(ChessSystem chess, int tournament_id,
                               int max_games_per_player, const char *tournament_location)
                               {
    if (!chess || !tournament_location) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0) {
        return CHESS_INVALID_ID;
    }
    if (mapContains(chess->tournaments, (MapKeyElement) &tournament_id)) {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if (badLocationName(tournament_location)) {
        return CHESS_INVALID_LOCATION;
    }
    if (max_games_per_player <= 0) {
        return CHESS_INVALID_MAX_GAMES;
    }
    Tournament new_tournament = tournamentCreate(tournament_location, max_games_per_player);
    if (new_tournament == NULL) {

        return CHESS_OUT_OF_MEMORY;
    }
    MapResult result = mapPut(chess->tournaments, (MapKeyElement) &tournament_id, (MapDataElement) new_tournament);

    if (result != MAP_SUCCESS) {
        ChessResult chess_result = handleMapPutError(result);
        free(new_tournament);
        return chess_result;
    }
    freeTournament(new_tournament);
    return CHESS_SUCCESS;
}


ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
                         {
    ChessResult resultError = checkErrorsAddGame(chess, tournament_id, first_player, second_player, winner, play_time);
    if (resultError != CHESS_SUCCESS) {
        return resultError;
    }

    Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    Player player1 = NULL, player2 = NULL, player1_tournament = NULL, player2_tournament = NULL;
    bool player1_allocated = false, player2_allocated = false;
    ChessResult player_1_result, player_2_result, player1_tournament_result, player2_tournament_result;
    player1 = handlePlayerAddGame(chess, first_player, tournament, &player_1_result, &player1_allocated);
    if (player_1_result != CHESS_SUCCESS) {
        return player_1_result;
    }
    player2 = handlePlayerAddGame(chess, second_player, tournament, &player_2_result, &player2_allocated);
    if (player_2_result != CHESS_SUCCESS) {
        deallocatePlayers(chess, tournament, first_player, 0, player1_allocated, false, false, false);
        return player_2_result;
    }
    player1_tournament = handleTournamentPlayerAddGame(tournament, first_player, &player1_tournament_result);

    if (player1_tournament_result != CHESS_SUCCESS) {
        deallocatePlayers(chess, tournament, first_player, second_player, player1_allocated, player2_allocated, false,
                          false);

        return CHESS_OUT_OF_MEMORY;
    }

    player2_tournament = handleTournamentPlayerAddGame(tournament, second_player, &player2_tournament_result);
    if (player2_tournament_result != CHESS_SUCCESS) {

        deallocatePlayers(chess, tournament, first_player, second_player, player1_allocated, player2_allocated,
                          player1_allocated, false);
        return CHESS_OUT_OF_MEMORY;
    }
    Game new_game = createGame(first_player, second_player, winner, play_time);
    if (!new_game) {
        deallocatePlayers(chess, tournament, first_player, second_player, player1_allocated, player2_allocated,
                          player1_allocated, player2_allocated);
        freeGame(new_game);
        return CHESS_OUT_OF_MEMORY;
    }

    increasePlayersStatistics(player1_tournament, player2_tournament, winner, play_time);
    increasePlayersStatistics(player1, player2, winner, play_time);

    ChessResult result = addGameToTournament(tournament, new_game);
    freeGame(new_game);
    return result;
}

static void
deallocatePlayers(ChessSystem chess, Tournament tournament, int first_player_id, int second_player_id,
                  bool first_player_allocated,
                  bool second_player_allocated, bool player1_tournament_allocated, bool player2_tournament_allocated)
                  {
    if (first_player_allocated) {
            mapRemove(chess->players, (MapKeyElement) &first_player_id);
    }
    if (second_player_allocated) {
            mapRemove(chess->players, (MapKeyElement) &second_player_id);
    }
    if (player1_tournament_allocated) {
        removePlayerFromTournament(tournament, first_player_id);
    }
    if (player2_tournament_allocated) {
        removePlayerFromTournament(tournament, second_player_id);

    }


}

Player handlePlayerAddGame(ChessSystem chess, int player_id, Tournament tournament, ChessResult *chess_result,
                           bool *player_was_allocated)
                           {
    Player player = NULL;
    if (mapContains(chess->players, (MapKeyElement) &player_id)) {
        if (playerExceededGames(tournament, player_id) == true) {
            *chess_result = CHESS_EXCEEDED_GAMES;
            *player_was_allocated = false;
            return NULL;
        }
        *chess_result = CHESS_SUCCESS;
        return (Player) mapGet(chess->players, (MapKeyElement) &player_id);
    }
    else {
        player = playerCreate();
        if (player == NULL) {
            *chess_result = CHESS_OUT_OF_MEMORY;
            *player_was_allocated = false;
            return NULL;
        }
        *player_was_allocated = true;
    }
    MapResult result = mapPut(chess->players, (MapKeyElement) &player_id, (MapDataElement) player);
    if (result != MAP_SUCCESS) {
        freePlayer(player);
        *chess_result = handleMapPutError(result);
        return NULL;
    }
    *chess_result = CHESS_SUCCESS;
    freePlayer(player);
    player = (Player) mapGet(chess->players, (MapKeyElement) &player_id);
    return player;

}


static ChessResult
checkErrorsAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner,
                   int play_time)
                   {

    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player
        || winner > DRAW || winner < FIRST_PLAYER) {
        return CHESS_INVALID_ID;
    }
    Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (tournament == NULL) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if (hasTournamentEnded(tournament)) {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (gameAlreadyExists(tournament, first_player, second_player)) {
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time <= 0) {
        return CHESS_INVALID_PLAY_TIME;
    }
    return CHESS_SUCCESS;
}


static ChessResult handleMapPutError(MapResult result)
{
    switch (result) {
        case MAP_NULL_ARGUMENT:
            return CHESS_NULL_ARGUMENT;
        case MAP_OUT_OF_MEMORY:

            return CHESS_OUT_OF_MEMORY;
        default:
            return CHESS_SUCCESS;
    }
}


ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id)
{
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, (MapKeyElement) &tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (!isTournamentEmpty(tournament)) {
        // There were games in the tournament, hence the player's statistics need to be updated.
        updatePlayerStatistics(chess->players, tournament);
    }
    MapResult map_remove_result = mapRemove(chess->tournaments, (MapKeyElement) &tournament_id);
    ChessResult result = handleMapRemoveError(map_remove_result);
    if (result != CHESS_SUCCESS) {
        return result;
    }
    //Need to change mapRemove.
    return CHESS_SUCCESS;
}



ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    ChessResult result = chessCheckArgumentForChessRemovePlayer(chess, player_id);
    if (result != CHESS_SUCCESS) {
        return result;
    }
    Tournament tournament = NULL;
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
        if (isTournamentEmpty(tournament) || hasTournamentEnded(tournament)) {
            free(tournament_id);
            continue;
        }

        changeGamesChessRemovePlayer(chess->players, tournament, player_id);
        free(tournament_id);
    }
        mapRemove(chess->players, (MapKeyElement) &player_id);

    return CHESS_SUCCESS;
}

ChessResult chessCheckArgumentForChessRemovePlayer(ChessSystem chess, int player_id)
{

    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id <= 0) {

        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players, (MapKeyElement) &player_id)) {
        return CHESS_PLAYER_NOT_EXIST;
    }
    return CHESS_SUCCESS;
}


ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    ///func 1: checkArgumentsForChessEndTournament
    ChessResult chessResult = checkArgumentsForChessEndTournament(chess, tournament_id);
    if (chessResult != CHESS_SUCCESS) {
        return chessResult;
    }

    Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    ChessResult result = tournamentEndTournament(tournament);
    if (result == CHESS_OUT_OF_MEMORY) {

        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;


}


double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result)
{
    if (!chess) {
        /// change the return since it is double?
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0.0f;
    }
    if (player_id < 0) {
        *chess_result = CHESS_INVALID_ID;
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players, (MapKeyElement) &player_id)) {
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return CHESS_PLAYER_NOT_EXIST;
    }
    if (playerWasDeleted(chess, player_id)) {
        return CHESS_INVALID_ID;
    }
    int game_time_sum = 0, game_amount = 0;
    Tournament tournament;
    Game game;
    Map games = NULL;
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        tournament = (Tournament) (mapGet(chess->tournaments, tournament_id));
        games = getGames(tournament);

        MAP_FOREACH(MapKeyElement, game_id, games) {
            game = (Game) (mapGet(games, game_id));
            if (getFirstPlayerId(game) == player_id || getSecondPlayerId(game) == player_id) {
                game_time_sum += getGameTime(game);
                game_amount++;
            }
            free(game_id);
        }

        free(tournament_id);
        mapDestroy(games);
    }
    *chess_result = CHESS_SUCCESS;
    return (game_time_sum / (double) game_amount);

}



ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file)
{
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if(!file){
        return CHESS_SAVE_FAILURE;
    }

    int player_map_size = mapGetSize(chess->players);

    int *taken_ids = malloc(sizeof(int) * player_map_size);
    if (!taken_ids) {

        return CHESS_OUT_OF_MEMORY;
    }
    for (int i = 0; i < player_map_size; i++) {
        taken_ids[i] = NOT_TAKEN;
    }

    double rank = 0;
    int id = NOT_TAKEN;

    for (int i = 0; i < player_map_size; i++) {
        findMaxLevel(chess->players, taken_ids, &rank, &id);
        fprintf(file, "%d %.2f\n", id, rank);
        taken_ids[i] = id;
        id = NOT_TAKEN;

    }
    free(taken_ids);
    return CHESS_SUCCESS;
}



ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file)
{
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (noTournamentsEnded(chess)) {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    int tournament_winner, longest_game_time, number_of_games, number_of_players;
    double average_game_time;
    FILE *file = fopen(path_file, "a");
    if (!file) {
        return CHESS_SAVE_FAILURE;
    }
    MAP_FOREACH(int*, tournament_id, chess->tournaments) {
        Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) tournament_id);
        if (hasTournamentEnded(tournament)) {
            tournament_winner = getTournamentWinner(tournament);
            longest_game_time = findLongestGameTime(tournament);
            average_game_time = getAverageGameTime(tournament);
            number_of_games = getNumberOfGames(tournament);
            number_of_players = getNumberOfPlayers(tournament);
            const char *location = getLocation(tournament);

            fprintf(file, "%d\n", tournament_winner);
            fprintf(file, "%d\n", longest_game_time);
            fprintf(file, "%.2f\n", average_game_time);
            fprintf(file, "%s\n", location);
            fprintf(file, "%d\n", number_of_games);
            fprintf(file, "%d\n", number_of_players);
        }
        free(tournament_id);
    }
    fclose(file);
    return CHESS_SUCCESS;
}

bool noTournamentsEnded(ChessSystem chess)
{
    if (mapGetSize(chess->tournaments) == 0) {
        return true;
    }
    Tournament tournament = NULL;
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
        if (hasTournamentEnded(tournament)) {
            free(tournament_id);
            return false;
        } else {
            free(tournament_id);
        }
    }
    return true;
}


static bool badLocationName(const char *location)
{

    if (*location < CAPITAL_A || *location > CAPITAL_Z) {
        return true;
    }
    int i = 1;
    while (*(location + i)) {
        if ((location[i] < SMALL_A || location[i] > SMALL_Z) && location[i] != SPACE)
            return true;
        i++;
    }
    return false;
}


void findMaxLevel(Map player_map, int* taken_ids ,double *max_rank, int *min_id)
{
    double player_rank;
    int player_id;
    MAP_FOREACH(int*, iterator, player_map) {
        Player player = (Player) (mapGet(player_map, (MapKeyElement) iterator));
        player_id = *iterator;
        player_rank = (WIN_CALCULATION_CONST * getPLayerWins(player) -
                       LOSS_CALCULATION_CONST * getPlayerLosses(player) +
                       TIES_CALCULATION_CONST * getPlayerTies(player)) / (double) (getAmountOfGames(player));

        if (playerWritten(taken_ids, player_id, mapGetSize(player_map))) {
            free(iterator);
            continue;
        }

        if (*min_id == NOT_TAKEN || *max_rank < player_rank) {
            *max_rank = player_rank;
            *min_id = player_id;
        }
        free(iterator);

    }

}

static bool playerWritten(const int *taken_ids, int current_id, int size) {
    if (current_id == NOT_TAKEN)
        return false;
    for (int i = 0; i < size; i++) {
        if (current_id == taken_ids[i])
            return true;
    }
    return false;
}

static ChessResult checkArgumentsForChessEndTournament(ChessSystem chess, int tournament_id)
{
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, (MapKeyElement) &tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if (hasTournamentEnded(tournament)) {
        return CHESS_TOURNAMENT_ENDED;
    }
    if (isTournamentEmpty(tournament)) {
        return CHESS_NO_GAMES;
    }
    return CHESS_SUCCESS;
}


bool playerWasDeleted(ChessSystem chess, int player_id)
{
    Player player = (Player) mapGet(chess->players, (MapKeyElement) &player_id);
    if (getPlayerPlayTime(player) == DELETED_PLAYER) {
        return true;
    }
    return false;
}


