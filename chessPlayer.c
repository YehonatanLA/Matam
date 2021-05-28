#include "chessPlayer.h"
#include <stdlib.h>


struct player_t {
    int wins;
    int losses;
    int ties;
    int play_time;
};

Player playerCreate() {
    Player player = malloc(sizeof(struct player_t));
    if (!player) {
        return NULL;
    }
    player->wins = 0;
    player->ties = 0;
    player->losses = 0;
    player->play_time = 0;

    return player;
}


Player playerCopy(Player player) {
    if (!player) {
        return NULL;
    }
    Player new_player = playerCreate();
    if (new_player == NULL) {
        return NULL;
    }
    new_player->wins = player->wins;
    new_player->losses = player->losses;
    new_player->ties = player->ties;
    new_player->play_time = player->play_time;
    return new_player;
}


double chessPlayerCalculateScoreForTournament(Player player) { //inline?
    return ((double) (2 * player->wins + player->ties) / (player->wins + player->losses + player->ties));
}

int getAmountOfGames(Player player) { //inline?
    return player->wins + player->losses + player->ties;
}


int getPlayerLosses(Player player) { //inline?
    return player->losses;
}

int getPLayerWins(Player player) {
    return player->wins;
}

int getPlayerTies(Player player) {
    return player->ties;
}

int getPlayerPlayTime(Player player) {
    return player->play_time;
}

/*
int addGameTime(Player player, int time) {
    player->play_time += time;
}
*/


void removePlayer(Player player) {
    player->play_time = DELETED_PLAYER;
}

void decreasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time) {
    switch (winner) {
        case FIRST_PLAYER:
            player1->wins--;
            player2->losses--;
        case SECOND_PLAYER:
            player1->losses--;
            player2->wins--;
        default:
            player1->ties--;
            player2->ties--;

    }
    if (player1->play_time - game_time >= 0) {
        player1->play_time -= game_time;
    }
    if (player2->play_time - game_time >= 0) {
        player2->play_time -= game_time;
    }

}

void increasePlayersStatistics(Player player1, Player player2, Winner winner, int game_time) {
    switch (winner) {
        case FIRST_PLAYER:
            player1->wins++;
            player2->losses++;
            break;
        case SECOND_PLAYER:
            player1->losses++;
            player2->wins++;
            break;
        case DRAW:
            player1->ties++;
            player2->ties++;
            break;
        default:
            break;
    }
    player1->play_time += game_time;
    player2->play_time += game_time;

}

void technicalWinChessRemovePlayer(Player player, Game game, Winner winner) {
    // If the player is NOT deleted
    if (player->play_time != DELETED_PLAYER) {
        Winner game_win = getWinner(game);
        if (game_win != winner) {
            player->wins++;

            if (winner == DRAW) {
                player->ties--;
            } else {
                player->losses--;
            }
        }
        changeWinner(game, winner);
    }
}


