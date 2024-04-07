CFLAGS = -g -std=c99 -Wall -pthread
DEBUGFLAGS = -g -Og -DDEBUG
RELEASEFLAGS = -O3 -DNDEBUG -march=native
LINKFLAGS = -g -flto -lpthread
FLAGS = $(RELEASEFLAGS)

all:
	make UltimateChess
	mv UltimateChess bin

clean: 
	rm -f bin/*.o 
	rm -f bin/UltimateChess

uninstall:
	rm -rf .

roll:
	mkdir -p include
	mv -f src/*.h include

unroll:
	mkdir -p bin
	mv -f include/*.h ./src 
	rm -rf include

bin/Player.o: src/Player.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Menu.o: src/Menu.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/main.o: src/main.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Gameplay.o: src/Gameplay.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/GameData.o: src/GameData.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/AIGameplay.o: src/AIGameplay.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Game.o: src/Game.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Moves.o: src/Moves.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Settings.o: src/Settings.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/Board.o: src/Board.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/AI.o: src/AI.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/MoveList.o: src/MoveList.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/MoveValidation.o: src/MoveValidation.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

bin/tcpClient.o: src/tcpClient.c
	gcc $(CFLAGS) $(FLAGS) -c $^ -o $@

UltimateChess: bin/Player.o bin/Board.o bin/Settings.o bin/main.o bin/Moves.o bin/Menu.o bin/Gameplay.o bin/AI.o bin/Game.o bin/GameData.o bin/AIGameplay.o bin/MoveList.o bin/MoveValidation.o bin/tcpClient.o
	gcc $^ $(LINKFLAGS) -o $@