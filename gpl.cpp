/*
 * main.cpp
 *
 *  Created on: May 8, 2018
 *      Author: ping
 */

#include "gpl.h"
int main(int argc, char *argv[]){
	readFile(argv[1]);
	//debugProblem();
	//Process process0(setBB[0], setII[0],setDD[0]);
	Process process1(setBB[1], setII[1],setDD[1]);
	//process0.setJob(true, false, false);
	//process0.setAssignment();
	process1.setJob(false,true,false);
	process1.setAssignment();
	//while(true){
		//process0.tid = 0;
		//process0.optimal();
		process1.tid = 1;
		process1.optimal();
		/*for(int i =0; i < numVs; i++){
			if(vs[i] == 0 || vs[i] == 1) assert(assignG[i] != 1);
		}
		for(int i =0; i < numVs; i++){
			if(vs[i] == 0){assignG[i]=process0.assign[i]; continue;}
			if(vs[i] == 1){assignG[i]=process1.assign[i]; continue;}
			assignG[i]+= process0.assign[i];
			assignG[i]+= process1.assign[i];
		}
		assert(assignG.size() == numVs);
		if(satis()) break;
		process0.combineAssign();
		process1.combineAssign();
	}*/
	cout<< "SATIS"<<endl;

	/*if(inter){
		for(int i = 0;i < assignG.size(); i++){
			cout<< assignG[i]<< " ";
		}
	}
	else{
		for(int i = 0;i < process0.assign.size(); i++){
			cout<< process0.assign[i]<< " ";
		}
	}*/
	if(inter )test(argv[1],true, false, false, process1.assign);
	if(!inter)test(argv[1],false, false, true, process1.assign);
	return 0;
}


void debugProblem(){
	printVariables();
	printClauses();
	cout<<"vs:" <<endl;
	for(int i = 0; i < numVs; i++){
		cout<<i<<": "<< " "<< vs[i]<<endl;
	}
	cout<< endl;
	cout<< "Occurences:"<< endl;
	for(int i = 1; i < numVs; i++){
		cout<< i<< ":"<<posOc[i]<< " "<<negOc[i]<< endl;
	}
}
void Process::debugAssign(){
	/* Testing code**********************************/
		//printOptions();
	   	printAssignment();
	   	printUnsatCs();
	   	printNumP();
	   	cout<< "flips: "<< flips<<endl;


	/*Testing code**********************************/

}
Process::Process(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
	parseOptions(setB, setI,setD);
	//tid = omp_get_thread_num();
	if(tid == 0){
		distribution0 = uniform_int_distribution<int>(0,INT_MAX);
		generator0.seed(seed);
		randINT = &Process::randI0;
	}
	else {
		distribution1 = uniform_int_distribution<int>(0,INT_MAX);
		generator1.seed(seed);
		randINT = &Process::randI1;
	}
	//set the parameters
	   // set tabuS
	if(tabu_flag){
		tabuS = (int*) malloc(sizeof(int) * numVs);
		for(int i = 0; i < numVs; i++){
			tabuS[i] = 0;
		}
	}
	maxLOcc = maxOcc*lct;
	numP = (int*) malloc(sizeof(int) * numCs);
	probs = (double*)malloc(sizeof(double) * numVs);
	assign.reserve(numVs);
	lookUpTable = (double*)malloc(sizeof(double) * maxLOcc);
	lookUpTableI = (double*)malloc(sizeof(double) * maxLOcc);
	lookUpTableO = (double*)malloc(sizeof(double) * maxLOcc);
	//set lookuptable
	switch (fct){
	case 0:initLookUpTable_poly(0);
			lookUp =&Process::LookUpTable_poly;
			break;
	default:initLookUpTable_exp(0);
			lookUp =&Process::LookUpTable_exp;
			break;
	}
	switch (fctI){
	case 0:initLookUpTable_poly(1);
			lookUpI =&Process::LookUpTable_poly;
			break;
	default:initLookUpTable_exp(1);
			lookUpI =&Process::LookUpTable_exp;
			break;
	}
	switch (fctO){
	case 0:initLookUpTable_poly(2);
			lookUpO =&Process::LookUpTable_poly;
			break;
	default:initLookUpTable_exp(2);
			lookUpO =&Process::LookUpTable_exp;
			break;
	}
}

