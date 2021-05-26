#include "chessSystem.h"
#include "map.h"
#include "chessTournament.h"
#include "chessGame.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
static bool playerWritten(const int *taken_ids, int current_id, int size);

/** The function adds any player that isn't playing in any game to the taken_Ids*/
static int fillUnusedPlayers(Map players_map, int *taken_ids);

/** The function handles and return the correct chess error in case there is a map error from mapPut*/
static ChessResult handleMapPutError(MapResult result);


/** The function checks possible errors for addGame function. If there are no errors, return the tournament that*/
static ChessResult
checkErrorsAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner,
                   int play_time);

/** The function checks if the id of a player is in the ChessSystem, if not it will create a new player.*/
ChessResult handlePlayerAddGame(ChessSystem chess, int player_id, Tournament tournament, Player player);

/** The function add game to the tournament. Returns CHESS_OUT_OF_MEMORY if allocation failed (and deallocates all
 * memory) and CHESS_SUCCESS otherwise. */
ChessResult addGameToTournament(ChessSystem chess, Tournament tournament, Game game);

/**The function checks if a player was deleted. Returns true if player was deleted, false otherwise.
 * Assumes that chess is not NULL and that the player is in the players' map*/
bool playerWasDeleted(ChessSystem chess, int player_id);

/**The function handles possible errors of the result from mapRemove. returns the appropriate ChessResult*/
ChessResult handleMapRemoveError(MapResult result);


///func 1
static ChessResult checkArgumentsForChessEndTournament(ChessSystem chess, int tournament_id);


/**
 * chessCreate: create an empty chess system.
 *
 * @return A new chess system in case of success, and NULL otherwise (e.g.
 *
 *
 *     in case of an allocation error)
 */
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

/**
 * chessDestroy: free a chess system, and all its contents, from
 * memory.
 *
 * @param chess - the chess system to free from memory. A NULL value is
 *     allowed, and in that case the function does nothing.
 */
void chessDestroy(ChessSystem chess) {
    if (chess) {
        mapDestroy(chess->players);
        mapDestroy(chess->tournaments);
        printf("%d", chess == NULL);
        free(chess);


        chess = NULL;
    }
}

/**
 * chessAddTournament: add a new tournament to a chess system.
 *
 * @param chess - chess system to add the tournament to. Must be non-NULL.
 * @param tournament_id - new tournament id. Must be positive, and unique.
 * @param max_games_per_player - maximum number of games a player is allow to play in the specified tournament.
 *                               Must be positive.
 * @param tournament_location - location in which the tournament take place. Must be non-empty.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess/tournament_location are NULL.
 *     CHESS_INVALID_ID - the tournament ID number is invalid.
 *     CHESS_TOURNAMENT_ALREADY_EXISTS - if a tournament with the given id already exist.
 *     CHESS_INVALID_LOCATION - if the name is empty or doesn't start with a capital letter (A -Z)
 *                      followed by small letters (a -z) and spaces (' ').
 *     CHESS_INVALID_MAX_GAMES - if the maximum number of games allowed is not positive
 *     CHESS_SUCCESS - if tournament was added successfully.
 */
ChessResult chessAddTournament(ChessSystem chess, int tournament_id,
                               int max_games_per_player, const char *tournament_location) {
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

    return CHESS_SUCCESS;
}


ChessResult createCopiesOfPlayers(ChessSystem chess, Player player1, Player player2, Player player1_tournament,
                                  Player player2_tournament);


bool noTournamentsEnded(ChessSystem chess);


