CXX = g++
#CXXFLAGS = -Wall -Wextra -Werror -pedantic -std=c++14
CXXFLAGS = -std=c++14

debug: CXXFLAGS += -g3 -DDEBUG
debug: all

release: CXXFLAGS += -Ofast -DNDEBUG
release: all

all:
	$(CXX) $(CXXFLAGS) bruteforce.cpp -o bruteforce

clean:
	rm bruteforce
