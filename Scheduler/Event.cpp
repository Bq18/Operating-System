#include<string>
#include<iostream>

#include "event.h"
Event::Event(){
    this->evtProcess = NULL;
    this->timestamp = 0;
    this->transition = "";
}


Event::Event (Process* evtProcess, int Timestamp, string transition){
    this->evtProcess = evtProcess;
    this->timestamp = Timestamp;
    this->transition = transition;
}