/*parse the argument (including options and filename)
 *using getopt_long to allow GNU-style long options as well as single-character options
 */
void Process::parseOptions(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
	tabu_flag = setB[0];

	maxFlips =setI[0];
	fct= setI[1];
	ict = setI[2];
	rct1 = setI[3];
	rct2 = setI[4];
	cct= setI[5];
	seed = setI[7];
	cb = setD[0];
	cbI= setD[1];
	cbO = setD[2];
	eps= setD[3];
	epsI= setD[4];
	epsO= setD[5];
	lct = setD[6];
}
void readFile(char* fileName){
	ifstream fp;
	fp.open(fileName,std::ios::in);
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
			memAllocate(buff);
			break;
		}
	  getline(fp,buff);
	}
	if(inter){
		while(!fp.eof()){
			if(buff.empty()) break;
			head =buff.at(0);
			if(head == 'i'){
				char* str = strdup(buff.c_str());
					const char s[2] = " ";
					char* token = strtok(str, s);
					token = strtok(NULL, s);
					int vIndex;
					while(token != NULL){
						vIndex = atoi(token);
						if(vIndex < numV0) vs[vIndex] = 2;
						else  vs[vIndex] = 3;
						token = strtok(NULL, s);
					}
				break;
			}
		  getline(fp,buff);
		}
	}
   	int index = -1;
   	int line = 0;
   	while(!fp.eof() && line < numCs){
   		index++;
		getline(fp,buff);
		if(buff.empty()) break;
		parseLine(buff, index);
		line++;
   	}
   	assert(line == numCs);
   	initialAssignment();
}
void memAllocate(string buff){
	char* str = strdup(buff.c_str());
    const char s[2] = " ";
	strtok(str, s);
	if(inter){
		numVs = atoi(strtok(NULL, s))+1;
		numV0 = atoi(strtok(NULL, s))+1;
		numCs = atoi(strtok(NULL, s));
		numC0 = atoi(strtok(NULL, s));
		numCi = atoi(strtok(NULL, s));
	}
	else{
		strtok(NULL, s);
		numVs = atoi(strtok(NULL, s))+1;
		//numV0 = numVs;
		numV0 = 1;
		numCs = atoi(strtok(NULL, s));
		numC0 = 0;
		numCi = 0;
	}
	clauses = new vector<int>[numCs];
	posC= new vector<int>[numVs];
	negC= new vector<int>[numVs];
	vs = (int*) malloc(sizeof(int) * numVs);
	posOc = (int*) malloc(sizeof(int) * numVs);
	negOc = (int*) malloc(sizeof(int) * numVs);
	for(int i = 0; i < numVs; i++){
		posOc[i] = 0;
	}
	for(int i = 0; i < numVs; i++){
		negOc[i] = 0;
	}
	for(int i = 0; i < numV0; i++){
		vs[i] = 0;
	}
	for(int i = numV0; i < numVs; i++){
		vs[i] = 1;
	}
	clauseT.reserve(numVs);
	assignG.reserve(numVs);
	for(int i =0; i < numVs; i++){
		assignG.push_back(0);
	}
	assert(assignG.size() == numVs);
}
void parseLine(string line,int indexC){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    int size;
    char* token = strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			clauseT.push_back(lit);
		    negOc[-lit]++;
			token = strtok(NULL, s);
			size++;
			continue;
		}
		if(*token == '0'){
			clauses[indexC] = clauseT;
			clauseT.clear();
		    return;
		}
		lit = atoi(token);
		clauseT.push_back(lit);
	    posOc[lit]++;
	    size++;
		token = strtok(NULL, s);
    }
	perror("a clause line does not terminates");
	exit(EXIT_FAILURE);
}
void Process::printOptions(){
	printf("localSAT options: \n");
	cout<<"c tabu_flag: "<<tabu_flag<<endl;
	cout<<"c maxFlips: "<<maxFlips<<endl;
	cout<<"c seed: "<<seed<<endl;
	cout<<"c fct: "<<fct<<endl;
	cout<<"c ict: "<<ict<<endl;
	cout<<"c rct1: "<<rct1<<endl;
	cout<<"c rct2: "<<rct2<<endl;
	cout<<"c cct: "<<cct<<endl;
	cout<<"c cb: "<<cb<<endl;
	cout<<"c eps: "<<eps<<endl;
	cout<<"c lct: "<<lct<<endl;
	switch(fct){
	case 0:{
		cout<<"c polynomial function"<<endl;
		cout<<"c eps: "<<eps<<endl;
		cout<<"c cb: "<<cb<<endl;
		cout<< "pow((eps+break),-cb)" << endl;
		break;
		  }
	default:{
		cout<<"c exponential function"<<endl;
		cout<<"c cb: "<<cb<<endl;
		cout<< "pow(cb,-break)"<< endl;
		break;
		   }
	}
}
void printVariables(){
	cout<< "Variables "<< ": " <<endl ;
   	for(int i = 1; i < numVs; i++){
   		cout<< "Variable "<<i << ": " <<endl ;
   		printVector(posC[i]);
   		printVector(negC[i]);
   	}
}
void printClauses(){

	cout<< "Clauses "<< ": " << endl ;
   	for(int i = 0; i < numCs; i++){
   		cout<< "Clause "<< i<< ": " ;
   		assert(clauses[i].size() > 0);
   		printVector(clauses[i]);
   	}
}
void Process::printAssignment(){
	cout<< "v ";
	for(int i = 1; i < numVs; i++){
		if(assign[i] >= 2){cout <<i<<" ";continue;}
		if(assign[i] <= -1){ cout <<-i<<" ";continue;}
		if(assign[i] == 0){ cout << "*"<< i<<" ";continue;}
		assert(false);
	}
	cout <<endl;
}
void Process::printUnsatCs(){
	cout<< "Unsatisfied clauses ";
	printVector(unsatCs);
	cout <<endl ;
}
void Process::printNumP(){
	cout<< "numP: ";
	for(int i = 0; i < numCs; i++){
		cout << numP[i]<< " ";
	}
	cout<<endl;
}




