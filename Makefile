all:
	g++ -O3 -fopenmp main.cpp miscellaneous.cpp partition.cpp problem.cpp process.cpp -std=c++11 -o plf
clean:
	rm pl

