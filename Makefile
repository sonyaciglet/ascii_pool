CXX = clang++
CXXFLAGS = -Wall -std=c++23 -g
LIBS = -lncurses

app: app.o bin
	mkdir -p bin
	$(CXX) -o bin/app main.o $(LIBS)

app.o: main.cpp
	$(CXX) $(CXXFLAGS) -d -c main.cpp

