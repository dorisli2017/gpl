#include "problem.h"
bool sat = false;
int numVsG;
char* inputFile;
int numCsG;
vector<int>* clauses0;
vector<int>* clauses1;
int* posOc0;
int* negOc0;
int* posOc1;
int* negOc1;
int maxOcc;
vector<int> partition;
vector<int> ind;
vector<int> clauseT0;
vector<int> clauseT1;
int c0=0;
int c1=0;
int v0=0;
int v1=0;
vector<bool> assignG;
// construct the Problem with fill information of the input file
void readFile(){
	//cout<< "in readFile"<<endl;
	ifstream fp;
	//fp.open("Debug/instance.cnf",std::ios::in);
	fp.open(inputFile,std::ios::in);
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
	assignG = vector<bool>(numVsG,false);
	  getline(fp,buff);
	}
   	// Get the clauses
   	int line = 0;
   	while(!fp.eof() && line < numCsG){
		getline(fp,buff);
		if(buff.empty()) break;
		parseLine(buff);
		line++;
   	}
};
void memAllocate(string buff){
	char* str = strdup(buff.c_str());
    const char s[2] = " ";
	strtok(str, s);
	strtok(NULL, s);
	numVsG = atoi(strtok(NULL, s))+1;
	numCsG = atoi(strtok(NULL, s));
	clauses0 = new vector<int>[numCsG];
	clauses1 = new vector<int>[numCsG];
	posOc0 = (int*) malloc(sizeof(int) * numVsG);
	negOc0 = (int*) malloc(sizeof(int) * numVsG);
	posOc1 = (int*) malloc(sizeof(int) * numVsG);
	negOc1 = (int*) malloc(sizeof(int) * numVsG);
	for(int i = 0; i < numVsG; i++){
		posOc0[i] = 0;
	}
	for(int i = 0; i < numVsG; i++){
		negOc0[i] = 0;
	}
	for(int i = 0; i < numVsG; i++){
		posOc0[i] = 0;
	}
	for(int i = 0; i < numVsG; i++){
		negOc0[i] = 0;
	}
	clauseT0.reserve(numVsG);
	clauseT1.reserve(numVsG);
}
void parseLine(string line){
	char* str = strdup(line.c_str());
    const char s[2] = " ";
    int lit;
    int size;
    int index;
    char* token= strtok(str, s);
    while(token != NULL){
		if(*token== '-'){
			lit = atoi(token);
			index = ind[-lit];
			if(partition[-lit] == 0){
				clauseT0.push_back(-index);
				negOc0[index]++;
			}
			else {
				clauseT1.push_back(-index);
				negOc1[index]++;
			}
			token = strtok(NULL, s);
			size++;
			continue;
		}
		if(*token == '0'){
			if(clauseT0.size()> clauseT1.size()){
				clauses0[c0]=clauseT0;
				c0++;
			}
			else {
				clauses1[c1]=clauseT1;
				c1++;
			}
			clauseT0.clear();
			clauseT1.clear();
		    return;
		}
		lit = atoi(token);
		index = ind[lit];
		if(partition[lit] == 0){
			clauseT0.push_back(index);
		    posOc0[index]++;
		}
		else{
			clauseT1.push_back(index);
		    posOc1[index]++;
		}

	    size++;
		token = strtok(NULL, s);
    }
	perror("a clause line does not terminates");
	exit(EXIT_FAILURE);
}
void readPartition(string file){
	//partition.reserve(10);
	partition.push_back(0);
	//cout<< "in readFile"<<endl;
	ifstream fp;
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
   	   	getline(fp,buff);

   	}
   	numVsG =partition.size();
   	ind.reserve(numVsG);
   	v0=0;
   	v1=1;
   	for(int i = 0; i < numVsG; i++){
   		if(partition[i]==0){
   			ind.push_back(v0);
   			v0++;
   		}
   		else{
   			ind.push_back(v1);
   			v1++;
   		}
   	}
};
string  part()
{
  string fileConvert = "./fileConvert ";
  fileConvert.append(string(inputFile)).append(" p");
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
  Kahypar.append(string(inputFile).append(".p")).append(block).append(imbalance).append(object).append(mode).append(preset0).append("> /dev/null");
  sr = system (Kahypar.c_str());
  if(sr == -1){
	perror("Kahypar fails");
	exit(EXIT_FAILURE);

  }
  std::array<char, 128> buffer;
  std::string result;
  string a = "find *.KaHyPar";
  const char* cmd =a.c_str();
  std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
      if (fgets(buffer.data(), 128, pipe.get()) != nullptr){
          result += buffer.data();
      }
  }
  	result.pop_back();
  	return result;
}

