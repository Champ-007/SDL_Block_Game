SDL_INCLUDE = C:/msys64/mingw64/include/SDL2
SDL_LIBS = C:/msys64/mingw64/lib
SRC_INCLUDE = src/include

SOURCES = main.cpp src/include/chunks.cpp src/include/player.cpp src/include/renderer.cpp src/include/world.cpp
OBJECTS = obj/main.o obj/renderer.o obj/chunks.o obj/player.o # src/include/world.o

all: main

main: objectDir $(OBJECTS)
	g++ -Wall -L $(SDL_LIBS) -o main.exe $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

obj/main.o: main.cpp
	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c main.cpp -o obj/main.o

obj/chunks.o: src/include/chunks.cpp src/include/chunks.hpp
	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/chunks.cpp -o obj/chunks.o

obj/player.o: src/include/player.cpp src/include/player.hpp
	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/player.cpp -o obj/player.o

obj/renderer.o: src/include/renderer.cpp src/include/renderer.hpp
	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/renderer.cpp -o obj/renderer.o

# src/include/world.o: src/include/world.cpp src/include/world.hpp
# 	g++ -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/world.cpp -o src/include/world.o

# src/include/collider.o: src/include/collider.cpp src/include/collider.hpp
# 	g++ -std=c++20 -I $(SDL_INCLUDE) -I $(SRC_INCLUDE) -c src/include/collider.cpp -o src/include/collider.o

objectDir:
	if not exist obj mkdir obj

run:
	.\run.bat

clean:
	del main.exe
	del save1.txt
	if exist obj rmdir /s /q obj
