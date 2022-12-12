all: waffle

waffle: src/main.cpp
	g++ -std=c++17 -o ./waffle src/main.cpp
