#include <algorithm>
#include <iostream>
#include <fstream>
#include <istream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <vector>
#include<map>
#include<iomanip>

using namespace std;

//declare struct
struct DefList{
    int defCount;
    vector<string> defSymbol;
    vector<string> defAddress;
    vector<bool> multiDefine;
};


struct UseList{
    int useCount;
    vector<string> useSymbol;
};

struct ProgramText{
    int codeCount;
    vector<string> type;
    vector<string> inStr;
};

struct Module{
    int no_Module;
    int baseAddress;
};


//function declaration
void passOne(char* fileName);
void passTwo(char* fileName);
string parseWord(ifstream& ,char);
void parseDefList(ifstream&,char);
void parseUseList(ifstream&);
void parseProgramText(ifstream&);
void parseDefList2(ifstream&,char);
void parseUseList2(ifstream&);
void parseProgramText2(ifstream&);
void printSymbolTable();
void parseError(int);
bool isNumber(string);
bool isSymbol(string);
bool isType(string);
void setSymbolUsed(string symbol);
void setInsUsed(string symbol);
bool checkDefined(string symbol);

bool checkRule2(string symbol);
bool checkRule3(string value);
void checkRule4();
void checkRule5();
bool checkRule5(string value);
bool checkRule6(string value);
void checkRule7();
bool checkRule8(string value);
bool checkRule9(string value);
bool checkRule10(string value);
bool checkRule11(string word);




//global variable declaration;

//int pos = 0;
string word = "";
//variable used in pass1
int lineoffset = 0;
int prelineoffset;
int line = 1;
int preline;
int word_length;
int moduleSize;
//variable used in pass2
int instrpos = 0;
int moduleno = 0;
string errMsg;

vector<DefList> defList;
vector<UseList> useList;
vector<ProgramText> progList;
vector<Module> moduleList;
//key-value:defined symbol-address
map<string, string> defMapSymbol;
//key-value:
map<string,int> defMapval;
//key-value:define symbol- module_no
map<string,int>defMapuse;
//key-value:instruction symbol- whether used in the use list
map<string,bool>useMapins;




string parseWord(ifstream& infile,char c){
	if ( c!= ' '&&  c!= '\n' && c!= '\t'){
		word_length = 1;
		word += c;
	} else {
        if(c == '\n'){
            line++;
        }
		word_length = 0;
	}
    lineoffset++;

    while(!infile.eof()){
        infile.get(c);
        if (c == ' '|| c == '\t'||c == '\n'){
            if(c == ' ' || c == '\t'){                
                preline = line;
                prelineoffset = lineoffset;

                lineoffset++;
                
                if (word_length != 0){
                 //cout << "   line  " << preline << "     lineoffset  " << prelineoffset - word_length + 1 <<"   word Len   " <<word_length <<"  word  " << word<< endl;
                    return word;
                }
            }else if (c == '\n'){
                preline = line;
                line++;
                prelineoffset = lineoffset;
                lineoffset = 0;
                if(word_length != 0){
                    
                   // cout << "   line  " << preline << "    lineoffset  " << prelineoffset-word_length +1 <<"   word Len   " <<word_length <<"  word  " << word<< endl;
		 return word;
                }
            }
        }else if ( c!= ' '&&  c!= '\n' && c!= '\t'){
            word +=c;
            lineoffset++;
            word_length++;
        }
    }
    prelineoffset++;
    return word;
}


bool isNumber(string str){
    if (str == ""){
        return false;
    }
    string::iterator c;
    for (c = str.begin(); c < str.end();c++){
        if (*c <'0' || *c > '9'){
            return false;
        }
    }
    return true;
}

bool isSymbol(string str){
    if (str == ""){
        return false;
    }
    
    if (!isalpha(str.at(0))){
        return false;
    }
    
    string::iterator c;
    
    for (c = str.begin(); c < str.end();c++){
        if(!isalpha(*c) && !isdigit(*c)){
            return false;
        }
    }
    return true;
}

bool isType(string ch){
    if (ch == ""){
        return false;
    }
    if (ch == "A" ||ch == "I" || ch == "R"|| ch =="E"){
        return true;
    }
    return false;
}

