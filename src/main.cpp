/*
 * main.cpp
 *
 *  Created on: Jul 22, 2018
 *      Author: ping
 */
#include "process.h"
#include "miscellaneous.h"

const vector<vector<bool>> setBB= {{false},{false}};
// maxFlips, maxSteps,fct,ict,rct1,rct2,cct,gen,seed
const vector<vector<int>> setII= {{INT_MAX,1,2,1,100,0,50,0,0}, {INT_MAX,1,2,1,100,0,50,1,0}};
const vector<vector<double>> setDD = {{3.6, 1.0,0.5},{3.6, 1.0,0.5}};
int main(int argc, char *argv[]){
	inputFile = argv[1];
	string result = part();
	readPartition(result);
/*	assert(partition.size() == numVsG);
	assert(ind.size() == numVsG);
	for(int i =0; i < numVsG; i++){
		partition[i] = 0;
		ind[i] = i;
	}
	v0 = numVsG;
	v1 = 0;*/
	readFile();
	cout<<endl;
	cout<<"c0:" <<c0<<endl;
	cout<<"c1:"<<c1<<endl;
	cout<<"v0:" <<v0<<endl;
	cout<<"v1:"<<v1<<endl;
	//debugProblem();
	int tid;
#pragma omp parallel num_threads(2) private(tid)
 {
	tid = omp_get_thread_num();
	const vector<bool>setB = setBB[tid];
	const vector<int> setI =setII[tid];
	const vector<double> setD = setDD[tid];
	switch(setI[7]){
	case 0:{
		Process<minstd_rand0> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 1:{
		Process<minstd_rand> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 2:{
		Process<mt19937> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 3:{
		Process<mt19937_64> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 4:{
		Process<ranlux24_base> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 5:{
		Process<ranlux48_base> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 6:{
		Process<ranlux24> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 7:{
		Process<ranlux48> process (setB, setI,setD);
		process.optimal();
		break;
	}
	case 8:{
		Process<knuth_b> process (setB, setI,setD);
		process.optimal();
		break;
	}
	default:{
		Process<knuth_b> process (setB, setI,setD);
		process.optimal();
		break;
	}
	}

}
test(inputFile,assignG);
}


