main: main.o
	g++ -g main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
main.o: main.cpp
	g++ -g -c main.cpp

clean:
	astyle main.cpp
	rm main.cpp.orig
	rm main.o
	rm sfml-app
