CXX = g++
#CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++14
CXXFLAGS = -std=c++14 -Wall -Wextra

release: CXXFLAGS += -Ofast -DNDEBUG -flto
release: all

debug: CXXFLAGS += -g3 -DDEBUG
debug: all

profile: CXXFLAGS += -g -Ofast -DNDEBUG -flto

all: board.o
	$(CXX) $(CXXFLAGS) bruteforce.cpp -o bruteforce board.o

clean:
	rm -f bruteforce *.o