void initialAssignment(){
	for(int i = 0; i < numVs; i++){
		if(posOc[i]> maxOcc) maxOcc = posOc[i];
		if(negOc[i]> maxOcc) maxOcc = negOc[i];
		posC[i].reserve(posOc[i]);
		negC[i].reserve(negOc[i]);
	}
	for(int j = 0; j < numCs; j++){
		for (std::vector<int>::const_iterator i = clauses[j].begin(); i != clauses[j].end(); ++i){
			if(*i < 0) negC[-(*i)].push_back(j);
			else  posC[(*i)].push_back(j);
		}
	}
	assignG.reserve(numVs);
}
void Process::setAssignment(){
   	for(int i = 0; i < numCs; i++){
   		numP[i] = 0;
   	}
	if( tabu_flag && (this->*randINT)()%100<cct){
		for(int i = 0; i < numVs; i++){
			tabuS[i] =0;
		}
	}
   	for(int j = 0; j < numVs; j++){
		if(assign[j] == -1){
	   		for (std::vector<int>::const_iterator i = negC[j].begin(); i != negC[j].end(); ++i){
	   			numP[*i]++;
	   		}
		}
		if(assign[j] == 2){
			for (std::vector<int>::const_iterator i = posC[j].begin(); i != posC[j].end(); ++i){
	   			numP[*i]++;
			}
   		}
   	}
   	for(int i = 0; i < numCs; i++){
   		if(numP[i] == 0){
   			pushBack(i);
   		}
   	}
}
void Process::setJob(bool p0, bool p1, bool pc){
	f0=p0;
	f1= p1;
	c = pc;
	if(f0){
		p = 0;
		inn = 2;
		out = 3;
		for(int i = 0; i < numV0; i++){
			if(vs[i] == 2){
				assign.push_back(0);
				continue;
			}
			if(posOc[i] > negOc[i]){
				assign.push_back(2);
			}
			else{
				assign.push_back(-1);
			}
		}
		for(int i = numV0; i < numVs; i++){
				assign.push_back(0);
		}
	}
	else{
		p = 1;
		inn = 3;
		out = 2;
		for(int i = 0; i < numV0; i++){
				assign.push_back(0);
		}
		for(int i = numV0; i < numVs; i++){
			if(vs[i] == 3){
				assign.push_back(0);
				continue;
			}
			if(posOc[i] > negOc[i]){
				assign.push_back(2);
			}
			else{
				assign.push_back(-1);
			}

		}

	}
}
void Process::optimal(){
	for(unsigned int i = 0; i < maxFlips; i++){
		if (unsatCs.size()== 0){
				return;
		}
		search_prob();
	}
}
int Process::getFlipLiteral(int cIndex){
	assert(clauses[cIndex].size()> 0);
	vector<int>&  vList = clauses[cIndex];
	int j=0,bre,min= numCs+1,pat;
	double sum=0,randD;
	int greedyLiteral = 0, randomLiteral;
	for (std::vector<int>::const_iterator i = vList.begin(); i != vList.end(); ++i){
		bre = computeBreakScore(*i);
		if(tabu_flag &&bre == 0 && tabuS[abs(*i)] == 0) return *i;
		if(bre < min){
			min = bre;
			greedyLiteral = *i;
		}
		pat = vs[abs(*i)];
		assert(p == 1);
		if(bre < maxLOcc){
			if(pat== p) sum+= lookUpTable[bre];
			else{
				if(pat== inn) sum+= lookUpTableI[bre];
				else sum+= lookUpTableO[bre];
			}
		}
		else{
			if(pat== p) sum+=(this->*Process::lookUp)(bre);
			else{
				if(pat== inn) sum+=(this->*Process::lookUpI)(bre);
				else sum+=(this->*Process::lookUpO)(bre);
			}
		}
		probs[j]= sum;
		j++;
	}
	randD = ((double)(this->*randINT)()/RAND_MAX)*sum;
	assert(randD >= 0);
	assert(randD <= probs[j-1]);
	assert(j < numVs);
	int i;
	for(i = 0; i < j;i++){
		if(probs[i]< randD){
			assert(i < j-1);
			continue;
		}
		randomLiteral= vList[i];
		break;
	}
	assert(abs(randomLiteral) < numVs);
	if(tabu_flag &&tabuS[abs(greedyLiteral)] < tabuS[abs(randomLiteral)]){
		return greedyLiteral;
	}
	return randomLiteral;
}
void Process::flip(int literal){
	std::vector<int>::const_iterator i;
	if(literal > 0){
		for (i = posC[literal].begin(); i != posC[literal].end(); ++i){
			assert(*i< numCs);
   			numP[*i]++;
		}
		if(assign[literal] == -1){
			for (i = negC[literal].begin(); i != negC[literal].end(); ++i){
				numP[*i]--;
				if(numP[*i] == 0) pushBack(*i);
			}
		}
		assign[literal] =2;

	}
	else{
   		for (i = negC[-literal].begin(); i != negC[-literal].end(); ++i){
   			numP[*i]++;
   		}
   		if(assign[-literal] == 2){
   			for (i = posC[-literal].begin(); i != posC[-literal].end(); ++i){
   				numP[*i]--;
   				if(numP[*i] == 0) pushBack(*i);
   			}
   		}
		assign[-literal]= -1;
	}
}
void test(char* fileName,bool f0, bool f1, bool fc,vector<int>& assign){
#pragma omp critical
{
	ifstream fp;
	fp.open(fileName,std::ios::in);
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
		if(head == 'i'){
			break;
		}
	  getline(fp,buff);
	}
   	int i = 0;
   	while(!fp.eof()){
		getline(fp,buff);
		if(buff.empty()) break;
		int cs;
		if (i < numC0) cs = 0;
		else{
			if(i < numCi) cs = 2;
			else cs =1;
		}
		switch(cs){
		case 0:{if(f0)	testLine(buff, assign);break;}
		case 1:{if(f1)	testLine(buff, assign);break;}
		case 2:{if(fc)	testLine(buff, assign);break;}
		assert(false);
		}
		i++;
   	}
   	cout<< "tested" << endl;
}
}
void testLine(string line,vector<int>& assign){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    int numT=0;
    char* token = strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			if(assign[-lit] <= -1) numT++;
			token = strtok(NULL, s);
			continue;
		}
		if(*token == '0'){
			if(numT == 0){
				cout<< endl<<line;
				perror("TEST FAILURE");
				exit(EXIT_FAILURE);
			}
		    return;
		}
		lit = atoi(token);
		if(assign[lit] >=2) numT++;
		token = strtok(NULL, s);
    }
	perror("a clause line does not terminates");
	exit(EXIT_FAILURE);
}
int Process::computeBreakScore(int literal){
    int score = 0;
    int aIndex = abs(literal);
    vector<int>& occList =(literal < 0)? posC[aIndex] :negC[aIndex];
    for(std::vector<int>::const_iterator i = occList.begin(); i != occList.end(); ++i) {
        if (numP[*i]== 1) {
            score++;
        }
    }
	//cout<< "out make "<<endl;
    return score;
}
double Process::func_exp(int literal){
	return pow(cb,-computeBreakScore(literal));
}
double Process::func_poly(int literal){
	return pow((eps+computeBreakScore(literal)),-cb);
}
void Process::search_prob(){
	int randC = (this->*randINT)()%unsatCs.size();
	int flipCindex = unsatCs[randC];
	if(numP[flipCindex] > 0){
		unsatCs[randC]=unsatCs.back();
		unsatCs.pop_back();
		return;
	}
	int flipLindex = getFlipLiteral(flipCindex);
	assert(abs(flipLindex) < numVs);
	unsatCs[randC]=unsatCs.back();
	unsatCs.pop_back();
	//cout<< "A"<<endl;
	flip(flipLindex);
	//cout<< "B"<<endl;
	flips++;
	if(tabu_flag) tabuS[abs(flipLindex)]++;
}


