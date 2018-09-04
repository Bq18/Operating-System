#include <iostream>
#include <fstream>
#include <istream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <queue>

#include "Process.h"

using namespace std;
Process::Process(){
    
    this->ID = 0;
    this->AT = 0;
    this->TC = 0;
    this->CB = 0;
    this->IO = 0;
    
    this->CW = 0;
    this->TT = 0;//Turnaround time;
    this->IT = 0;//i.o time
    this->FT = 0;
    
    this->cb = 0;
    this->ib = 0;
    this->cb_rem = 0;
    
    this->rem_time = 0;
    this->state_gt = 0;//prev event genrate time
    this->timeInPrevState = 0;
    this->finish_time = 0;
    
    this->PRIO = 0;
    this->D_PRIO = 0;
}

Process::Process (int ID, int AT,int TC, int CB,int IO){
    
    this->ID = ID;
    this->AT = AT;
    this->TC = TC;
    this->CB = CB;
    this->IO = IO;
    
    this->CW = 0;
    this->TT = 0;//Turnaround time;
    this->IT = 0;//i.o time
    this->FT = 0;
    
    this->rem_time = TC;
    this->state_gt = AT;//prev event genrate time
    this->timeInPrevState = 0;
    this->finish_time = 0;
    
    this->cb = 0;
    this->ib = 0;
    this->cb_rem = 0;
    
    this->PRIO = 0;
    this->D_PRIO = 0; 
}   