void parseError(int errcode) {
    static string errstr[] = {
        "NUM_EXPECTED", // Number expect
        "SYM_EXPECTED", // Symbol Expected
        "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG", // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR"// > machine size
    };
    printf("Parse Error line %d offset %d: %s\n",preline,prelineoffset-word_length+1, errstr[errcode].c_str());
}




void setSymbolUsed(string symbol){
    map<string,int>:: iterator it = defMapuse.find(symbol);
    if(it != defMapuse.end()){
        it->second = -1;
    }
}


void setInsUsed(string symbol){
    map<string,bool>:: iterator it = useMapins.find(symbol);
    if(it != useMapins.end()){
        it->second = true;
    }
}

bool checkRule2(string symbol){
    map<string,string>::iterator it;
    it = defMapSymbol.find(symbol);
    if ( it != defMapSymbol.end()){
        return true;
    }
    return false;
}



bool checkRule3(string word){
    
    //int opcode = atoi(word.c_str())/1000;
    int operand  = atoi(word.c_str())%1000;
    string symbol = useList[moduleno-1].useSymbol[operand];
    if(!checkRule2(symbol)){
        errMsg = "Error: " + symbol +" is not defined; zero used";
        return true;
    }
    return false;
}


void checkRule4(){
    for(map<string,int >::iterator it = defMapuse.begin(); it != defMapuse.end();++it){
        if(it->second != -1){
            printf( "Warning: Module %d: %s was defined but never used\n",it->second,it->first.c_str() );
        }
    }
}


bool checkRule6(string word){
    
    int operand = atoi(word.c_str())%1000;
    if(operand > useList[moduleno-1].useSymbol.size()-1){
        errMsg ="Error: External address exceeds length of uselist; treated as immediate";
        return true;
    }
    return false;
    //intermdiate
}

void checkRule7(){
    for(map<string,bool>::iterator it = useMapins.begin(); it != useMapins.end();++it){
        if(it->second == false){
            it->second = true;
            printf( "Warning: Module %d: %s appeared in the uselist but was not actually used\n",moduleno,it->first.c_str());
        }
    }
}



bool checkRule8(string word){
    int n = atoi(word.c_str());
    int operand = n % 1000;
    if (operand >= 512-1){
        errMsg = "Error: Absolute address exceeds machine size; zero used";
        return true;
    }
    return false;
}



bool checkRule9(string word){
    int n = atoi(word.c_str());
    int operand = n % 1000;
    for (int i = 0;i < progList.size();i++){
        moduleSize += progList[i].codeCount;
    }
    if (operand > moduleSize){
        errMsg =  "Error: Relative address exceeds module size; zero used";
        return true;
    }
    return false;
}



bool checkRule10(string word){
    if (word.length() > 4){
        errMsg = "Error: Illegal immediate value; treated as 9999";
        return true;
    }
    return false;
}


bool checkRule11(string word){
    if(word.length() > 4){
        errMsg = "Error: Illegal opcode; treated as 9999";
        return true;
    }
    return false;
}

//codeCount-1?
void checkRule5(){
    for (int i = 0; i < moduleno;i++){
        for (int j= 0; j < defList[i].defSymbol.size();j++){
            int value = atoi(defList[i].defAddress[j].c_str());
            //check whether defined symbol address is larger than module size
            if (value > progList[i].codeCount){
                printf( "Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",moduleno,defList[i].defSymbol[j].c_str(),atoi(defList[i].defAddress[j].c_str()),progList[i].codeCount-1);
                    defList[i].defAddress[j] = "0";

            }
        }
    }
}