/**
 * chessAddGame: add a new match to a chess tournament.
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be positive, and unique.
 * @param first_player - first player id. Must be positive.
 * @param second_player - second player id. Must be positive.
 * @param winner - indicates the winner in the match. if it is FIRST_PLAYER, then the first player won.
 *                 if it is SECOND_PLAYER, then the second player won, otherwise the match has ended with a draw.
 * @param play_time - duration of the match in seconds. Must be non-negative.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number, either the players or the winner is invalid or both players
 *                        have the same ID number.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_TOURNAMENT_ENDED - if the tournament already ended
 *     CHESS_GAME_ALREADY_EXISTS - if there is already a game in the tournament with the same two players
 *                                  (both were not removed).
 *     CHESS_INVALID_PLAY_TIME - if the play time is negative.
 *     CHESS_EXCEEDED_GAMES - if one of the players played the maximum number of games allowed
 *     CHESS_SUCCESS - if game was added successfully.
 */

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time) {
    ChessResult resultError = checkErrorsAddGame(chess, tournament_id, first_player, second_player, winner, play_time);
    if (resultError != CHESS_SUCCESS) {
        return resultError;
    }

    Tournament tournament = (Tournament) mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    Player player1 = NULL, player2 = NULL, player1_tournament = NULL, player2_tournament = NULL;
    ChessResult player_1_result = handlePlayerAddGame(chess, first_player, tournament, player1);
    if (player_1_result != CHESS_SUCCESS) {
        return player_1_result;
    }

    ChessResult player_2_result = handlePlayerAddGame(chess, second_player, tournament, player2);
    if (player_2_result != CHESS_SUCCESS) {
        return player_2_result;
    }
    ChessResult create_copies_result = createCopiesOfPlayers(chess, player1, player2, player1_tournament,
                                                             player2_tournament);
    if (create_copies_result != CHESS_SUCCESS) {
        return create_copies_result;
    }
    increasePlayersStatistics(player1_tournament, player2_tournament, winner, play_time);
    increasePlayersStatistics(player1, player2, winner, play_time);

    ChessResult add_to_tournament = addPlayerToTournament(tournament, player1_tournament, first_player);
    if (add_to_tournament != CHESS_SUCCESS) {
        if (add_to_tournament == CHESS_OUT_OF_MEMORY) {

            free(player1_tournament);
        }
        return add_to_tournament;
    }

    add_to_tournament = addPlayerToTournament(tournament, player2_tournament, second_player);
    if (add_to_tournament != CHESS_SUCCESS) {
        if (add_to_tournament == CHESS_OUT_OF_MEMORY) {

            free(player2_tournament);
        }
        return add_to_tournament;
    }

    Game new_game = createGame(first_player, second_player, winner, play_time);
    if (!new_game) {

        return CHESS_OUT_OF_MEMORY;
    }
    ChessResult result = addGameToTournament(chess, tournament, new_game);
    return result;
}

ChessResult createCopiesOfPlayers(ChessSystem chess, Player player1, Player player2, Player player1_tournament,
                                  Player player2_tournament) {
    player1_tournament = playerCopy(player2);
    if (!player1_tournament) {

        free(player1);
        free(player2);
        return CHESS_OUT_OF_MEMORY;
    }

    player2_tournament = playerCopy(player2);
    if (!player2_tournament) {

        free(player1);
        free(player2);
        free(player1_tournament);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;

}

ChessResult addGameToTournament(ChessSystem chess, Tournament tournament, Game game) {
    Map games = getGames(tournament);
    if (!games) {

        return CHESS_OUT_OF_MEMORY;
    }

    int game_amount = mapGetSize(games) + 1;
    mapDestroy(games);
    MapResult add_game_result = mapPut(games, (MapKeyElement) &game_amount, (MapDataElement) game);
    if (add_game_result != MAP_SUCCESS) {
        handleMapPutError(add_game_result);
        free(game);
        return CHESS_OUT_OF_MEMORY;

    }
    mapDestroy(games);
    return CHESS_SUCCESS;

}


ChessResult handlePlayerAddGame(ChessSystem chess, int player_id, Tournament tournament, Player player) {

    if (mapContains(chess->players, (MapKeyElement) &player_id)) {
        player = (Player) mapGet(chess->players, (MapKeyElement) &player_id);
        if (getMaxGamesPerPlayer(tournament) == getAmountOfGames(player)) {
            return CHESS_EXCEEDED_GAMES;
        }
        if (getPlayerPlayTime(player) == DELETED_PLAYER) { // Player was deleted
            player = playerCreate();
            if (player == NULL) {

                return CHESS_OUT_OF_MEMORY;
            }

        }
    }

    else{
        player = playerCreate();
        if (player == NULL) {

            return CHESS_OUT_OF_MEMORY;
        }

        MapResult result = mapPut(chess->players, (MapKeyElement) &player_id,(MapDataElement) player);
        if (result != MAP_SUCCESS) {
            free(player);
            return handleMapPutError(result);
        }
    }
    return CHESS_SUCCESS;

}


static ChessResult
checkErrorsAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner,
                   int play_time) {

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


static ChessResult handleMapPutError(MapResult result) {
    switch (result) {
        case MAP_NULL_ARGUMENT:
            return CHESS_NULL_ARGUMENT;
        case MAP_OUT_OF_MEMORY:

            return CHESS_OUT_OF_MEMORY;
        default:
            return CHESS_SUCCESS;
    }
}

/**
 * chessRemoveTournament: removes the tournament and all the games played in it from the chess system
 *                        updates all players statistics (wins, losses, draws, average play time).
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be positive, and unique.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number is invalid.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_SUCCESS - if tournament was removed successfully.
 */
ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id) {
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
    MapResult result = mapRemove(chess->tournaments, (MapKeyElement) &tournament_id);
    ChessResult map_remove_result = handleMapRemoveError(result);
    if (map_remove_result != CHESS_SUCCESS) {
        return map_remove_result;
    }

    return CHESS_SUCCESS;
}