void printVector(vector<int>& vec){
	for (std::vector<int>::const_iterator i = vec.begin(); i != vec.end(); ++i){
		cout << *i << ' ';
	}
	cout << endl;
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
	printf("   --seed, -s : seed used for Randomness\n");
	printf("\n");
	printf("**further options:\n");
	printf("   --output, -o : output the final assignment\n");
	printf("   --help, -h : output this help\n");
	printf("\n");
	printf("---------------------------------------------------------------------------------\n");
}

void Process::initLookUpTable_exp(int i){
	double* l;
	double cbX;
	switch(i){
	case 0:l = lookUpTable;cbX = cb; break;
	case 1:l = lookUpTableI;cbX = cbI; break;
	case 2:l = lookUpTableO;cbX= cbO; break;
	}
	for(int i = 0; i < maxLOcc;i++){
		l[i] = pow(cbX,-i);
	}
}
void Process::initLookUpTable_poly(int i){
	double* l;
	double cbx;
	double epsX;
	switch(i){
	case 0:l = lookUpTable;cbx = cb;epsX = eps; break;
	case 1:l = lookUpTableI;cbx = cbI;epsX = epsI;break;
	case 2:l = lookUpTableO;cbx= cbO; epsX = epsO;break;
	}
	for(int i = 0; i < maxLOcc;i++){
		l[i] = pow((epsX+i),-cbx);
	}
}
double Process::LookUpTable_exp(int bre){
	return pow(cb,-bre);
};
double Process::LookUpTable_poly(int bre){
	return pow((eps+bre),-cb);
};
double Process::LookUpTable_expI(int bre){
	return pow(cbI,-bre);
};
double Process::LookUpTable_polyI(int bre){
	return pow((epsI+bre),-cbI);
};
double Process::LookUpTable_expO(int bre){
	return pow(cbO,-bre);
};
double Process::LookUpTable_polyO(int bre){
	return pow((epsO+bre),-cbO);
};
int Process::randI0(){
	//return distribution0(generator0);
	return rand();
};