void printSymbolTable(){
    cout << "Symbol Table"<< endl;
    for (int i = 0; i < moduleno;i++){
        for (int j = 0; j< defList[i].defSymbol.size();j++){
            
            int addressSum = 0;
            for (int k = 0; k < i;k++){
                addressSum += moduleList[k].baseAddress;
            }
            int value = atoi(defList[i].defAddress[j].c_str())+ addressSum;
            string symbol = defList[i].defSymbol[j];
            
            map<string,string>::iterator it = defMapSymbol.find(symbol);
            
            if (it!= defMapSymbol.end()){
                if ( it->second == "false"){
                    defMapuse.insert(make_pair(defList[i].defSymbol[j].c_str(),moduleList[i].no_Module));
                    defMapval.insert(make_pair(defList[i].defSymbol[j].c_str(),value));
                    cout<< symbol.c_str() << "=" << value <<endl;
                    it->second = "used";
                }else if (it->second =="true"){
                    cout << symbol << "=" << value << " Error: This variable is multiple times defined; first value used" <<endl;
                    it->second = "used";
                    defMapval.insert(make_pair(defList[i].defSymbol[j].c_str(),value));

                }
            }
        }
    }
    cout << endl;
}

void parseDefList(ifstream& infile,char c){
    DefList def;
    word = "";
    word = parseWord(infile,c);
    //number test
    if(!isNumber(word)){
        parseError(0);
        exit(1);
    }
    
    int count = atoi(word.c_str());
    //number of defined symbol test
    if (count > 16){
        parseError(4);
        exit(1);
    }
    
    for (int i = 0; i < count; i++){
        infile.get(c);
        word = "";
	    word = parseWord(infile,c);
        //symbol test
        if(!isSymbol(word)){
            parseError(1);
            exit(1);
        }
        //symbol length test
        if (word.size() > 16){
            parseError(3);
            exit(1);
        }
        def.defSymbol.push_back(word);
        
        //multiDefine
        map<string,string>:: iterator it = defMapSymbol.find(word);
        if(it != defMapSymbol.end()){
            it->second = "true";
        } else{
            defMapSymbol.insert(make_pair(word,"false"));
        }
    
        infile.get(c);
        word = "";
        word = parseWord(infile,c);
        //address test
        if (!isNumber(word)){
            parseError(0);
            exit(1);
        }
        def.defAddress.push_back(word);

    }
    def.defCount = count;
    defList.push_back(def);
}

void parseUseList(ifstream& infile){
    char c;
	UseList use;
    infile.get(c);
    word = "";
    word = parseWord(infile,c);
    
    //number test
    
    if (!isNumber(word)){
        parseError(0);
        exit(1);
    }
    
    int count = atoi(word.c_str());
    //number of use symbol test
    if (count > 16){
        parseError(5);
        exit(1);
    }
    
    for (int i = 0; i < count;i++){
        infile.get(c);
        word = "";
	word = parseWord(infile,c);
        
        // symbol test
        if(!isSymbol(word)){
            parseError(1);
            exit(1);
        }
        //symbol lenghth test
        if (word.size() > 16){
            parseError(3);
            exit(1);
        }
        use.useSymbol.push_back(word);
    }
    use.useCount = count;
    useList.push_back(use);
}

void parseProgramText(ifstream& infile){
    char c;
	Module module;
    
    //parse Program Test
    infile.get(c);
    word = "";
	word = parseWord(infile,c);
    ProgramText programText;
    if (!isNumber(word)){
        parseError(0);
        exit(1);
    }
    int count = atoi(word.c_str());
    
    if (count >= 512-1){
        parseError(6);
        exit(1);
    }
    
    
    module.no_Module = moduleno;
    module.baseAddress = count;
    for (int i = 0; i < count; i++){
        infile.get(c);
        word = "";
	word = parseWord(infile,c);
        //instruction test
        if(!isType(word)){
            parseError(2);
            exit(1);
        }
        
        //number test
        infile.get(c);
        word = "";
	word = parseWord(infile,c);
        if(!isNumber(word)){
            parseError(0);
            exit(1);
        }
    }
    programText.codeCount = count;
    progList.push_back(programText);
    moduleList.push_back(module);
}

void parseDefList2(ifstream& infile,char c){
 
    word = "";
	word = parseWord(infile,c);
    int count = atoi(word.c_str());//number of symbol
    for (int i = 0; i < count; i++){
        
 
    infile.get(c);
    word = "";
	word = parseWord(infile,c);// Defined Symbol
 
    infile.get(c);
    word = "";
	word = parseWord(infile,c);//Address of Symbol
    }
}

