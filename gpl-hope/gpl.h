/*
 * main.h
 *
 *  Created on: May 8, 2018
 *      Author: Guang 
 */

#ifndef MAIN_H_
#define MAIN_H_

/* includes*/
#include <omp.h>
#include <iostream>
#include <climits>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string>
#include <string.h>
#include <fstream>
#include <vector>
#include <assert.h>
#include <math.h>
#include <vector>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <functional>
#include <memory>
using namespace std;

const vector<vector<bool>> setBB= {{false},{false}};
// maxFlips, maxSteps,fct,ict,rct1,rct2,cct,gen,seed
const vector<vector<int>> setII= {{2,INT_MAX,2,1,100,0,50,0,0}, {2,INT_MAX,1,2,1,100,0,50,1,0}};
const vector<vector<double>> setDD = {{3.6, 1.0,0.5},{3.6, 1.0,0.5}};


/*problem and assignment*/
int numCs;
int numC0;
int numCi;
int numVs;
int numV0;
vector<int>* clauses;
vector<int>* posC;
vector<int>* negC;
int* posOc;
int* negOc;
int maxOcc;
vector<int> clauseT;
vector <bool> assignG;

bool sat = false;
bool inter = true;
int* vs;//0 for P0, 1 for P1, 2 for boundary of P0, 3 for boundary of P1.-1 for unused ones.
/*methods*/
uniform_int_distribution<int> distribution0;
uniform_int_distribution<int> distribution1;
minstd_rand generator0;
minstd_rand0 generator1;
void readPartition(string file);
void readFile(char* fileName);
void memAllocate(string buff);
void parseLine(string line, int index);
void initialAssignment();
void printVariables();
void printClauses();
void debugProblem();
int getFlipCandidate_wa(int cIndex);
int getFlipCandidate_max(int cIndex);
void printVector(vector<int>& vec);
void printUsage();
void test(char* fileName,bool f0, bool f1, bool fc,vector<bool>& assignment);
void testLine(string line,vector<bool>& assign);
bool satis();
int setConflict(vector<bool>& assign);

class Process{
	int seed;
	int* numP;
	vector<int> unsatCs;
	double* probs;
	double* lookUpTable;
	int* tabuS;
	int maxLOcc;
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
	bool f0;
	bool f1;
	bool c;


public:
	int tid;
	vector<bool> assign;
	Process(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD);
	void optimal(bool p0, bool p1, bool c);
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
	int randI0();
	int randI1();
	void pushBack(int cIndex);
	void combine0();
	void combine1();
	void combine();
	void combineR();
	void combineP();
};
#endif /* MAIN_H_ */
