
#ifndef EX1_CHESSPLAYER_H
#define EX1_CHESSPLAYER_H

typedef struct player_t *Player;


/** Function that creates a new player. return the player if succeeded, otherwise NULL.*/
Player playerCreate();

/** Function that copies a new player. Returns the copied player if succeeded, otherwise NULL.*/
Player playerCopy(Player player);

/** Function returns  the win amount of a player Assumes the player is not NULL.*/
int getPLayerWins(Player player);

/** Function returns the loss amount of a player Assumes the player is not NULL.*/
int getPlayerLosses(Player player);

/** Function returns the tie amount of a player. Assumes the player is not NULL.*/
int getPlayerTies(Player player);

/** Function increases the win amount of a player. Assumes the player is not NULL.   */
void incPLayerWins(Player player);

/** Function increases the loss amount of a player. Assumes the player is not NULL.   */
void incPlayerLosses(Player player);

/** Function increases the tie amount of a player. Assumes the player is not NULL.   */
void incPlayerTies(Player player);

/** Function returns the amount of games a player played. Assumes the player is not NULL.   */
inline int getAmountOfGames(Player player);

inline int chessPlayerCalculateScoreForTournament(Player player);

#endif //EX1_CHESSPLAYER_H
