/*
* fileConvert.cpp
*
*  Created on: Jun 25, 2018
*      Author: ping
*/
#include "fileConvert.h"
int main(int argc, char *argv[]){
	fp.open(argv[1],std::ios::in);
	if(!fp.is_open()){
		perror("read file fails");
		exit(EXIT_FAILURE);
	}
	if(argc > 2){
		outFP.open((string(argv[1])+ p).c_str(),std:: ios::out);
		primary();
	}
	else{
		outFP.open((string(argv[1])+ d).c_str(),std:: ios::out);
		dual();
	}
	outFP.close();
	fp.close();

};

void primary(){
	if(!outFP.is_open()){
		perror("write file fails");
				exit(EXIT_FAILURE);
	}
	// strat read
	char head;
	getline(fp,buff);
	// Get the p line
	while(!fp.eof()){
		//cout<<buff<<endl;
		//todo:parseLine
		if(buff.empty()) break;
		head =buff.at(0);
		if(head == 'p'){
			char* str = strdup(buff.c_str());
	    	strtok(str, s);
			strtok(NULL, s);
			numVs = atoi(strtok(NULL, s));
			numCs = atoi(strtok(NULL, s));
			outFP<< numCs << ' '<< numVs<< endl;
			break;
		}
	  getline(fp,buff);
	}
	// Get the clause
	int line = 0;
	while(!fp.eof() && line < numCs){
		getline(fp,buff);
		if(buff.empty()) break;
		char* str = strdup(buff.c_str());
		int lit;
		bool flag;
		char* token = strtok(str, s);
		 while(token != NULL){
			flag = false;
			if(*token== '-'){
				lit = -atoi(token);
				outFP<<lit << ' ';
				token = strtok(NULL, s);
				continue;
			}
			if(*token == '0'){
				outFP<<endl;
				flag = true;
				line++;
				break;
			}
			if(flag) break;
			lit = atoi(token);
			outFP<<lit << ' ';
			token = strtok(NULL, s);
		 }
	}
}

void dual(){
	if(!outFP.is_open()){
			perror("write file fails");
					exit(EXIT_FAILURE);
		}
	char head;
	   	getline(fp,buff);
	   	// Get the p line
	   	while(!fp.eof()){
			//cout<<buff<<endl;
			//todo:parseLine
	   		if(buff.empty()) break;
			head =buff.at(0);
			if(head == 'p'){
				char* str = strdup(buff.c_str());
		    	strtok(str, s);
				strtok(NULL, s);
				numVs = atoi(strtok(NULL, s));
				numCs = atoi(strtok(NULL, s));
				Cs = new vector<int>[numVs+1];
				break;
			}
		  getline(fp,buff);
		}

	   	// Get the clauses
	   	int index = 0;
	   	int line = 0;
	   	while(!fp.eof() && line < numCs){
			getline(fp,buff);
			if(buff.empty()) break;
			parseLine(buff, index);
			index++;
			line++;
	   	}
	   	initialAssignment();
}

void parseLine(string line,int indexC){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    char* token = strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			Cs[-lit].push_back(indexC);
			token = strtok(NULL, s);
			continue;
		}
		if(*token == '0'){
		    return;
		}
		lit = atoi(token);
		Cs[lit].push_back(indexC);
		token = strtok(NULL, s);
    }
	perror("a clause line does not terminates");
	exit(EXIT_FAILURE);
}

void initialAssignment(){
	int size = 0;
	for(int j = 0; j < numVs+1; j++){
		if(Cs[j].size() == 0) continue;
		size++;
	}
	outFP<< size << ' '<< numCs<< endl;
	for(int j = 0; j < numVs+1; j++){
		if(Cs[j].size() == 0) continue;
		for (std::vector<int>::const_iterator i = Cs[j].begin(); i != Cs[j].end(); ++i){
			outFP<<*i << " ";
		}
		outFP<<endl;
	}
}
