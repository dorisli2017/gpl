/*
 * main.cpp
 *
 *  Created on: May 8, 2018
 *      Author: ping
 */

#include "gpl.h"
int main(int argc, char *argv[]){
	fileName = argv[1];
	part();
	readFile();
	#pragma omp parallel num_threads(2)
	{
		int tid = omp_get_thread_num();
		const vector<bool>setB = setBB[tid];
		const vector<int> setI =setII[tid];
		const vector<double>& setD = setDD[tid];
		Process process(setB, setI,setD);
		while(!satis()){
			#pragma omp barrier
			if(tid == 0){
				process.optimal(true, false, true);
			}
			if(tid == 1){
				process.optimal(false,true , true);
			}
			process.combineR();
			#pragma omp barrier
		}
		cout<< "SATIS";
		test(true, true, true, assignG);
		abort();
	}// parallel version
	return 0;
}


void debugProblem(){
	printVariables();
	printClauses();
	cout<<"vs:" <<endl;
	for(int i = 0; i < numVs; i++){
		cout<<i<<": "<<partition[i]<< " "<< vs[i]<<endl;
	}
	cout<< endl;
	cout<<"cs:" <<endl;
	for(int i =0; i < numCs; i++){
		cout<<i<<": "<< cs[i]<<endl;
	}
	cout<< "Occurences:"<< endl;
	for(int i = 1; i < numVs; i++){
		cout<< i<< ":"<<posOc[i]<< " "<<negOc[i]<< endl;
	}
}
void Process::debugAssign(){
	/* Testing code**********************************/
		printOptions();
	   	printAssignment();
	   	printUnsatCs();
	   	printNumP();


	/*Testing code**********************************/

}
Process::Process(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
	parseOptions(setB, setI,setD);
	tid = omp_get_thread_num();
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
	//set lookuptable
	switch (fct){
	case 0:initLookUpTable_poly();
			lookUp =&Process::LookUpTable_poly;
			break;
	default:initLookUpTable_exp();
			lookUp =&Process::LookUpTable_exp;
			break;
	}
}

/*parse the argument (including options and filename)
 *using getopt_long to allow GNU-style long options as well as single-character options
 */