int Process::randI1(){
	//return distribution0(generator0);
	return rand();
};

void Process::pushBack(int cIndex){
	int pat;
	if (cIndex < numC0) pat = 0;
	else{
		if(cIndex < numCi) pat = 2;
		else pat =1;
	}
	switch(pat){
	case 0:{if(f0)	unsatCs.push_back(cIndex);break;}
	case 1:{if(f1)	unsatCs.push_back(cIndex);break;}
	case 2:{if(c)	unsatCs.push_back(cIndex);break;}
	assert(false);
	}

}

bool satis(){
	for(int i =0; i < numVs; i++){
		if(assignG[i] == 1) return false;
	}
	return true;
}

int setConflict(vector<int>& assign){
	int conflict = 0;
	int count = 0;
	for(int i =0; i < numCs; i++){
		for (std::vector<int>::const_iterator j = clauses[i].begin(); j != clauses[i].end(); ++j){
			if((*j) >0 && assign[*j] == 1) count++;
			else{
				if(*j <0 && assign[-*j] == 0) count++;
			}
		}
		if(count ==0) conflict++;
		count = 0;
	}
	return conflict;
}
// prefer thread 0
void Process::combine0(){
	if(tid == 0){
		for(int i =0; i < numVs; i++){
			if(vs[i] !=1 ) assignG[i] = assign[i];
		}
	}
	else{
		for(int i =0; i < numVs; i++){
			if(vs[i] ==1 ) assignG[i] = assign[i];
		}
	}
}
// prefer thread 1
void Process::combine1(){
	if(tid == 1){
		for(int i =0; i < numVs; i++){
			if(vs[i] !=0 ) assignG[i] = assign[i];
		}
	}
	else{
		for(int i =0; i < numVs; i++){
			if(vs[i] ==0) assignG[i] = assign[i];
		}
	}
}

