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
/* system example : DIR */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <string>
#include <iostream>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <iostream>
#include <cstring>
#include <string>
#include <cstring>
#include <iostream>
#include <cstring>
#include <string>
using namespace std;

/*problem and assignment*/
extern char* inputFile;
extern int numCsG;
extern int numVsG;
extern vector<int>* clauses0;
extern vector<int>* clauses1;
extern int* posOc0;
extern int* negOc0;
extern int* posOc1;
extern int* negOc1;
extern int maxOcc;
extern vector<int> clauseT0;
extern vector<int> clauseT1;
extern bool sat;
extern int c0,c1,v0,v1;
extern vector<bool> assignG;
extern vector<int> partition;
extern vector<int> ind;
/*methods*/

void readFile();
string part();
void readPartition(string file);
void memAllocate(string buff);
void parseLine(string line);
void initialAssignment();
int getFlipCandidate_wa(int cIndex);
int getFlipCandidate_max(int cIndex);
void printVector(vector<int>& vec);