void Process::parseOptions(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
	tabu_flag = setB[0];

	maxFlips =setI[0];
	maxSteps = setI[1];
	fct= setI[2];
	ict = setI[3];
	rct1 = setI[4];
	rct2 = setI[5];
	cct= setI[6];
	seed = setI[8];
	cb=setD[0];
	eps= setD[1];
	lct = setD[2];
}
// construct the Problem with fill information of the input file
void readFile(){
	//cout<< "in readFile"<<endl;
	ifstream fp;
	//fp.open("Debug/instance.cnf",std::ios::in);
	fp.open(fileName,std::ios::in);
	if(!fp.is_open()){
		perror("read file fails");
		exit(EXIT_FAILURE);
	}
	string buff;
	char head;
   	getline(fp,buff);
   	// Get the p line
   	while(!fp.eof()){
		//cout<<buff<<endl;
		//todo:parseLine
   		if(buff.empty()) break;
		head =buff.at(0);
		if(head == 'p'){
			memAllocate(buff);
			break;
		}
	  getline(fp,buff);
	}
   	// Get the clauses
   	int index = -1;
   	int line = 0;
   	while(!fp.eof() && line < numCs){
   		index++;
		getline(fp,buff);
		if(buff.empty()) break;
		parseLine(buff, index);
		line++;
   	}
   	initialAssignment();
};
void memAllocate(string buff){
	parseLine(buff,-1);
	clauses = new vector<int>[numCs];
	posC= new vector<int>[numVs];
	negC= new vector<int>[numVs];
	cs = (int*) malloc(sizeof(int) * numCs);
	vs = (int*) malloc(sizeof(int) * numVs);
	posOc = (int*) malloc(sizeof(int) * numVs);
	negOc = (int*) malloc(sizeof(int) * numVs);
	for(int i = 0; i < numVs; i++){
		posOc[i] = 0;
		vs[i] = -1;
	}
	for(int i = 0; i < numVs; i++){
		negOc[i] = 0;
	}
	clauseT.reserve(numVs);
	assignG.reserve(numVs);
}
void parseLine(string line,int indexC){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    if( indexC == -1){
    	strtok(str, s);
		strtok(NULL, s);
		numVs = atoi(strtok(NULL, s))+1;
		numCs = atoi(strtok(NULL, s));
		return;
    }// for the p line
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
			// Todo: not efficient !
			int j = partition[abs(clauseT[0])];
			for(int i =1; i < clauseT.size(); i++){
				if (j != partition[abs(clauseT[i])]){
					j=2;
					break;
				}
			}
			switch(j){
			case 0:{
				cs[indexC]= 0;
				for(int i=0; i < clauseT.size(); i++){
					if(vs[abs(clauseT[i])] == -1) vs[abs(clauseT[i])] = 0;
				}
				break;}
			case 1:{
				cs[indexC]= 1;
				for(int i=0; i < clauseT.size(); i++){
					if(vs[abs(clauseT[i])] == -1) vs[abs(clauseT[i])] = 1;
				}
				break;}
			case 2:{
				cs[indexC]= 2;
				for(int i=0; i < clauseT.size(); i++){

					if(partition[abs(clauseT[i])]== 0) vs[abs(clauseT[i])] = 2;
					else vs[abs(clauseT[i])] = 3;
				}
				break;}
			}
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
	cout<<"c maxSteps: "<<maxSteps<<endl;
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
   		printVector(clauses[i]);
   	}
}
void Process::printAssignment(){
	cout<< "v ";
	for(int i = 1; i < numVs; i++){
		if(assign[i]) cout <<i<<" ";
		else cout << -i<<" ";
	}
	cout <<endl ;
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
	for(int i = 0; i < numVs; i++){
			if(posOc[i] > negOc[i]){
				assignG.push_back(true);
			}
			else{
				assignG.push_back(false);
			}
	}
}
void Process::biasAssignment(){
	for(int i = 0; i < numVs; i++){
			if(posOc[i] > negOc[i]){
				assign[i] = true;
			}
			else{
				assign[i] = false;
			}
	}
	setAssignment();
}
void Process::randomBiasAssignment(){
	int sum;
	for(int i = 0; i < numVs; i++){
		sum = posOc[i] +negOc[i];
		if(sum == 0){
			assign[i] = true;
		}
		else{
			assign[i] = ((this->*randINT)()%sum)<posOc[i];
		}
	}
	setAssignment();
}
void Process::randomAssignment(){
   	for(int j = 0; j < numVs; j++){
   		assign[j] = ((this->*randINT)()%2 ==1);
   	}
    setAssignment();
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
		if(assign[j] == false){
	   		for (std::vector<int>::const_iterator i = negC[j].begin(); i != negC[j].end(); ++i){
	   			numP[*i]++;
	   		}
		}
		else{
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
void Process::optimal(bool p0, bool p1, bool pc){
	f0=p0;
	f1= p1;
	c = pc;
	int rct;
	for(int i; i < numVs;i++){
		assign[i] = assignG[i];
	}
	setAssignment();
	for(unsigned int i = 0; i < maxFlips; i++){
		for(unsigned int j = 0; j < maxSteps; j++){
			if (unsatCs.size()== 0){
				return;
			}
			search_prob();
		}
		rct = (this->*randINT)()%100;
		if(rct < rct1) randomAssignment();
		else{
			if(rct< rct2) biasAssignment();
			else randomBiasAssignment();
		}
	}
}
int Process::getFlipLiteral(int cIndex){
	vector<int>&  vList = clauses[cIndex];
	int j=0,bre,min= numCs+1;
	double sum=0,randD;
	int greedyLiteral = 0, randomLiteral;
	for (std::vector<int>::const_iterator i = vList.begin(); i != vList.end(); ++i){
		bre = computeBreakScore(*i);
		if(tabu_flag &&bre == 0 && tabuS[abs(*i)] == 0) return *i;
		if(bre < min){
			min = bre;
			greedyLiteral = *i;
		}
		if(bre < maxLOcc){
		sum+= lookUpTable[bre];
		}
		else{
		sum+=(this->*Process::lookUp)(bre);
		}
		probs[j]= sum;
		j++;
	}
	randD = ((double)(this->*randINT)()/RAND_MAX)*sum;
	assert(randD >= 0);
	for(int i = 0; i < j;i++){
		if(probs[i]< randD){
			continue;
		}
		randomLiteral= vList[i];
		break;
	}
	if(tabu_flag &&tabuS[abs(greedyLiteral)] < tabuS[abs(randomLiteral)]){
		return greedyLiteral;
	}
	return randomLiteral;
}
void Process::flip(int literal){
	std::vector<int>::const_iterator i;
	if(literal > 0){
   		for (i = negC[literal].begin(); i != negC[literal].end(); ++i){
   			numP[*i]--;
   			if(numP[*i] == 0) pushBack(*i);
   		}
		for (i = posC[literal].begin(); i != posC[literal].end(); ++i){
   			numP[*i]++;
		}

		assign[literal] = true;
	}
	else{
   		for (i = negC[-literal].begin(); i != negC[-literal].end(); ++i){
   			numP[*i]++;
   		}
		for (i = posC[-literal].begin(); i != posC[-literal].end(); ++i){
   			numP[*i]--;
   			if(numP[*i] == 0) pushBack(*i);
		}
		assign[-literal]= false;
	}
}
void test(bool f0, bool f1, bool fc,vector<bool>& assign){
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
		if(head == 'p'){
			break;
		}
	  getline(fp,buff);
	}
   	int i = 0;
   	while(!fp.eof()){
		getline(fp,buff);
		if(buff.empty()) break;
	switch(cs[i]){
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
void testLine(string line,vector<bool>& assign){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    int numT=0;
    char* token = strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			if(assign[-lit] == false) numT++;
			token = strtok(NULL, s);
			continue;
		}
		if(*token == '0'){
			if(numT == 0){
				cout<< fileName<<endl;
				perror("TEST FAILURE");
				exit(EXIT_FAILURE);
			}
		    return;
		}
		lit = atoi(token);
		if(assign[lit] == true) numT++;
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
	unsatCs[randC]=unsatCs.back();
	unsatCs.pop_back();
	flip(flipLindex);
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

void Process::initLookUpTable_exp(){

	lookUpTable = (double*)malloc(sizeof(double) * maxLOcc);
	for(int i = 0; i < maxLOcc;i++){
		lookUpTable[i] = pow(cb,-i);
	}
}
void Process::initLookUpTable_poly(){
	lookUpTable = (double*)malloc(sizeof(double) * maxLOcc);
	for(int i = 0; i < maxLOcc;i++){
		lookUpTable[i] = pow((eps+i),-cb);
	}
}
double Process::LookUpTable_exp(int bre){
	return pow(cb,-bre);
};
double Process::LookUpTable_poly(int bre){
	return pow((eps+bre),-cb);
};
int Process::randI0(){
	return distribution0(generator0);
};

int Process::randI1(){
	return distribution0(generator0);
};

void Process::pushBack(int cIndex){
	int p = cs[cIndex];
	switch(p){
	case 0:{if(f0)	unsatCs.push_back(cIndex);break;}
	case 1:{if(f1)	unsatCs.push_back(cIndex);break;}
	case 2:{if(c)	unsatCs.push_back(cIndex);break;}
	assert(false);
	}

}

bool satis(){
	int count = 0;
	for(int i =0; i < numCs; i++){
		for (std::vector<int>::const_iterator j = clauses[i].begin(); j != clauses[i].end(); ++j){
			if((*j) >0 && assignG[*j] == true) count++;
			else{
				if(*j <0 && assignG[-*j] == false) count++;
			}
		}
		if(count ==0) return false;
		count = 0;
	}
	return true;
}

int setConflict(vector<bool>& assign){
	int conflict = 0;
	int count = 0;
	for(int i =0; i < numCs; i++){
		for (std::vector<int>::const_iterator j = clauses[i].begin(); j != clauses[i].end(); ++j){
			if((*j) >0 && assign[*j] == true) count++;
			else{
				if(*j <0 && assign[-*j] == false) count++;
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
void readPartition(string file){
	//partition.reserve(10);
	partition.push_back(0);
	//cout<< "in readFile"<<endl;
	ifstream fp;
	int ind = 1;
	fp.open(file,std::ios::in);
	//fp.open(partFile,std::ios::in);
	if(!fp.is_open()){
		perror("read partition fails");
		exit(EXIT_FAILURE);
	}
	string buff;
   	getline(fp,buff);
   	while(!fp.eof()){
   		partition.push_back(stoi(buff));
   		ind++;
   	   	getline(fp,buff);

   	}
};
void  part()
{
  string fileConvert = "fileConverter/fileConvert ";
  fileConvert.append(string(fileName)).append(" p");
  int sr =system (fileConvert.c_str());
  if(sr == -1){
	perror("fileConvert fails");
	exit(EXIT_FAILURE);

  }

  string Kahypar = "../kahypar/build/kahypar/application/KaHyPar -h ";// other programm possible
  string block = " -k 2";// numThreads
  string imbalance = " -e 0.03";
  string object = " -o km1";// km1 :(lambda-1) metric , cut: cut-net metric
  string mode =" -m direct";// partition mode: direct: k-way, recursive: bisection recursive
  string preset0 = " -p ../kahypar/config/km1_direct_kway_sea18.ini";
  string preset1 = " -p ../../../config/km1_direct_kway_gecco18.ini";
  string preset2 = " -p ../../../config/km1_direct_kway_sea17.ini";
  string preset3 =" -p ../../../config/km1_direct_kway_alenex17.ini";
  string preset4 =" -p ../../../config/cut_rb_alenex16.ini";
  Kahypar.append(string( fileName).append(".p")).append(block).append(imbalance).append(object).append(mode).append(preset0).append("> /dev/null");
  sr = system (Kahypar.c_str());
  if(sr == -1){
	perror("Kahypar fails");
	exit(EXIT_FAILURE);

  }
  std::array<char, 128> buffer;
  std::string result;
  string a = "find ";
  a.append(string( fileName).append("*.KaHyPar"));
 // cout<< a;
  const char* cmd =a.c_str();
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
          result += buffer.data();
      }
  }
  	result.pop_back();
  	readPartition(result);
    a = "rm ";
    a.append(string( fileName)).append(".*");
    sr = system (a.c_str());
    if(sr == -1){
  	perror("Remove files fails");
  	exit(EXIT_FAILURE);

    }

}



