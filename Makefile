all:
	g++ -O3 -fopenmp src/main.cpp src/miscellaneous.cpp src/partition.cpp src/problem.cpp src/process.cpp -std=c++11 -o plf
clean:
	rm pl

