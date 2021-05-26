#include <stdlib.h>
#include "chessSystem.h"
#include "test_utilities.h"

/*The number of tests*/
#define NUMBER_TESTS 5


bool testChessAddTournament() {
    ChessSystem chess = chessCreate();
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddTournament(chess, 2, 5, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddTournament(chess, 4, 3, "A_") == CHESS_INVALID_LOCATION);
    ASSERT_TEST(chessAddTournament(chess, 4, -1, "Test") == CHESS_INVALID_MAX_GAMES);
    ASSERT_TEST(chessAddTournament(chess, -1, 3, "Test") == CHESS_INVALID_ID);
    ASSERT_TEST(chessAddTournament(chess, 1, 10, "Paris") == CHESS_TOURNAMENT_ALREADY_EXISTS);

    chessDestroy(chess);
    return true;
}

bool testChessRemoveTournament() {
    ChessSystem chess = chessCreate();
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddTournament(chess, 2, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessRemoveTournament(chess, 1) == CHESS_SUCCESS);
    ASSERT_TEST(chessRemoveTournament(chess, 1) == CHESS_TOURNAMENT_NOT_EXIST);
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "Paris") == CHESS_SUCCESS);

    chessDestroy(chess);
    return true;
}

bool testChessAddGame() {
    ChessSystem chess = chessCreate();
    ASSERT_TEST(chessAddGame(chess, 1, 1, 2, FIRST_PLAYER, 2000) == CHESS_TOURNAMENT_NOT_EXIST);
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 2, FIRST_PLAYER, 2000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 3, FIRST_PLAYER, 3000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 3, 2, SECOND_PLAYER, 3000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 4, 1, SECOND_PLAYER, 1000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 5, FIRST_PLAYER, 2000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 6, DRAW, 3000) == CHESS_EXCEEDED_GAMES);
    ASSERT_TEST(chessAddGame(chess, 1, -1, 4, FIRST_PLAYER, 3500) == CHESS_INVALID_ID);
    ASSERT_TEST(chessAddGame(chess, 1, 3, 4, -1, 400) == CHESS_INVALID_ID);

    chessDestroy(chess);
    return true;
}

bool testChessPrintLevelsAndTournamentStatistics() {
    FILE *file_levels = fopen("./tests/player_levels_your_output.txt", "w");
    if (!file_levels) {
        printf("test failed to open file, make sure you have the folder tests in the same folder with chessSystem"
               "excutable file and you have write permissions for the file /tests/player_levels_your_output.txt");
    }
    ChessSystem chess = chessCreate();
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 2, FIRST_PLAYER, 2000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 3, FIRST_PLAYER, 3000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 3, 2, SECOND_PLAYER, 3000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 4, 1, SECOND_PLAYER, 1000) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 2, 4, FIRST_PLAYER, 3500) == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 3, 4, DRAW, 400) == CHESS_SUCCESS);
    ASSERT_TEST(chessEndTournament(chess, 1) == CHESS_SUCCESS);
    ASSERT_TEST(chessSavePlayersLevels(chess, file_levels) == CHESS_SUCCESS);
    ASSERT_TEST(chessSaveTournamentStatistics(chess, "./tests/tournament_statistics_your_output.txt") == CHESS_SUCCESS);
    chessDestroy(chess);
    return true;
}

bool testChessCalculateAveragePlayTime() {
    ChessSystem chess = chessCreate();
    ChessResult *result = NULL;
    ASSERT_TEST(chessAddTournament(chess, 1, 4, "London") == CHESS_SUCCESS);
    ASSERT_TEST(chessAddGame(chess, 1, 1, 2, FIRST_PLAYER, 2000) == CHESS_SUCCESS);
    ASSERT_TEST(chessCalculateAveragePlayTime(chess, 1, result) == 2000.0);
    chessDestroy(chess);
    return true;
}



/*The functions for the tests should be added here*/
bool (*tests[])(void) = {
        testChessAddTournament,
        testChessRemoveTournament,
        testChessAddGame,
        testChessPrintLevelsAndTournamentStatistics,
        testChessCalculateAveragePlayTime
};

/*The names of the test functions should be added here*/
const char *testNames[] = {
        "testChessAddTournament",
        "testChessRemoveTournament",
        "testChessAddGame",
        "testChessPrintLevelsAndTournamentStatistics",
        "testChessCalculateAveragePlayTime"
};

int main(int argc, char *argv[]) {
    if (argc == 1) {
        for (int test_idx = 0; test_idx < NUMBER_TESTS; test_idx++) {
            RUN_TEST(tests[test_idx], testNames[test_idx]);
        }
        return 0;
    }
    if (argc != 2) {
        fprintf(stdout, "Usage: chessSystem <test index>\n");
        return 0;
    }

    int test_idx = strtol(argv[1], NULL, 10);
    if (test_idx < 1 || test_idx > NUMBER_TESTS) {
        fprintf(stderr, "Invalid test index %d\n", test_idx);
        return 0;
    }

    RUN_TEST(tests[test_idx - 1], testNames[test_idx - 1]);
    return 0;
}
