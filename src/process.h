/*
 * process.h
 *
 *  Created on: May 8, 2018
 *      Author: Guang
 */

#ifndef PROCESS_H_
#define PROCESS_H_
#include "problem.h"
/* includes*/
#include <omp.h>
#include <iostream>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <fstream>
#include <string.h>
#include <vector>
#include <assert.h>
#include <math.h>
#include <vector>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <functional>
using namespace std;
template<class T>
class Process{
	uniform_int_distribution<int> distribution;
	int seed;
	int* numP;
	vector<int> unsatCs;
	double* probs;
	bool* assign;
	double* lookUpTable;
	int* tabuS;
	int maxLOcc;
	T generator;

	/*settings*/
	bool tabu_flag;
	/*option values*/


	int maxFlips;
	int maxSteps;
	int fct;
	int ict;
	int rct1;
	int rct2;
	int cct;
	double cb;
	double eps;
	double lct;
	vector<int>* clauses;
	vector<int>* posC;
	vector<int>* negC;
	int* posOc;
	int* negOc;
	int numCs;
	int numVs;

public:
	Process(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD);
	void optimal();
	void initialAssignment();
	void parseOptions(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD);
	void printOptions();
	double LookUpTable_exp(int literal);
	double LookUpTable_poly(int literal);
	double func_exp(int literal);
	double func_poly(int literal);
	double (Process::*lookUp)(int)  = NULL;
	void initLookUpTable_exp(void);
	void initLookUpTable_poly(void);
	void search_prob();
	int getFlipLiteral(int cIndex);
	void flip(int literal);
	void biasAssignment();
	void randomBiasAssignment();
	void randomAssignment();
	void setAssignment();
	void printAssignment();
	void printUnsatCs();
	void printNumP();
	void debugAssign();
	int computeBreakScore(int literal);
	int (Process::*randINT)(void) = NULL;
	int randI();
	int randI2();
	void printVariables();
};
#endif /* PROCESS_H_ */
