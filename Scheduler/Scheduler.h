#ifndef SCHEDULER_H
#define SCHEDULER_H


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
#include "Event.h"

using namespace std;

class Scheduler{
   
public:
    Scheduler();//constructor
    string type;
    int quantum;
    deque <Process*> readyQ;
    
    virtual void addReadyQ(Process* );
    virtual Process* getProcess();
    virtual  void addExpireQ(Process*);

    
};


class FCFS : public Scheduler{
public:
    FCFS();//constructor;
    
    void addReadyQ(Process* );
    Process* getProcess();
    void addExpireQ(Process*);

};


class LCFS: public Scheduler{
public:
    LCFS();//constructor
    void addReadyQ(Process* );
    Process* getProcess();
    void addExpireQ(Process*);

};


class SJF:public Scheduler{
public:
    SJF();//constructor
    
    void addReadyQ(Process* );
    Process* getProcess();
    void addExpireQ(Process*);

};

class RR: public Scheduler{

public:
    RR(int );
    void addReadyQ(Process*);
    Process* getProcess();
    void addExpireQ(Process*);

};


class PRIO:public Scheduler{

public:
    deque<Process*> expireQ;
    PRIO(int );
    void addReadyQ(Process*);
    void addExpireQ(Process*);
    Process* getProcess();
};

#endif

