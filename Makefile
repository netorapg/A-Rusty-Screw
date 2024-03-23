all:
	g++ -o main main.cpp -std=c++11 `pkg-config --cflags --libs sdl2`

mingw:
	g++ -o main.exe main.cpp -I d:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\include\SDL2\ -LD:\mingw\SDL2-2.0.9\x86_64-w64-mingw32\lib\ -lSDL2 -lSDL2main -lmingw32 -w