// prefer the better one
void Process::combine(){
	//if(conflict0 > conflict1) combine1();
	//else combine0();
	//combine0();
#pragma omp critical
	{
	for(int i =0; i < numVs; i++){
		 assignG[i] = assign[i];
	}
	}
}

// no critical
void Process::combineR(){
	if(tid == 0) combine0();
	else combine1();
}
// o thread fill 0 and 2 vertices. 1. thread fill 1 and 3 vertices
void Process::combineP(){
	if(tid == 0){
		for(int i =0; i < numVs; i++){
			if(vs[i]==0 || vs[i]==2) assignG[i] = assign[i];
		}
	}
	else{
		for(int i =0; i < numVs; i++){
			if(vs[i]==1 || vs[i]==3 ) assignG[i] = assign[i];
		}
	}
}

void Process::combineAssign(){
	int v;
	for(int i =0; i < numVs; i++){
		v = assignG[i];
		if(v >= 2){ assign[i] = 2;continue;}
		if(v <= -1){ assign[i] = -1;continue;}
		if(v == 0){ assign[i] = 0; continue;}
		if(v == 1){
			if(vs[i] == inn)continue;
			if(vs[i] == out){
				if(assign[i] == -1){ assign[i] = 2; continue;};
				if(assign[i] == 2){ assign[i] = -1; continue;};
				continue;
			}
			cout<< vs[i];
			assert(false);
		}
		assert(false);
	}
	setAssignment();
}


