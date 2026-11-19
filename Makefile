CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRC = main.cpp $(wildcard src/*.cpp)

bank: $(SRC) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) $(SRC) -o bank

clean:
	rm -f bank
