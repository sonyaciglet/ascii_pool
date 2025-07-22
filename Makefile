CXX = clang++
CXXFLAGS = -Wall -std=c++23 -g
LIBS = -lncurses

app: main.o field.o
	mkdir -p bin
	$(CXX) -o bin/app main.o field.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -d -c main.cpp

field.o: field.cpp
	$(CXX) $(CXXFLAGS) -d -c field.cpp
