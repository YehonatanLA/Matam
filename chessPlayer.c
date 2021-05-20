#include "chessPlayer.h"
#include <stdlib.h>

#define PLAYER_NULL_ARGUMENT (-1)

struct player_t {
    int wins;
    int losses;
    int ties;
};

Player createPlayer(){
    Player player = malloc(sizeof(Player));
    if(!player){
        return NULL;
    }
    player->wins = 0;
    player->ties = 0;
    player->losses = 0;
    return player;
}


Player playerCopy(Player player){
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
    return new_player;
}




inline double chessPlayerCalculateScoreForTournament(Player player){
    return (2*player->wins + player->ties) / (player->wins + player->losses + player->ties);
}

inline int getAmountOfGames(Player player){
    return player->wins + player->losses + player->ties;
}


inline int getPlayerLosses(Player player){
    return player->losses;
}

int getPLayerWins(Player player){
    return player->wins;
}

int getPlayerTies(Player player){
    return player->ties;
}



void incPLayerWins(Player player){
    player->wins++;
}

void incPlayerLosses(Player player){
    player->losses++;
}

void incPlayerTies(Player player){
    player->ties++;
}



