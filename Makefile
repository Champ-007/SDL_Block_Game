SDL_INCLUDE = C:/msys64/mingw64/include/SDL2
SDL_LIBS = C:/msys64/mingw64/lib
SRC_INCLUDE = src/include

SOURCES = main.cpp src/include/chunks.cpp src/include/player.cpp src/include/renderer.cpp src/include/world.cpp
OBJECTS = main.o # src/include/chunks.o src/include/player.o src/include/renderer.o src/include/world.o

all: main

main: $(OBJECTS)
	g++ -Wall -L $(SDL_LIBS) -o main.exe $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

main.o: main.cpp
	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c main.cpp -o main.o

# src/include/chunks.o: src/include/chunks.cpp src/include/chunks.hpp
# 	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/chunks.cpp -o src/include/chunks.o

# src/include/player.o: src/include/player.cpp src/include/player.hpp
# 	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/player.cpp -o src/include/player.o

# src/include/renderer.o: src/include/renderer.cpp src/include/renderer.hpp
# 	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/renderer.cpp -o src/include/renderer.o

# src/include/world.o: src/include/world.cpp src/include/world.hpp
# 	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/world.cpp -o src/include/world.o

# src/include/collider.o: src/include/collider.cpp src/include/collider.hpp
# 	g++ -std=c++20 -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/collider.cpp -o src/include/collider.o

run:
	.\run.bat

clean:
	del main.exe $(OBJECTS)
