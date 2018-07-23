/*
 * main.cpp
 *
 *  Created on: May 8, 2018
 *      Author: ping
 */


#include "process.h"
#include "miscellaneous.h"

template<class T>
void Process<T>::debugAssign(){
	/* Testing code**********************************/
		//printOptions();
	   	printAssignment();
	   	printUnsatCs();
	   	printNumP();


	/*Testing code**********************************/

}
template<class T>
Process<T>::Process(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
	parseOptions(setB, setI,setD);
	if(omp_get_thread_num()== 0){
		clauses = clauses0;
		posOc = posOc0;
		negOc = negOc0;
		numCs = c0;
		numVs = v0;
	}
	else{
		clauses = clauses1;
		posOc = posOc1;
		negOc = negOc1;
		numCs = c1;
		numVs = v1;
	}
	distribution = uniform_int_distribution<int>(0,INT_MAX);
	//set the parameters
	   // set tabuS
	if(setI[7] == 9){
		srand(seed);
		randINT = &Process::randI2;
	}
	else {
		randINT = &Process::randI;
		generator.seed(seed);
	}
	if(tabu_flag){
		tabuS = (int*) malloc(sizeof(int) * numVs);
		for(int i = 0; i < numVs; i++){
			tabuS[i] = 0;
		}
	}
	maxLOcc = maxOcc*lct;
	numP = (int*) malloc(sizeof(int) * numCs);
	probs = (double*)malloc(sizeof(double) * numVs);
	assign = (bool*)malloc(sizeof(bool) * numVs);
	//set inititial assignment
   	initialAssignment();
	switch(ict){
	case 0:randomAssignment();break;
	case 1:biasAssignment();break;
	default: randomBiasAssignment();
	}
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
template<class T>
void Process<T>::parseOptions(const vector<bool>& setB, const vector<int>& setI,const vector<double>& setD){
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

template<class T>
void Process<T>::printAssignment(){
	cout<< "v ";
	for(int i = 0; i < numVs; i++){
		if(assign[i]) cout <<i<<" ";
		else cout << -i<<" ";
	}
	cout <<endl ;
}
template<class T>
void Process<T>::printUnsatCs(){
	cout<< "Unsatisfied clauses ";
	printVector(unsatCs);
	cout <<endl ;
}
template<class T>
void Process<T>::printNumP(){
	cout<< "numP: ";
	for(int i = 0; i < numCs; i++){
		cout << numP[i]<< " ";
	}
	cout<<endl;
}

template<class T>
void Process<T>::biasAssignment(){
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
template<class T>
void Process<T>::randomBiasAssignment(){
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
template<class T>
void Process<T>::randomAssignment(){
   	for(int j = 0; j < numVs; j++){
   		assign[j] = ((this->*randINT)()%2 ==1);
   	}
    setAssignment();
}
template<class T>
void Process<T>:: initialAssignment(){
	posC= new vector<int>[numVs];
	negC= new vector<int>[numVs];
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

}
template<class T>
void Process<T>::setAssignment(){
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
   			unsatCs.push_back(i);
   		}
   	}
}

template<class T>
void Process<T>::optimal(){
	cout<< "start optimal";
	int rct;
	while(true){
		for(int i = 0; i < maxFlips; i++){
			for(int j = 0; j < maxSteps; j++){
				//cout<<unsatCs.size()<<endl;
				if (unsatCs.size()== 0){
					cout<< "s SATISFIABLE"<< endl;
					//debugAssign();
					//printAssignment();
					//int index =0;
					for(int i = 0; i < numVsG; i++){
						if(partition[i] == omp_get_thread_num()){
							assignG[i] = assign[ind[i]];
						}

					}
					return;
				}
				search_prob();
			}
			//if(sat) return;
		}
		rct = (this->*randINT)()%100;
		if(rct < rct1) randomAssignment();
		else{
			if(rct< rct2) biasAssignment();
			else randomBiasAssignment();
		}
	}
}
template<class T>
int Process<T>::getFlipLiteral(int cIndex){
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
template<class T>
void Process<T>::flip(int literal){
	std::vector<int>::const_iterator i;
	if(literal > 0){
   		for (i = negC[literal].begin(); i != negC[literal].end(); ++i){
   			numP[*i]--;
   			if(numP[*i] == 0) unsatCs.push_back(*i);
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
   			if(numP[*i] == 0) unsatCs.push_back(*i);
		}
		assign[-literal]= false;
	}
}
template<class T>
int Process<T>::computeBreakScore(int literal){
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
template<class T>
double Process<T>::func_exp(int literal){
	return pow(cb,-computeBreakScore(literal));
}
template<class T>
double Process<T>::func_poly(int literal){
	return pow((eps+computeBreakScore(literal)),-cb);
}
template<class T>
void Process<T>::search_prob(){
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



template<class T>
void Process<T>::initLookUpTable_exp(){

	lookUpTable = (double*)malloc(sizeof(double) * maxLOcc);
	for(int i = 0; i < maxLOcc;i++){
		lookUpTable[i] = pow(cb,-i);
	}
}
template<class T>
void Process<T>::initLookUpTable_poly(){
	lookUpTable = (double*)malloc(sizeof(double) * maxLOcc);
	for(int i = 0; i < maxLOcc;i++){
		lookUpTable[i] = pow((eps+i),-cb);
	}
}
template<class T>
double Process<T>::LookUpTable_exp(int bre){
	return pow(cb,-bre);
};
template<class T>
double Process<T>::LookUpTable_poly(int bre){
	return pow((eps+bre),-cb);
};
template<class T>
int Process<T>::randI(){
	return distribution(generator);
};

template<class T>
int Process<T>::randI2(){
	return rand();
};

template<class T>
void Process<T>::printOptions(){
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
template<class T>
void Process<T>:: printVariables(){
	cout<< "Variables "<< ": " <<endl ;
   	for(int i = 0; i < numVs; i++){
   		cout<< "Variable "<<i << ": " <<endl ;
   		printVector(posC[i]);
   		printVector(negC[i]);
   	}
}
template class Process<minstd_rand0>;
template class Process<minstd_rand>;
template class Process<mt19937_64>;
template class Process<ranlux24_base>;
template class Process<ranlux48_base>;
template class Process<ranlux24>;
template class Process<ranlux48>;
template class Process<knuth_b>;
template class Process<mt19937>;


