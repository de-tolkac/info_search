all:
	clang++ -O2 -o prog main.cpp --std=c++17
	clang++ -O2 -o alternative main_alternative.cpp --std=c++17