void parseUseList2(ifstream& infile){
    char c;
    infile.get(c);
    word = "";
	word = parseWord(infile,c);//number of used symbol
    int count = atoi(word.c_str());
    
    for (int i = 0; i < count; i++){
 
        infile.get(c);
        word = "";
        word = parseWord(infile,c);//Used Symbol
        setSymbolUsed(word);
        useMapins.insert(make_pair(word.c_str(),false));
    }
}


void parseProgramText2(ifstream& infile){
    char c;
    infile.get(c);
    word = "";
	word = parseWord(infile,c);
    int count = atoi(word.c_str());//number of instruction
    int opcode;
    int operand;
    int value;
    
    for (int i = 0;i < count;i++){
        errMsg = "";
        word = "";
        infile.get(c);
        word = parseWord(infile,c);//instruction
        instrpos++;
        value = 0;
        if (word == "A"){
 
            infile.get(c);
            word = "";
            word = parseWord(infile,c);
            value = atoi(word.c_str());
            
            if(checkRule11(word)){
                value = 9999;
            }else if(checkRule8(word)){
                opcode = atoi(word.c_str())/1000;
                value = opcode*1000 + 0;
            }
        }else if (word == "E"){
 
            infile.get(c);
            word = "";
            word = parseWord(infile,c);

            if(checkRule11(word)){
                value = 9999;
                
                string symbol = useList[moduleno-1].useSymbol[0];
                setInsUsed(symbol);
                
            }else if(checkRule6(word)){
                value = atoi(word.c_str());
                
            }else if(checkRule3(word)){
                value = atoi(word.c_str());

               // value = 0;
                string symbol = useList[moduleno-1].useSymbol[0];
                setInsUsed(symbol);

                
            } else {
                opcode = atoi(word.c_str())/1000;
                operand  = atoi(word.c_str())%1000;
                string symbol = useList[moduleno-1].useSymbol[operand];
                map<string,int>:: iterator it = defMapval.find(symbol);
                if(it != defMapval.end()){
                    value = it->second;
                }
                setInsUsed(symbol);
                value += opcode*1000;
                
            }
        } else if (word == "I"){
            
 
            infile.get(c);
            word = "";
            word = parseWord(infile,c);

            value = atoi(word.c_str());
            
            if(checkRule10(word)){
                value = 9999;
            }
        } else if (word == "R"){
 
            infile.get(c);
            word = "";
            word = parseWord(infile,c);
            int n =atoi(word.c_str());
            int moduleSum = 0;
            for(int i = 0; i < moduleno-1;i++){
                moduleSum += moduleList[i].baseAddress;
            }
            value = n + moduleSum;
            if(checkRule11(word)){
                value = 9999;
            } else if (checkRule9(word)){
                opcode = n/1000;
                value = 1000*opcode + moduleSum;
                
            }
        }
        std::cout << std::setfill ('0') << std::setw (3);
        
        cout<< instrpos-1;
        
        cout<< ": " ;
        
        cout << std::setfill ('0') << std::setw (4);

        cout << value;
        
        cout << " " << errMsg << endl;

    }
}


void passOne(char* filename){
    
    ifstream infile(filename);
    char c;
    while (infile.get(c)){
        if (!infile.eof()){
        	moduleno++;
        	parseDefList(infile,c);
        	parseUseList(infile);
        	parseProgramText(infile);
        //check def address size after each module
        	checkRule5();
    	}
    }

   	 printSymbolTable();
     infile.close();
}


void passTwo(char* filename){
    char c;
    moduleno = 0;
    line = 1;
    preline = 1;
    lineoffset = 1;
    prelineoffset = 1;
    ifstream infile(filename);
    while (infile.get(c)){
        if (!infile.eof()){
            moduleno++;
            parseDefList2(infile,c);
            parseUseList2(infile);
            parseProgramText2(infile);
            checkRule7();
        }
    }
    cout << endl;
    checkRule4();
    infile.close();
}

int main(int argc, char* argv[]){
    if (argc != 2 or argv[1] == NULL){
        cout<<"Please enter the file name"<< endl;
        exit(0);
    }
    passOne(argv[1]);
    
    cout <<"Memory Map"<< endl;
    
    passTwo(argv[1]);
}

