/*
 * miscellaneous.h
 *
 *  Created on: Jul 19, 2018
 *      Author: ping
 */

#ifndef MISCELLANEOUS_H_
#define MISCELLANEOUS_H_
#include "problem.h"
void printUsage();
void debugProblem();
void printVariables();
void printClauses();
void test( string inputFile,vector<bool>& assign);
void testLine(string line, vector<bool>& assign, int* cut);



#endif /* MISCELLANEOUS_H_ */