ChessResult handleMapRemoveError(MapResult result) {
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

/**
 * chessRemovePlayer: removes the player from the chess system.
 *                      In games where the player has participated and not yet ended,
 *                      the opponent is the winner automatically after removal.
 *                      If both player of a game were removed, the game still exists in the system.
 *
 * @param chess - chess system that contains the player. Must be non-NULL.
 * @param player_id - the player id. Must be positive.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the player ID number is invalid.
 *     CHESS_PLAYER_NOT_EXIST - if the player does not exist in the system.
 *     CHESS_SUCCESS - if player was removed successfully.
 */
ChessResult chessRemovePlayer(ChessSystem chess, int player_id) {
    if (!chess) {
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players, (MapKeyElement) &player_id)) {
        return CHESS_PLAYER_NOT_EXIST;
    }

    if (playerWasDeleted(chess, player_id)) {
        return CHESS_INVALID_ID;
    }

    Tournament tournament;
    Game game;
    Player player;
    int first_player_id;
    int second_player_id;
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        tournament = (Tournament) mapGet(chess->tournaments, tournament_id);
        if (isTournamentEmpty(tournament)) {
            free(tournament_id);
            continue;
        }
        MAP_FOREACH(MapKeyElement, game_id, getGames(tournament)) {
            game = (Game) mapGet(getGames(tournament), game_id);
            if (getFirstPlayerId(game) == player_id) {

                second_player_id = getSecondPlayerId(game);
                player = (Player) mapGet(chess->players, (MapKeyElement) &second_player_id);
                technicalWinChessRemovePlayer(player, game, SECOND_PLAYER);
                technicalWinTournamentPlayer(tournament, game, SECOND_PLAYER);

                //add technical win for player map in tournament

            } else if (second_player_id == player_id) {
                first_player_id = getFirstPlayerId(game);
                player = (Player) mapGet(chess->players, (MapKeyElement) &first_player_id);
                technicalWinChessRemovePlayer(player, game, FIRST_PLAYER);
                technicalWinTournamentPlayer(tournament, game, FIRST_PLAYER);

            }
            free(game_id);
        }

        free(tournament_id);
    }
    removePlayer(player);
    return CHESS_SUCCESS;
}

/**
 * chessEndTournament: The function will end the tournament if it has at least one game and
 *                     calculate the id of the winner.
 *                     The winner of the tournament is the player with the highest score:
 *                     player_score = ( num_of_wins * 2 + num_of_draws * 1 ) / ( num_of_games_of_player )
 *                     If two players have the same score, the player with least losses will be chosen.
 *                     If two players have the same number of losses, the player with the most wins will be chosen
 *                     If two players have the same number of wins and losses,
 *                     the player with smaller id will be chosen.
 *                     Once the tournament is over, no games can be added for that tournament.
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be positive, and unique.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number is invalid.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_TOURNAMENT_ENDED - if the tournament already ended
 *     CHESS_N0_GAMES - if the tournament does not have any games.
 *     CHESS_SUCCESS - if tournament was ended successfully.
 */
