#include "chessSystem.h"
#include "map.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define CAPITAL_A 'A'
#define CAPITAL_Z 'Z'
#define SMALL_A 'a'
#define SMALL_Z 'z'
#define SPACE ' '
#define NO_WINNER (-1)

#define WIN_CALCULATION_CONST 6
#define LOSS_CALCULATION_CONST 10
#define TIES_CALCULATION_CONST 2
#define NOT_TAKEN -1
typedef struct tournament_t *Tournament;

typedef struct player_t *Player;

typedef struct game_t *Game;

/** Struct chess_system
 * */
struct chess_system_t {
    Map tournaments;
    Map players;
    int amount_of_games; //Key generator
};


struct player_t {
    int wins;
    int losses;
    int ties;
    int games_participated_count;
};

struct tournament_t {
    char *location;
    int max_games_per_player;
    Map games;
    int winner_id;
    bool ended;

};

struct game_t {;
    int player1_id;
    int player2_id;
    int winner;
    int game_time_seconds;
};


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

///Create a new player
static Chess playerCreate(ChessSystem chess);


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


static Tournament tournamentCreate(const char *location, int max_games_per_player);


static MapKeyElement copyKeyInt(MapKeyElement n) {
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

static void freeKeyInt(MapKeyElement n) {
    free(n);
}

static int compareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}

