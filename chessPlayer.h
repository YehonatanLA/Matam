//
// Created by Yehonatan on 18/05/2021.
//

#ifndef EX1_CHESSPLAYER_H
#define EX1_CHESSPLAYER_H

typedef struct player_t *Player;

struct player_t {
    int wins;
    int losses;
    int ties;
    int games_participated_count;
};

inline chessPlayerCalculateScoreForTournament(Player player);

#endif //EX1_CHESSPLAYER_H
