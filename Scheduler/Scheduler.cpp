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

#include "Scheduler.h"

using namespace std;

//Scheduler

Scheduler::Scheduler(){
    this->quantum = 0;
    this->type = "";
}

void Scheduler:: addExpireQ(Process*){}

void Scheduler:: addReadyQ(Process*){}

Process* Scheduler:: getProcess(){Process* p; return p;}

//FCFS
FCFS::FCFS(){
    
}

void FCFS::addReadyQ(Process* process){
    readyQ.push_back(process);
}

Process* FCFS::getProcess(){
    Process* p;
    if (!readyQ.empty()){
        p = readyQ.front();
        readyQ.pop_front();
    }
    return p;;
}

void FCFS::addExpireQ(Process*){}


//LCFS
LCFS::LCFS():Scheduler(){
    
}

void LCFS::addExpireQ(Process*){};

void LCFS::addReadyQ(Process* process){
    readyQ.push_back(process);
}

Process* LCFS::getProcess(){
    Process* p;
    if(!readyQ.empty()){
        p = readyQ.back();
        readyQ.pop_back();
    }
return p;
}


//SJF
SJF::SJF():Scheduler(){
   
}

void SJF::addExpireQ(Process*){}

void SJF::addReadyQ(Process* process){
   int pos = int(readyQ.size());
   for (int i = 0; i < readyQ.size();i++){
       if(readyQ[i]->rem_time > process->rem_time){
           pos = i;
           break;
       }
   }
   readyQ.insert(readyQ.begin()+pos,process);
}

Process* SJF::getProcess(){
   Process* p;
   if(!readyQ.empty()){
       p = readyQ.front();
       readyQ.pop_front();
   }
   return p;
}

   
//RR
RR::RR(int quantum):Scheduler(){
   this->quantum = quantum;
}

void RR::addExpireQ(Process*){}


void RR:: addReadyQ(Process* process){
   readyQ.push_back(process);
}

Process* RR::getProcess(){
   Process* p;
   if(!readyQ.empty()){
       p = readyQ.front();
       readyQ.pop_front();
   }
   return p;
}
               
//PRIO
PRIO::PRIO(int quantum):Scheduler(){
   this->quantum = quantum;
}

void PRIO::addReadyQ(Process* process){
    int pos = int(readyQ.size());
   for (int i = 0; i < readyQ.size();i++){
       if(readyQ[i]->D_PRIO < process->D_PRIO){
           pos = i;
           break;
       }
   }
   readyQ.insert(readyQ.begin()+pos,process);
}

void PRIO::addExpireQ(Process* process){
   int pos = int(expireQ.size());
   for (int i = 0; i < expireQ.size();i++){
       if(expireQ[i]->D_PRIO < process->D_PRIO){
           pos = i;
           break;
       }
   }
   expireQ.insert(expireQ.begin()+pos,process);

}


Process* PRIO::getProcess(){
   Process* p;
   if (readyQ.empty()&& expireQ.empty()){
       return NULL;
   }

   if(readyQ.empty()){

       while(!expireQ.empty()){

           p = expireQ.front();
           expireQ.pop_front();
           readyQ.push_back(p);
       }
    }

        p = readyQ.front();
        readyQ.pop_front();
        return p;

}
