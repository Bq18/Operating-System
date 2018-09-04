#include <stdio.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <ctype.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <deque>
#include <string>
#include <cstring>
#include <sstream>

#include "PlaceAlgo.h"
#include "PTE.h"

using namespace std;

int COUNT = 0;
bool O = false;
bool P = false;
bool F = false;
bool S = false;
int FrameSize = 32;
int FrameIndex = 0;
PlaceAlgo* algo;


struct Instruction{
    int op;
    int page;
};

struct Statistics{
    int inst_count;
    int unmaps;
    int maps;
    int outs;
    int ins;
    int zeros;
    long long totalcost;
}stas;

vector<PTE> pagetable;
vector<int> Frame;
vector<int>randVals;
vector<Instruction>insVec;

//Member Function
void readFile(string );
void readRand(string);
int myRandom(int);
void printSummery();
void printPageTable();
void printFrameTable();
void simulation(int op, int pageIndex,int i);
int getFreeFrame();
void initialFrame();
void initialPT();

int main(int argc, char*argv[]){
    string s;
    int c;
    opterr = 0;
    bool age = false;
    //parse_option
    while ( (c = getopt(argc, argv, "a:o:f:")) != -1 ){
        switch (c) {
            case 'a':{
                char word = optarg[0];
                    if (word == 'N'){
                        algo = new NRU();
                    } else if (word == 'r' ){
                        algo = new Random();
                    } else if (word == 'f'){
                        algo = new FIFO();
                    } else if (word == 's'){
                        algo = new Second_Chance();
                    } else if (word == 'c'){
                        algo = new Clock_P();
                    } else if (word == 'X'){
                        algo = new Clock_V();
                    } else if (word == 'a'){
                        algo = new Aging_P();
                        age = true;
                    } else if(word == 'Y'){
                        algo = new Aging_V();
                    }
                break;
            }
        
            case 'o':{
            unsigned long len = strlen(optarg);
                for (int i = 0; i < len; i++){
                    if (optarg[i] == 'O'){
                        O = true;
                    } else if (optarg[i] == 'P'){
                        P = true;
                    } else if (optarg[i] == 'F'){
                        F = true;
                    } else if (optarg[i] == 'S'){
                        S = true;
                    }
                }
                break;
            }
            case 'f':
                sscanf(optarg, "%d", &FrameSize);
                break;
            case '?':
                if (optopt == 'f')
                    fprintf (stderr, "Option -%c requires an argument. \n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option '-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
                return 1;
                
            default:
                abort ();
        }
    }

    string inputfile = argv[optind];
    string randfile = argv[optind+1];
    readRand(randfile.c_str());
    readFile(inputfile.c_str());
    initialPT();
    initialFrame();
    algo->randVals = randVals;
    algo->FrameSize = FrameSize;
    if (age){
        algo->initialAge();
    }
    for (int i; i < insVec.size();i++){
        Instruction instr = insVec[i];
        simulation(instr.op,instr.page,i);
    }
    if(P){
        printPageTable();
    }
    
    if(F){
        printFrameTable();
    }
    
    if(S){
        printSummery();
    }

    return 0;
}
//read inputfile
void readFile(string fileName){
    int op;
    int page;
    ifstream inFile;
    inFile.open(fileName.c_str());
    string line;
    //while(getline(inFile,line)){

    while(getline(inFile,line)){
        Instruction instruction;
        if (line.length() == 0 || line[0] == '#') continue;
       	istringstream token(line);

        if(!(token >> op >> page)){
            break;
        }

        instruction.op = op;
        instruction.page = page ;
        insVec.push_back(instruction);
    }
    
    inFile.close();
}

void initialPT(){
    for (int i = 0; i < 64; i++){
        PTE pte;
        pte.present = 0;
        pte.pageout = 0;
        pte.modified = 0;
        pte.referenced = 0;
        pte.f_index = 0;
        pagetable.push_back(pte);
    }
}

void initialFrame(){
    for (int i = 0; i < FrameSize;i++){
        Frame.push_back(-1);
    }
}

//read random file
void readRand(string fileName){
    ifstream inFile;
    inFile.open(fileName.c_str());
    string line;
    string str;
    getline(inFile,str);
    
    COUNT = atoi(str.c_str());
    
    while(getline(inFile,line)){
        randVals.push_back(atoi(line.c_str()));
    }
    inFile.close();
}



int getFreeFrame(){
    for (int i = 0; i < FrameSize;i++){
        if (Frame[i] == -1){
            return i;
        }
    }
    return -1;
}

void simulation(int op, int pageIndex,int i){
    bool FrameFree = true;

    if (O) {
        printf("==> inst: %d %d\n", op, pageIndex);
    }
    
    //not present
    if(pagetable[pageIndex].present == 0){
        int ptIndex;
        FrameIndex = getFreeFrame();
        //not free space, replace algorithm select the Frame.
        if(FrameIndex == -1){
            FrameFree = false;
            FrameIndex = algo->getPage(Frame,pagetable);
            
            ptIndex = Frame[FrameIndex];

            //unmap
            pagetable[ptIndex].present = 0;
            pagetable[ptIndex].referenced = 0;
            stas.unmaps++;
            
            if(O) {
                printf("%d: UNMAP %3d %3d\n", stas.inst_count, ptIndex, FrameIndex);
            }
            
            
            //check whether oldpage is dirty
            if(pagetable[ptIndex].modified == 1){
                pagetable[ptIndex].modified = 0;
                //out
                pagetable[ptIndex].pageout = 1;
                stas.outs++;
                if(O){
                    printf("%d: OUT  %4d%4d\n", stas.inst_count, ptIndex,FrameIndex);
                    
                }
            }
        }
            
            //check pageout
        if(pagetable[pageIndex].pageout == 1){
            if (O){
                printf("%d: IN %6d %3d\n", stas.inst_count, pageIndex, FrameIndex);
            }
            stas.ins++;
        } else{
            if (O){
                printf("%d: ZERO %8d\n", stas.inst_count, FrameIndex);
            }
            stas.zeros++;
        }
        if (O){
            printf("%d: MAP %5d %3d\n", stas.inst_count, pageIndex, FrameIndex);
            
        }
        pagetable[pageIndex].f_index = FrameIndex ;
        pagetable[pageIndex].present = 1;
        stas.maps++;
        
        if(FrameFree){
            Frame[FrameIndex] = pageIndex;
        }else{
            Frame[FrameIndex] = pageIndex;
        }
            //page present
    }
    
    pagetable[pageIndex].referenced = 1;
    if(op == 1)	{
        pagetable[pageIndex].modified = 1;
    }
    stas.inst_count++;
    
}

void printPageTable() {
    for (int i=0; i< pagetable.size(); i++) {
        PTE page = pagetable[i];
        if (pagetable[i].present== 1){
            cout << i << ":" ;
            if(pagetable[i].referenced == 1){
                cout << "R";
            }else{
                cout << "-";
            }
            
            if(pagetable[i].modified == 1){
                cout << "M";
            }else{
                cout << "-";
            }
            
            if(pagetable[i].pageout == 1){
                cout << "S";
            }else{
                cout << "-";
            }
            cout << " ";
            
        } else {
            if (pagetable[i].pageout == 1) {
                printf("# ");
            } else {
                printf("* ");
            }
        }
    }
    printf("\n");
}

void printFrameTable() {
    for (int i=0; i<FrameSize; i++){
        if (Frame[i] != -1) {
            printf("%d ",Frame[i]);
        }else{
            printf("* ");
        }
    }
    printf("\n");
}

void printSummery(){
    stas.totalcost = long(400 * (stas.maps + stas.unmaps)) + long(3000 * (stas.ins + stas.outs)) + long(150 * stas.zeros + stas.inst_count);
    
    printf("SUM %d U=%d M=%d I=%d O=%d Z=%d ===> %llu\n",
           stas.inst_count, stas.unmaps, stas.maps, stas.ins, stas.outs, stas.zeros, stas.totalcost);
    
}
