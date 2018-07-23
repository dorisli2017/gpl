/*
 * miscellaneous.cpp
 *
 *  Created on: Jul 19, 2018
 *      Author: ping
 */

#ifndef MISCELLANEOUS_CPP_
#define MISCELLANEOUS_CPP_
#include "miscellaneous.h"

void printClauses(){
	cout<< "Clauses "<< ": " << endl ;

   	for(int i = 0; i <c0; i++){
   		cout<< "Clause0 "<< i<< ": " ;
   		printVector(clauses0[i]);
   	}
   	for(int i = 0; i <c1; i++){
   		cout<< "Clause1 "<< i<< ": " ;
   		printVector(clauses1[i]);
   	}
}

void debugProblem(){
	printVector(partition);
	printVector(ind);
	printClauses();
	cout<< "Occurences:"<< endl;
	for(int i = 0; i < v0; i++){
		cout<< i<< ":"<<posOc0[i]<< " "<<negOc0[i]<< endl;
	}
	for(int i = 0; i <v1; i++){
		cout<< i<< ":"<<posOc1[i]<< " "<<negOc1[i]<< endl;
	}
	cout<<  endl;
}

/*print the manual of the program*/
void printUsage(){
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n--------localSAT, a stochastic local search solving SAT problem-----------------\n");
	printf("\nautor: Guangping Li\n");
	printf("references: Engineering a lightweight and efficient local search sat solver \n");

	printf("\n-------------------Usage--------------------------------------------------------\n");
	printf("\n");
	printf("./locolSAT  <DIMACS CNF instance> [options]\n");
	printf("\n--------------------------------------------------------------------------------\n");

	printf("\n");
	printf("\n-------------------Options------------------------------------------------------\n");
	printf("\n");
	printf("**for the potential flipping function:\n");
	printf("   --function, -f: 0 =  polynomial; 1 = exponential with make; 2 = exponential only with break: 3 = equal  [default = 0]\n");
	printf("   --eps, -e <double value> [0.0,1.0] :  [default = 1.0]\n");
	printf("   --cb, -b <double value>  [2.0,10.0] : constant for break\n");
	printf("   --cm, -m <double value>  [-2.0,2.0] : constant for make\n");
	printf("   --w, -w <double value>  [0.0,1.0] : [default = 0.3] the probability used only in the WALKSAT\n");
	printf("\n");
	printf("**for the process:\n");
	printf("   --caching, -c  : 0 =  no caching ; 1 = caching  for scores\n");
	printf("   --rounds, -r <long long int_value>,   : maximum number of tries \n");
	printf("   --flips, -p  <long long int_value>,   : maximum number of flips per try \n");
	printf("   --seed,"
			" -s : seed used for Randomness\n");
	printf("\n");
	printf("**further options:\n");
	printf("   --output, -o : output the final assignment\n");
	printf("   --help, -h : output this help\n");
	printf("\n");
	printf("---------------------------------------------------------------------------------\n");
}

void test( string inputFile,vector<bool>& assign){
	ifstream fp;
    int cut = 0;
	fp.open(inputFile,std::ios::in);
	if(!fp.is_open()){
		perror("read file fails");
		exit(EXIT_FAILURE);
	}
	string buff;
	char head;
   	getline(fp,buff);
   	while(!fp.eof()){
   		if(buff.empty()) break;
		head =buff.at(0);
		if(head == 'p'){
			break;
		}
	  getline(fp,buff);
	}
   	while(!fp.eof()){
		getline(fp,buff);
		if(buff.empty()) break;
		testLine(buff,assign, &cut);
   	}
   	assert(cut+c0+c1==numCsG);
   	assert(v0+v1== numVsG+1);
   	cout<< "tested" << endl;
}
void testLine(string line, vector<bool>& assign, int* cut){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    int numT=0;
    int q0=0;
    int q1=0;
    char* token = strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			if(assign[-lit] == false) numT++;
			if(partition[-lit] == 0) q0++;
			else q1++;
			token = strtok(NULL, s);
			continue;
		}
		if(*token == '0'){
			if(q0 >0 && q1>0){
				(*cut)++;
				 return;
			}
			if(numT == 0){
				perror("TEST FAILURE");
				exit(EXIT_FAILURE);
			}
			q0=0;
			q1=0;
		    return;
		}
		lit = atoi(token);
		if(partition[lit] == 0) q0++;
		else q1++;
		if(assign[lit] == true) numT++;
		token = strtok(NULL, s);
    }
	perror("a clause line does not terminates");
	exit(EXIT_FAILURE);
}


#endif /* MISCELLANEOUS_CPP_ */
