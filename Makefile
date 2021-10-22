CXXFLAGS = -ISDL2/include -std=c++11
#LXXFLAGS = -LSDL2/lib/x86 -lSDL2main -lSDL2 -lSDL2_image
LXXFLAGS = `sdl2-config --cflags --libs`

main.exe: chip8.o platform.o main.o
	g++ src/chip8.o src/platform.o app/main.o -o main.exe $(LXXFLAGS) -std=c++11
main.o: app/main.cpp
	g++ app/main.cpp -o app/main.o -c $(CXXFLAGS)
chip8.o: src/chip8.cpp
	g++ src/chip8.cpp -o src/chip8.o -c $(CXXFLAGS)
platform.o: src/platform.cpp
	g++ src/platform.cpp -o src/platform.o -c $(CXXFLAGS)
