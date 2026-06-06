SDL_INCLUDE = C:/msys64/mingw64/include/SDL2
SDL_LIBS = C:/msys64/mingw64/lib
SRC_INCLUDE = src/include

CXXFLAGS = -I $(SDL_INCLUDE) -I $(SRC_INCLUDE)

OBJECTS = obj/main.o obj/renderer.o obj/chunks.o obj/player.o obj/physics.o obj/collider.o obj/blockRegistry.o

all: main

debug: CXXFLAGS += -g
debug: objectDir $(OBJECTS)
	g++ -Wall -L $(SDL_LIBS) -o dll/main.exe $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -g

main: objectDir $(OBJECTS)
	g++ -Wall -L $(SDL_LIBS) -o dll/main.exe $(OBJECTS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

obj/main.o: main.cpp
	g++ $(CXXFLAGS) -c main.cpp -o obj/main.o

obj/chunks.o: src/include/chunks.cpp src/include/chunks.hpp
	g++ $(CXXFLAGS) -c src/include/chunks.cpp -o obj/chunks.o

obj/player.o: src/include/player.cpp src/include/player.hpp
	g++ $(CXXFLAGS) -c src/include/player.cpp -o obj/player.o

obj/renderer.o: src/include/renderer.cpp src/include/renderer.hpp
	g++ $(CXXFLAGS) -c src/include/renderer.cpp -o obj/renderer.o

obj/physics.o: src/include/physics.cpp src/include/physics.hpp
	g++ $(CXXFLAGS) -c src/include/physics.cpp -o obj/physics.o

obj/collider.o: src/include/collider.cpp
	g++ $(CXXFLAGS) -c src/include/collider.cpp -o obj/collider.o

obj/blockRegistry.o: src/include/blockRegistry.cpp src/include/blockRegistry.hpp
	g++ $(CXXFLAGS) -c src/include/blockRegistry.cpp -o obj/blockRegistry.o

objectDir:
	if not exist obj mkdir obj

run:
	./dll/main.exe

clean:
	del dll\main.exe
	if exist obj rmdir /s /q obj
