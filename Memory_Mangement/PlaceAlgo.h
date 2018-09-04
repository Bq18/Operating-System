#ifndef PLACEALGO_h
#define PLACEALGO_h

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

#include "PTE.h"

using namespace std;

class PlaceAlgo{
public:
    //constructor
    PlaceAlgo();
    
    int Frame_counter;
    int FrameSize;
    int ofs;
    int FrameIndex;
    vector<int> randVals;

    int myRandom(int );
    virtual void initialAge();
    virtual int getPage(vector<int>&,vector<PTE>&) = 0;
};


class NRU: public PlaceAlgo{
public:
    
    int clock;
    int getPage(vector<int> &,vector<PTE>&);
    //vector <vector<int>> class(4,vector<int>(64));
};

class Random:public PlaceAlgo{
public:
    
    int getPage(vector<int>&,vector<PTE>&);
    
};

class Second_Chance:public PlaceAlgo{
public:
    int getPage(vector<int>&,vector<PTE>&);
};

class FIFO:public PlaceAlgo{
public:
    int getPage(vector<int>&,vector<PTE>&);
    
};

class Clock_P:public PlaceAlgo{
public:
    int hand;
    int getPage(vector<int>&,vector<PTE>&);
};

class Clock_V:public PlaceAlgo{
public:
    int hand;
    int getPage(vector<int>&,vector<PTE>&);
};

class Aging_P:public PlaceAlgo{
public:
//CONSTRUCTOR;
  /* Aging_P(int Size):PlaceAlgo(int Size){
        for(int i = 0; i < FrameSize;i++){
            age[i] = 0;

        }
   };*/
    vector<unsigned int> age;
    void initialAge();
    int getPage(vector<int>&,vector<PTE>&);

};

class Aging_V:public PlaceAlgo{
public:
    //CONSTRUCTOR;
    vector<unsigned int> age;
    Aging_V():PlaceAlgo(){
        for(int i = 0; i < 64;i++){
            age.push_back(0);
        }
    };
    int getPage(vector<int>&,vector<PTE>&);
};

#endif
