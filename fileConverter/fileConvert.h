/*
 * fileConvert.h
 *
 *  Created on: Jun 25, 2018
 *      Author: ping
 */

#ifndef FILECONVERTER_FILECONVERT_H_
#define FILECONVERTER_FILECONVERT_H_

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <cstdlib>
#include <string.h>
#include <vector>
using namespace std;
ifstream fp;
ofstream outFP;
const char s[2] = " ";
void primary();
string buff;
char head[] = "p";
string p = ".p";
string d = ".d";
int numCs;
int numVs;
void dual();
vector<int>* Cs;
void parseLine(string line,int indexC);
void initialAssignment();
void memAllocate(string buff);





#endif /* FILECONVERTER_FILECONVERT_H_ */
