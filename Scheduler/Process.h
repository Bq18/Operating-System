#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <fstream>
#include <istream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

class Process{
    
public:
    Process();
    Process(int ID,int AT,int TC,int CB,int IO);//constructor
    
    int ID;
    int AT;
    int TC;
    int CB;//CPU Burst;
    int IO;//IO BURST;
    int PRIO;//STATIC PRIORITY
    
    int D_PRIO;
    
    int ib;
    int cb;
    int cb_rem;
    
    int FT;//finish time;
    int TT;//Turnaround time;
    int IT;//i.o time
    int CW;//in waiting time;
    
    int rem_time;
    int state_gt;//prev event genrate time
    int timeInPrevState;
    int finish_time;
    
    
};

#endif
