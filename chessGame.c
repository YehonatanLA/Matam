#include "chessGame.h"
#include <stdlib.h>


struct game_t {
    int player1_id;
    int player2_id;
    Winner winner;
    int game_time_seconds;
};


Game createGame(int first_player_id, int second_player_id, Winner winner, int game_time_seconds) {
    Game new_game = (Game) malloc(sizeof(struct game_t));
    if (!new_game) {
        return NULL;
    }
    new_game->player1_id = first_player_id;
    new_game->player2_id = second_player_id;
    new_game->winner = winner;
    new_game->game_time_seconds = game_time_seconds;
    return new_game;
}


int getFirstPlayerId(Game game) {
    return game->player1_id;
}


int getSecondPlayerId(Game game) {
    return game->player2_id;
}

Winner getWinner(Game game) {
    return game->winner;
}

int getGameTime(Game game) {
    return game->game_time_seconds;
}


Game createCopyGame(Game game) {
    Game new_game = (Game) malloc(sizeof(*new_game));
    if(new_game == NULL){
        //memory error
        return NULL;
    }
    new_game->player1_id = game->player1_id;
    new_game->player2_id = game->player2_id;
    new_game->winner = game->winner;
    new_game->game_time_seconds = game->game_time_seconds;
    return new_game;
}

void changeWinner(Game game, Winner new_winner) {
    if (new_winner != game->winner) {
        game->winner = new_winner;
    }
}

bool checkGameWasPlayed(Game game, int player1_id, int player2_id) {
    if ((game->player1_id == player1_id || game->player1_id == player2_id) &&
        (game->player2_id == player1_id || game->player2_id == player2_id)) {
        return true;
    }
    return false;
}

void deletePlayerIdFromGame(Game game, Winner winner){
    if(winner == FIRST_PLAYER){
        game->player2_id = DELETED_PLAYER;
    }
    else if(winner == SECOND_PLAYER){
        game->player1_id = DELETED_PLAYER;
    }
}

