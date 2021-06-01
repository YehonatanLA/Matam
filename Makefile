CC = gcc
OBJS = chessSystemTestsExample.o chessSystem.o chessTournament.o chessGame.o chessPlayer.o chessCopyFreeCompareFunctions.o
EXEC = chess
DEBUG_FLAG =  # now empty, assign -g for debug
CFLAG = -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG
LDFLAGS = -L./ -lmap

$(EXEC) : $(OBJS)
	$(CC) $(CFLAG) $(OBJS) -o $@ $(LDFLAGS)

chessSystemTestsExample.o : tests/chessSystemTestsExample.c test_utilities.h chessSystem.h
	$(CC) -c $(CFLAG) tests/chessSystemTestsExample.c

chessSystem.o : chessSystem.c chessSystem.h chessTournament.c chessTournament.h chessPlayer.c chessPlayer.h  chessGame.c chessGame.h  chessCopyFreeCompareFunctions.c chessCopyFreeCompareFunctions.h
	$(CC) -c $(CFLAG) $*.c

chessTournament.o : chessTournament.c chessTournament.h chessPlayer.c chessPlayer.h chessGame.c chessGame.h chessCopyFreeCompareFunctions.c chessCopyFreeCompareFunctions.h
	$(CC) -c $(CFLAG) $*.c

chessGame.o : chessGame.c chessGame.h chessPlayer.c chessPlayer.h  chessCopyFreeCompareFunctions.c chessCopyFreeCompareFunctions.h
	$(CC) -c $(CFLAG) $*.c

chessPlayer.o : chessPlayer.c chessPlayer.h  chessCopyFreeCompareFunctions.c chessCopyFreeCompareFunctions.h
	$(CC) -c $(CFLAG) $*.c

chessCopyFreeCompareFunctions.o :  chessCopyFreeCompareFunctions.c chessCopyFreeCompareFunctions.h
	$(CC) -c $(CFLAG) $*.c

clean:
	rm -f $(OBJS) $(EXEC)