ChessResult chessEndTournament(ChessSystem chess, int tournament_id) {
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


//
//
//        return result;
//    }
//    Tournament tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
//    Map players_in_tournament = mapCreate(copyPlayer, copyKeyInt, freePlayer, freeKeyInt, compareInts);
//
//    ///func 2: fillInPlayersInTournamentForChessEndTournament
//    result = fillInPlayersInTournamentForChessEndTournament(chess, players_in_tournament, tournament);
//    if(result == CHESS_OUT_OF_MEMORY){
//
//        mapDestroy(players_in_tournament);
//        return CHESS_OUT_OF_MEMORY;
//    }
//    ///func 3: getMaxScoreForChessEndTournament (+4?)
//    double maxScore = getMaxScoreForChessEndTournament(players_in_tournament);
//
//    Player player =  NULL;
//    ///func 4: remove score < max score
//    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
//        player = mapGet((MapKeyElement) ptr_player_id, players_in_tournament);
//        score = chessPlayerCalculateScoreForTournament(player);
//        if (score < maxScore) {
//            free(player);
//            player = NULL;
//            mapRemove(players_in_tournament, (MapKeyElement) ptr_player_id);
//        }
//        free(ptr_player_id);
//    }
//
//    ///func 5: get min losses (+6?)
//    int *firstGameKey = (int *) mapGetFirst(players_in_tournament);
//    int min_losses = getPlayerLosses((Player) mapGet(players_in_tournament, (MapKeyElement) firstGameKey));
//    int losses;
//    winner_id = *firstGameKey;
//    free(firstGameKey);
//    firstGameKey = NULL;
//    MAP_FOREACH(int*, ptr_player_id, players_in_tournament) {
//        player = mapGet((MapKeyElement) ptr_player_id, players_in_tournament);
//        free(ptr_player_id);
//        ptr_player_id = NULL;
//        losses = getPlayerLosses(player);
//        if (min_losses > losses) {
//            min_losses = losses;
//            winner_id = *ptr_player_id;
//        }
//    }
//
//    ///func 6: remove losses > min losses
//
//    ///func 7: get max wins (+8?)
//
//    ///func 8: remove wins < max_wins
//
//    ///func 9: get min_id + set in tournament
//
//    return CHESS_SUCCESS;
//
//}

/**
 * chessCalculateAveragePlayTime: the function returns the average playing time for a particular player
 *
 * @param chess - a chess system that contains the player. Must be non-NULL.
 * @param player_id - player ID. Must be positive.
 * @param chess_result - this variable will contain the returned error code.
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the player ID number is invalid.
 *     CHESS_PLAYER_NOT_EXIST - if the player does not exist in the system.
 *     CHESS_SUCCESS - if average playing time was returned successfully.
 */
double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result) {
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
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        tournament = (Tournament) (mapGet(chess->tournaments, tournament_id));

        MAP_FOREACH(MapKeyElement, game_id, getGames(tournament)) {
            game = (Game) (mapGet(getGames(tournament), game_id));
            if (getFirstPlayerId(game) == player_id || getSecondPlayerId(game) == player_id) {
                game_time_sum += getGameTime(game);
                game_amount++;
            }
            free(game_id);
        }
        free(tournament_id);
    }
    (*chess_result) = CHESS_SUCCESS;
    return (game_time_sum / (double) game_amount);

}


/**
 * chessSavePlayersLevels: prints the rating of all players in the system as
 * explained in the *.pdf
 *
 * @param chess - a chess system. Must be non-NULL.
 * @param file - an open, writable output stream, to which the ratings are printed.
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_SAVE_FAILURE - if an error occurred while saving.
 *     CHESS_SUCCESS - if the ratings was printed successfully.
 */
