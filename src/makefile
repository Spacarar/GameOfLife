OBJS = GameOfLife.cpp GameEngine.cpp GridWeb.cpp WebWorker.cpp Grid.cpp

CC = g++

INCLUDE_PATHS = -ID:\Programs\SDL\SDL2-2.0.8\include -ID:\Programs\SDL\SDL2_ttf-2.0.14\include -ID:\Programs\SDL\SDL2_image-2.0.3\include -ID:\Programs\SDL\SDL2_mixer-2.0.2\include

LIBRARY_PATHS = -LD:\Programs\SDL\SDL2-2.0.8\lib\x64 -LD:\Programs\SDL\SDL2_ttf-2.0.14\lib\x64 -LD:\Programs\SDL\SDL2_image-2.0.3\lib\x64 -LD:\Programs\SDL\SDL2_mixer-2.0.2\lib\x64

#removes console window & warnings
PROD_FLAGS = -Wl,-subsystem,windows -w -std=c++14
DEV_FLAGS = -Wall -std=c++14

L_FLAGS = -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -SDL2_mixer -lmingw32
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(DEV_FLAGS) $(L_FLAGS) -o ..\\build\\ConwaysGame.exe
