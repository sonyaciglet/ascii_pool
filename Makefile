CXX = clang++
CXXFLAGS = -Wall
LIBS = -lncurses

app: app.o
	mkdir bin
	$(CXX) -o bin/app main.o $(LIBS)

app.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp
