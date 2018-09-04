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

#include "Process.h"
#include "Scheduler.h"
#include "Event.h"


using namespace std;

int ofs = 0;
int FINISH = 0;
int io_num = 0;
int ioStart = 0;
double cpu_util = 0;
double io_util = 0;
double avg_tt = 0;
double avg_wt = 0;;
double Throught_out = 0;;
int quantum = -1;
int ava_time;

deque <Event*> eventQ;
deque<Process*> processQ;
vector<int> randVals;
Event* e;
Scheduler* scheduler;
int b;

//Member Function
void readFile(string );
void readRand(string);
void insertEvent(Event* );
int myRandom(int);
void selectScheduler(string s);
void printSummery();
void printVerbose(Event*,bool,int);



int main(int argc, char*argv[]){
    bool v = false;
    string s;
    int c;
    opterr = 0;
    
    //parse_option
    while ( (c = getopt(argc, argv, "vs:")) != -1 ){
        switch (c) {
            case 'v':
                v = true;
                break;
            case 's':
                s = string(optarg);
                selectScheduler(s);
                break;
            case '?':
                if (optopt == 's')
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
    
    while(!eventQ.empty()){
        Event* event  = eventQ.front();
        int current_time = event->timestamp;
        event->evtProcess->timeInPrevState = current_time -
        event->evtProcess->state_gt;
        eventQ.pop_front();
        
        string trans = event->transition;
        
        int selector;
        if (trans == "CREATED -> READY"){
            selector = 0;
        }else if (trans == "READY -> RUNNG"){
            selector = 1;
        }else if (trans == "RUNNG -> BLOCK"){
            selector = 2;
        }else if(trans == "BLOCK -> READY"){
            selector = 3;
        }else if(trans == "RUNNG -> READY"){
            selector = 4;
        }else if(trans == "Done" ){
            selector = 5;
        }
        
        switch(selector){
            case 0:{
                    Event* e = new Event(event->evtProcess,current_time,"READY -> RUNNG");
                    insertEvent(e);
                    scheduler->addReadyQ(event->evtProcess);

                    printVerbose(event,v,current_time);
                    break;
            }
            case 1:{
                /*
                if(!eventQ.empty() &&  eventQ.front()->timestamp == current_time && eventQ.front()->transition == "CREATED> -> READY"){
                    insertEvent(new Event(event->evtProcess,current_time,"READY -> RUNNG"));
                    break;
                }*/

                if(current_time < ava_time){
                    current_time = ava_time;
                    //event->evtProcess->state_gt = current_time;
                    insertEvent(new Event(event->evtProcess,current_time,"READY -> RUNNG"));
                    break;
                }

                Process* p = scheduler->getProcess();
                
                p->timeInPrevState = current_time - p->state_gt;

                p->CW += p->timeInPrevState;

                if(quantum == -1){

                    p->cb = myRandom(p->CB);
                    if(p->cb >= p->rem_time){
                        p->cb = p->rem_time;
                        printVerbose(event,v,current_time);
                        p->rem_time = 0;
                        Event* e = new Event(p,current_time +p->cb,"Done");
                        e->evtProcess->state_gt = current_time;
                        insertEvent(e);
                        ava_time = current_time + p->cb;


                    } else if (p->cb < p->rem_time){
                        printVerbose(event,v,current_time);
                        p->rem_time = p->rem_time - p->cb;
                        Event* e = new Event(p,current_time + p->cb,"RUNNG -> BLOCK");
                        e->evtProcess->state_gt = current_time;
                        insertEvent(e);
                        ava_time = current_time + p->cb;



                    }

                }else {
                      if (p->cb_rem == 0){
                            p->cb = myRandom(p->CB);
                            p->cb_rem = p->cb;

                        }
                        if( p->cb_rem >= p->rem_time){
                            p->cb_rem = p->rem_time;
                            p->cb = p->rem_time;

                        }
                        if (quantum < p->cb_rem){
                            if(v){
                            cout << current_time <<" " << p->ID <<" " <<p->timeInPrevState
                            << ": " << "READY -> RUNNG" <<" cb="  <<p->cb <<" rem="
                            << p->rem_time << " prio="
                            <<p->D_PRIO <<endl;
                            }
                            
                            p->cb = p->cb - quantum;
                            p->cb_rem = p->cb_rem - quantum;
                            p->rem_time = p->rem_time-quantum ;
                            Event* e = new Event(p,current_time + quantum,"RUNNG -> READY");
                            
                           
                            e->evtProcess->state_gt = current_time;
                            insertEvent(e);
                            ava_time = current_time + quantum;


                        }else if (quantum >= p->cb_rem){
                            if (p->cb_rem == p->rem_time){
                                if(v){
                                cout << current_time <<" " << p->ID <<" " <<p->timeInPrevState
                                << ": " << "READY -> RUNNG" <<" cb="  <<p->cb <<" rem="
                                << p->rem_time << " prio="
                                <<p->D_PRIO <<endl;
                                }
                                
                                Event* e = new Event(p,current_time + p->cb_rem,"Done");
                                ava_time = current_time + p->cb_rem;

                                p->cb_rem = 0;
                                p->rem_time = 0;
                                p->cb = 0;
                                e->evtProcess->state_gt = current_time;
                                insertEvent(e);


                            } else{
                                if (v){
                                cout << current_time <<" " << p->ID <<" " <<p->timeInPrevState
                                << ": " << "READY -> RUNNG" <<" cb="  <<p->cb <<" rem="
                                << p->rem_time << " prio=" <<p->D_PRIO <<endl;
                                }
                                p->rem_time = p->rem_time - p->cb_rem;
                                Event* e = new Event(p,current_time + p->cb_rem,"RUNNG -> BLOCK");
                                ava_time = current_time + p->cb_rem;
                                p->cb_rem = 0;
                                p->cb = 0;
                            
                                e->evtProcess->state_gt = current_time;
                                insertEvent(e);


                            }
                        }
                 }
                    break;
            }
            case 2:{
                    io_num++;
                    if (io_num == 1){
                        ioStart = current_time;
                    }
                    int ob = myRandom(event->evtProcess->IO);
                    event->timestamp += ob;
                    event->evtProcess->IT+= ob;
                    event->evtProcess->ib = ob;
                    Event* e = new Event(event->evtProcess,event->timestamp,"BLOCK -> READY");
                    e->evtProcess->state_gt = current_time;
                    insertEvent(e);
                    printVerbose(event,v,current_time);
                    break;
            }
            case 3:{
                    //if(scheduler.type == PRIO){
                    io_num--;
                    if(io_num == 0){
                        io_util += (current_time -ioStart);
                    }
                    event->evtProcess->D_PRIO = event->evtProcess->PRIO -1;
                    Event* e = new Event(event->evtProcess,current_time,"READY -> RUNNG");
                    scheduler->addReadyQ(event->evtProcess);
                e->evtProcess->state_gt = current_time;

                    insertEvent(e);
                    printVerbose(event,v,current_time);
                    break;
            }
            case 4:{
                    printVerbose(event,v,current_time);
                    if(scheduler->type == "PRIO"){
                        event->evtProcess->D_PRIO = event->evtProcess->D_PRIO -1 ;

                        if (event->evtProcess->D_PRIO == -1){
                            event->evtProcess->D_PRIO = event->evtProcess->PRIO -1;
                            scheduler->addExpireQ(event->evtProcess);
                            
                        }else {
                            scheduler->addReadyQ(event->evtProcess);
                        }
                        
                    } else{
                        scheduler->addReadyQ(event->evtProcess);

                    }
               // cout << "Running to Ready" << endl;
                    Event* e = new Event(event->evtProcess,current_time,"READY -> RUNNG");
                    e->evtProcess->state_gt = current_time;

                    insertEvent(e);
                    break;
            }
            case 5:{
                printVerbose(event,v,current_time);

                    event->evtProcess->FT = current_time;
                    event->evtProcess->TT = current_time - event->evtProcess->AT;
            }
        }
    }
        printSummery();
        return 0;
}

    
    //print verbose trace
    
    void printVerbose(Event* event, bool verbose,int current_time){
        if (verbose == true){
            cout << current_time <<" " << event->evtProcess->ID <<" " <<event->evtProcess->timeInPrevState
            << ": " << event->transition;
            
            if(event->transition == "RUNNG -> BLOCK"){
                cout << " ib=" << event->evtProcess->ib << " rem="
                << event->evtProcess->rem_time;
            }else if(event->transition == "READY -> RUNNG"){
                cout <<" cb="  <<event->evtProcess->cb <<" rem="
                << event->evtProcess->rem_time << " prio="
                <<event->evtProcess->D_PRIO;
            }else if(event->transition == "RUNNG -> READY"){
                cout <<" cb="  <<event->evtProcess->cb <<" rem="
                << event->evtProcess->rem_time << " prio="
                << event->evtProcess->D_PRIO;
            }
            cout << endl;
        }
        
    }
    
    //read inputfile
    void readFile(string fileName){
        ifstream inFile;
        inFile.open(fileName.c_str());
        string line;
        int id = 0;
        //while(getline(inFile,line)){
        while(true){
            int at;
            int tc;
            int cb;
            int io;
           
            inFile >> at >> tc >> cb >> io;
            if(inFile.eof()){
              break;
            }
            
            Process* p = new Process(id, at,tc,cb,io);
            p->PRIO = myRandom(4);
            p->D_PRIO = p->PRIO -1;
            Event* e = new Event (p,at,"CREATED -> READY");
            processQ.push_back(p);
            eventQ.push_back(e);
            id++;
        }
        
        inFile.close();
    }
    
    
    //read random file
    void readRand(string fileName){
        ifstream inFile;
        inFile.open(fileName.c_str());
        string line;
        string str;
        getline(inFile,str);
        
        b = atoi(str.c_str());
        
        while(getline(inFile,line)){
            randVals.push_back(atoi(line.c_str()));
        }
        inFile.close();
    } 
    
    void selectScheduler(string s){
        
        if ( s == "F"){
            scheduler = new FCFS();
            scheduler->type = "FCFS";
        }else if (s =="L"){
            scheduler = new LCFS();
            scheduler->type = "LCFS";
        }else if (s == "S"){
            scheduler = new SJF();
            scheduler->type = "SJF";
        }else if (s.substr(0,1) =="R"){
            quantum = atoi(s.substr(1).c_str());
            scheduler = new RR (quantum);
            scheduler->type = "RR";
        }else {
            quantum = atoi(s.substr(1).c_str());
            scheduler = new PRIO(quantum);
            scheduler->type = "PRIO";
        }
    }
    
    
    int myRandom(int burst){
        if(ofs < b){
            return 1 + (randVals[ofs++]%burst);
        }else{
            ofs = 0;
            return 1 +(randVals[ofs++]%burst);
        }
    }
    
    
    void insertEvent(Event* e){
        int pos = int(eventQ.size());
        for (int i = 0; i < eventQ.size();i++){
            if (e->timestamp < eventQ[i]->timestamp){
                pos = i;
                break;
            }
        }
        eventQ.insert(eventQ.begin()+ pos,e);
        
    }
    
    void printSummery(){
        printf("%s", scheduler->type.c_str());
        
        if (scheduler->type == "RR" || scheduler->type == "PRIO"){
            printf(" %d", quantum);
        }
        cout << endl;
        
        int size = int(processQ.size());
        for (int i = 0; i < size;i++){
            printf("%04d: %4d %4d %4d %4d %d |  %4d  %4d  %4d  %4d\n", processQ[i]->ID, 
                   processQ[i]->AT, processQ[i]->TC,processQ[i]->CB, processQ[i]->IO, 
                   processQ[i]->PRIO,  processQ[i]->FT, processQ[i]->TT , processQ[i]->IT, 
                   processQ[i]->CW);
            
            cpu_util += processQ[i]->TC;
            avg_tt += processQ[i]->TT;
            avg_wt += processQ[i]->CW;
            
            if (FINISH < processQ[i]->FT){
                FINISH = processQ[i]->FT;
            }	
        }
        printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",FINISH,cpu_util*100/FINISH,io_util*100/FINISH,avg_tt/size,
               		avg_wt/size,(double)(size*100)/FINISH);
        
    }
