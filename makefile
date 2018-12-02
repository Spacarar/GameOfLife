OBJS = GameOfLife.cpp GameEngine.cpp GridWeb.cpp Grid.cpp

CC = g++

INCLUDE_PATHS = -ISDL2-2.0.8\include

LIBRARY_PATHS = -LSDL2-2.0.8\lib\x64

#removes console window & warnings
PROD_FLAGS = -Wl,-subsystem,windows -w
DEV_FLAGS = -Wall

L_FLAGS = -lSDL2main -lSDL2 -lmingw32
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(DEV_FLAGS) $(L_FLAGS) -o ConwaysGame.exe
