#include "chessPlayer.h"


inline int chessPlayerCalculateScoreForTournament(Player player){
    return 2*player->wins + player->ties;
}



