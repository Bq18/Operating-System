#ifndef EVENT_H
#define EVENT_H
#include<string>
#include<iostream>

#include "Process.h"

using namespace std;

class Event{
    
public:
    Event();
    Event(Process*, int, string);
    Process* evtProcess;
    int timestamp;
    string transition;
    
    
};
#endif