static MapDataElement copyPlayer(MapDataElement player) {
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

static void freePlayer(MapDataElement player) {
    free(player);
}

static void *copyGame(MapDataElement game) {
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

static void freeGame(MapDataElement game) {
    free(game);
}

static MapDataElement copyTournament(MapDataElement tournament) {
    if (!tournament) {
        return NULL;
    }
    Tournament new_tournament = (Tournament) malloc(sizeof(Tournament));
    if (!new_tournament) {
        return NULL;
    }
    new_tournament->location = malloc(sizeof(char) * strlen(((Tournament) tournament)->location));
    if (new_tournament->location == NULL) {
        return NULL;
    }
    strcpy(new_tournament->location, ((Tournament) tournament)->location);
    new_tournament->max_games_per_player = ((Tournament) tournament)->max_games_per_player;
    new_tournament->games = mapCopy(((Tournament) tournament)->games);
    if (((Tournament) tournament)->games == NULL) {
        freeTournament(tournament);
    }
    new_tournament->winner_id = ((Tournament) tournament)->winner_id;
    new_tournament->ended = ((Tournament) tournament)->ended;
    return (MapDataElement) new_tournament;

}

static void freeTournament(MapDataElement tournament) {
    mapDestroy(((Tournament) tournament)->games);
    free(((Tournament) tournament)->location);
    free(tournament);
}


/**
 * chessCreate: create an empty chess system.
 *
 * @return A new chess system in case of success, and NULL otherwise (e.g.
 *
 *
 *     in case of an allocation error)
 */
ChessSystem chessCreate() {
    ChessSystem chess = malloc(sizeof(ChessSystem));
    if (!chess) {
        return NULL;
    }
    chess->tournaments = mapCreate(copyTournament, copyKeyInt, freeTournament, freeKeyInt, compareInts);
    if (!(chess->tournaments)) {
        free(chess);
        return NULL;
    }
    chess->players = mapCreate(copyPlayer, copyKeyInt, freePlayer, freeKeyInt, compareInts);
    if (!(chess->players)) {
        mapDestroy(chess->tournaments);
        free(chess);
        return NULL;
    }
    chess->amount_of_games = 0;
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
        free(chess);
    }
}


/**
 * chessAddTournament: add a new tournament to a chess system.
 *
 * @param chess - chess system to add the tournament to. Must be non-NULL.
 * @param tournament_id - new tournament id. Must be non-negative, and unique.
 * @param tournament_location - location in which the tournament take place. Must be non-empty.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess/tournament_location are NULL.
 *     CHESS_INVALID_ID - the tournament ID number is invalid.
 *     CHESS_INVALID_LOCATION - if the name is empty or doesn't start with a capital letter (A -Z)
 *                      followed by small letters (a -z) and spaces (' ').
 *     CHESS_TOURNAMENT_ALREADY_EXIST - if a tournament with the given id already exist.
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
    if (max_games_per_player <= 0) {
        return CHESS_INVALID_MAX_GAMES;
    }

    if (badLocationName(tournament_location)) {
        return CHESS_INVALID_LOCATION;
    }
    if (mapContains(chess->tournaments, (MapKeyElement) &tournament_id)) {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    Tournament new_tournament = tournamentCreate(tournament_location, max_games_per_player);
    if (new_tournament == NULL) {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult result = mapPut(chess->tournaments, (MapKeyElement) &tournament_id, (MapDataElement) new_tournament);
    switch (result) {
        case MAP_SUCCESS:
            break;

        case MAP_OUT_OF_MEMORY:
            free(new_tournament->location);
            freeTournament(new_tournament);
            chessDestroy(chess);
            free((char *) tournament_location);
            return CHESS_OUT_OF_MEMORY;


        case MAP_NULL_ARGUMENT:
            free(new_tournament->location);
            freeTournament(new_tournament);
            return CHESS_NULL_ARGUMENT;

        default:
            break;

    }
    return CHESS_SUCCESS;


}

static Tournament tournamentCreate(const char *location, int max_games_per_player) {
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


/**
 * chessAddGame: add a new match to a chess tournament.
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be non-negative, and unique.
 * @param first_player - first player id. Must be non-negative.
 * @param second_player - second player id. Must be non-negative.
 * @param winner - indicates the winner in the match. if it is FIRST_PLAYER, then the first player won.
 *                 if it is SECOND_PLAYER, then the second player won, otherwise the match has ended with a draw.
 * @param play_time - duration of the match in seconds. Must be non-negative.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number, either the players or the winner is invalid.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_GAME_ALREADY_EXIST - if there is already a game in the tournament with the same two players.
 *     CHESS_INVALID_PLAY_TIME - if the play time is negative.
 *     CHESS_SUCCESS - if game was added successfully.
 */
ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time){
    if(!chess){
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id < 0 || first_player < 0 || second_player < 0 || first_player == second_player
    || winner > DRAW || winner < FIRST_PLAYER){
        return CHESS_INVALID_ID;
    }
    Tournament tournament = mapGet(chess->tournaments, (MapKeyElement) &tournament_id);
    if(tournament == NULL){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    if(play_time <= 0){
        return CHESS_INVALID_PLAY_TIME;
    }
    if(sameGame(first_player, second_player)){
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if(mapContains(chess->players,(MapKeyElement) &first_player)){
        Player player1 = (Player) mapGet(chess->players,(MapKeyElement) &first_player);
    }
    else{
        Player player1 = playerCreate(chess, first_player);
        if(player1 == NULL){ //out of memory
            chessDestroy(chess);
            return CHESS_OUT_OF_MEMORY;
        }
        MapResult result1 = mapPut(chess->players,(MapKeyElement) &first_player ,(MapDataElement) &player1);
        switch(result1){
            case
        }


    }
    if(mapContains(chess->players,(MapKeyElement) &second_player)) {

        Player player2 = (Player) mapGet(chess->players, (MapKeyElement) &second_player);
    }
    else{
        Player player2 = playerCreate(chess);
    }
    updatePlayers(player1, player2, winner);

    if(winner == FIRST_PLAYER){
        player1->wins += 1;
        player2->losses += 1;
    }
    else if(winner == SECOND_PLAYER){
        player2->wins += 1;
        player1->losses += 1;
    }
    else{
        player1->ties += 1;
        player2->ties += 1;
    }
    Game new_game = malloc(sizeof (Game));
    if(!new_game){
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    new_game->player1_id = first_player;
    new_game->player2_id = second_player;
    new_game->winner = winner;
    new_game->game_time_seconds = play_time;


}

/**
 * chessRemoveTournament: removes the tournament and all the games played in it from the chess system.
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be non-negative, and unique.
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
    if (tournament_id < 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, (MapKeyElement) &tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    MapResult result = mapRemove(chess->tournaments, (MapKeyElement) &tournament_id);
    switch (result) {
        case MAP_SUCCESS:
            break;
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
 *
 * @param chess - chess system that contains the player. Must be non-NULL.
 * @param player_id - the player id. Must be non-negative.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the player ID number is invalid.
 *     CHESS_PLAYER_NOT_EXIST - if the player does not exist in the system.
 *     CHESS_SUCCESS - if player was removed successfully.
 */
ChessResult chessRemovePlayer(ChessSystem chess, int player_id);

/**
 * chessEndTournament: The function will end the tournament and calculate the id of the winner.
 *                     The winner of the tournament is the player with the highest score.
 *                     Once the tournament is over, no games can be added for that tournament.
 *
 * @param chess - chess system that contains the tournament. Must be non-NULL.
 * @param tournament_id - the tournament id. Must be non-negative, and unique.
 *
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the tournament ID number is invalid.
 *     CHESS_TOURNAMENT_NOT_EXIST - if the tournament does not exist in the system.
 *     CHESS_SUCCESS - if tournament was ended successfully.
 */
ChessResult chessEndTournament(ChessSystem chess, int tournament_id);

/**
 * chessCalculateAveragePlayTime: the function returns the average playing time for a particular player.
 *
 * @param chess - a chess system that contains the player. Must be non-NULL.
 * @param player_id - player ID. Must be non-negative.
 * @param chess_result - this variable will contain the returned error code.
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_INVALID_ID - if the player ID number is invalid.
 *     CHESS_PLAYER_NOT_EXIST - if the player does not exist in the system.
 *     CHESS_SUCCESS - if average playing time was returned successfully.
 */
double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult *chess_result) {
    if (!chess) {
        (*chess_result) = CHESS_NULL_ARGUMENT;
        return CHESS_NULL_ARGUMENT;
    }
    if (player_id < 0) {
        (*chess_result) = CHESS_INVALID_ID;
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->players, (MapKeyElement) &player_id)) {
        (*chess_result) = CHESS_PLAYER_NOT_EXIST;
        return CHESS_PLAYER_NOT_EXIST;
    }
    int game_time_sum = 0, game_amount = 0;
    MAP_FOREACH(MapKeyElement, tournament_id, chess->tournaments) {
        /// Should I declare outside the for loop?
        Tournament tournament = (Tournament) (mapGet(chess->tournaments, tournament_id));
        MAP_FOREACH(MapKeyElement, game_id, tournament->games) {
            /// Should I declare outside the for loop?
            Game game = (Game) (mapGet(tournament->games, game_id));
            if (game->player1_id == player_id || game->player2_id == player_id) {
                game_time_sum += game->game_time_seconds;
                game_amount++;
            }
        }
    }
    (*chess_result) = CHESS_SUCCESS;
    return (game_time_sum / (double) game_amount);

}


/**
 * chessSavePlayersLevels: prints the rating of all players in the system as
 * explained in the *.pdf
 *
 * @param chess - a chess system. Must be non-NULL.
 * @param file - an open, writable output file, to which the ratings are printed.
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
        chessDestroy(chess);
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

}


/**
 * chessSaveTournamentStatistics: prints to the file the statistics for each tournament that ended as
 * explained in the *.pdf
 *
 * @param chess - a chess system. Must be non-NULL.
 * @param path_file - the file path which within it the tournament statistics will be saved.
 * @return
 *     CHESS_NULL_ARGUMENT - if chess is NULL.
 *     CHESS_NO_TOURMENTS_ENDED - if there are no tournaments ended in the system.
 *     CHESS_SAVE_FAILURE - if an error occurred while saving.
 *     CHESS_SUCCESS - if the ratings was printed successfully.
 */
ChessResult chessSaveTournamentStatistics(ChessSystem chess, char *path_file);


bool badLocationName(const char *location) {
    if (strcmp(location, "") == 0) {
        return true;
    }
    if (*location < CAPITAL_A || *location > CAPITAL_Z) {
        return true;
    }
    int i = 1;
    while (*(location + i)) {
        if ((location[i] < SMALL_A || location[i] > SMALL_Z) && location[i] != SPACE)
            return true;
    }
    return false;
}


void findMaxLevel(Map player_map, const int *taken_ids, double *max_rank, int *min_id) {
    double player_rank;
    int player_id;
    MAP_FOREACH(int*, iterator, player_map) {
        Player player = (Player) (mapGet(player_map, (MapKeyElement) iterator));
        player_id = *iterator;
        player_rank = (WIN_CALCULATION_CONST * player->wins - LOSS_CALCULATION_CONST * player->losses +
                       TIES_CALCULATION_CONST * player->ties) / (double) (player->games_participated_count);

        if(playerWritten(taken_ids, player_id, mapGetSize(player_map))){
            continue;
        }
        if ( *min_id == NOT_TAKEN || *max_rank < player_rank )  {
            *max_rank = player_rank;
            *min_id = player_id;
        }

    }

}

bool playerWritten(const int *taken_ids, int current_id, int size) {
    if(current_id == NOT_TAKEN)
        return false;
    for(int i = 0; i < size; i++){
        if(current_id == taken_ids[i])
            return true;
    }
    return false;
}

static ChessResult playerCreate(ChessSystem chess, int id){
    Player player = malloc(sizeof(Player));
    if(player == NULL) {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    player->wins = 0;
    player->losses = 0;
    player->ties = 0;
    player->games_participated_count = 0;
    return player;
}

/*
 * Benjo:
 * chessAddGame
 * chessEndTournament
 * chessSaveTournamentStatistics
 * chessRemovePlayer
 *
 *  Lachman:
 *  chessDestroy - DONE
 *  chessRemoveTournament - DONE
 *  chessCalculateAveragePlayTime - DONE
 *  chessSavePlayersLevels - DONE
 *
 * */