ChessResult chessSavePlayersLevels(ChessSystem chess, FILE *file) {
    if (!chess || !file) {
        return CHESS_NULL_ARGUMENT;
    }
    int player_map_size = mapGetSize(chess->players);
    int *taken_ids = malloc(sizeof(int *) * player_map_size);
    if (!*(taken_ids)) {

        return CHESS_OUT_OF_MEMORY;
    }
    for (int i = 0; i < player_map_size; i++) {
        taken_ids[i] = NOT_TAKEN;
    }

    double rank = 0;
    int id = NOT_TAKEN;
    int unused = fillUnusedPlayers(chess->players, taken_ids);

    for (int i = unused; i < player_map_size; i++) {
        findMaxLevel(chess->players, taken_ids, &rank, &id);
        if (rank >= 0) {
            fprintf(file, "%d %.2f\n", id, rank);
            taken_ids[i] = id;
        }
        id = NOT_TAKEN;

    }
    free(taken_ids);
    return CHESS_SUCCESS;
}


/**
 * chessSaveTournamentStatistics: prints to the file the statistics for each tournament that ended as
 * explained in the *.pdf
 *
 * @param chess - a chess system. Must be non-NULL.
 * @param path_file - the file path which within it the tournament statistics will be saved.
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_NO_TOURNAMENTS_ENDED - if there are no tournaments ended in the system.
 *     CHESS_SAVE_FAILURE - if an error occurred while saving.
 *     CHESS_SUCCESS - if the ratings was printed successfully.
 */
ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file) {
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
            fprintf(file, "%f\n", average_game_time);
            fprintf(file, "%s\n", location);
            fprintf(file, "%d\n", number_of_games);
            fprintf(file, "%d\n", number_of_players);
            fclose(file);
            free(tournament_id);
        }
    }
    return CHESS_SUCCESS;
}

bool noTournamentsEnded(ChessSystem chess) {
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


static bool badLocationName(const char *location) {

    if (strcmp(location, "") == 0) {
        return true;
    }
    if (*location < CAPITAL_A || *location > CAPITAL_Z) {
        return true;
    }
    /*if(*(location+1) == '\0'){
        return true;
    }*/
    int i = 1;
    while (*(location + i)) {
        if ((location[i] < SMALL_A || location[i] > SMALL_Z) && location[i] != SPACE)
            return true;
        i++;
    }
    return false;
}


static int fillUnusedPlayers(Map players_map, int *taken_ids) {
    int counter = 0;
    MAP_FOREACH(int*, iterator, players_map) {
        if (getAmountOfGames(mapGet(players_map, (MapKeyElement) iterator)) == 0) {
            taken_ids[counter] = *iterator;
            counter++;
        }
        free(iterator);
    }
    return counter;
}

void findMaxLevel(Map player_map, const int *taken_ids, double *max_rank, int *min_id) {
    double player_rank;
    int player_id;
    MAP_FOREACH(int*, iterator, player_map) {
        Player player = (Player) (mapGet(player_map, (MapKeyElement) iterator));
        player_id = *iterator;
        player_rank = (WIN_CALCULATION_CONST * getPLayerWins(player) -
                       LOSS_CALCULATION_CONST * getPlayerLosses(player) +
                       TIES_CALCULATION_CONST * getPlayerTies(player) / (double) (getAmountOfGames(player)));

        if (playerWritten(taken_ids, player_id, mapGetSize(player_map))) {
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

///func 1
static ChessResult checkArgumentsForChessEndTournament(ChessSystem chess, int tournament_id) {
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


bool playerWasDeleted(ChessSystem chess, int player_id) {
    Player player = (Player) mapGet(chess->players, (MapKeyElement) &player_id);
    if (getPlayerPlayTime(player) == DELETED_PLAYER) {
        return true;
    }
    return false;
}


/*
 * Benjo:
 * chessAddGame - DONE
 * chessEndTournament
 * chessSaveTournamentStatistics
 * chessRemovePlayer - DONE
 *
 *  Lachman:
 *  chessDestroy - DONE
 *  chessRemoveTournament - DONE
 *  chessCalculateAveragePlayTime - DONE
 *  chessSavePlayersLevels - DONE
 *
 * */
