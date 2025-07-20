CXX = clang++
CXXFLAGS = -Wall
LIBS = -lncurses

app: app.o
	$(CXX) -o app main.o $(LIBS)

app.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp
