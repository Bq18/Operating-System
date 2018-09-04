#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "schedulers.h"

using namespace std;

int total_time = 0;
int tot_movement = 0;
double avg_turnaround = 0.0;
double avg_waittime = 0.0;
int max_waittime = 0;

vector<ioOperation> vectorIO;
unsigned int totalIONum = 0;

int main(int argc, char** argv)
{
	char opt;
	char s_argv;
	bool b_print_detail = false;
	bool b_print_result = false;
	
	schedulers *sched = NULL;
	
	//argvs																-(1)
	while((opt = getopt(argc, argv, "s:vd")) != EOF)
	{
		switch(opt)
		{
			case 's':
				s_argv = optarg[0];
				switch(s_argv)
				{
					case 'i':
						sched = new fifo();
						break;
					case 'j':
						sched = new sstf();
						break;
					case 's':
						sched = new scan();
						break;
					case 'c':
						sched = new cscan();
						break;
					case 'f':
						sched = new fscan();
						break;
					default:
						cerr<<"error."<<endl;
						return 1;
				}
				break;
			case 'v':
				b_print_detail = true;
				break;
			case 'd':
				b_print_result = true;
				break;
			default:
				cerr<<"error."<<endl;
				return 1;
		}
	}
	
	//input file														-(2)
	ifstream file_stream;
	file_stream.open(argv[argc - 1]);
	if(!file_stream)
	{
		cerr<<"error."<<endl;
		return 1;
	}
	
	//push to vectorIO													-(3)
	string line = "";
	int i = 0;
	while(getline(file_stream,line))
    {
        if(line[0] == '#')
        {
            continue;
        }
        ioOperation ioo;
        ioo.seqNum = i++;
        stringstream ss;
        ss<<line;
        ss>>ioo.timestep>>ioo.destPosition;
        
        vectorIO.push_back(ioo);
    }
    totalIONum = vectorIO.size();
    
    //processing 														-(4)
    if(b_print_detail)
    {
		cout<<"TRACE"<<endl;
	}
	
	unsigned int ioNum = vectorIO.size();
	unsigned int ioSeqNum = 0;
	bool ioOngoing = false;
	int systemTime = 0;
	int totalWaitTime = 0;
	int totalTurnAround = 0;
	int lastPosition = 0;
	int nowPosition = 0;
	int nextTime = 0;
	
	ioOperation activeIO;
	
	while(ioNum > 0)
	{
		//if there none active I/O ongoing. 'issue' an I/O
		if(!ioOngoing)
		{
			//if empty, 'add'
			if(ioSeqNum<vectorIO.size() && sched->runtimeQueueEmpty())
			{
				ioOperation ioTemp = vectorIO.at(ioSeqNum++);
				systemTime = ioTemp.timestep;
				sched->runtimeQueue->push_back(ioTemp);
				
				if(b_print_detail)
				{
					cout<<systemTime<<":\t"<<ioTemp.seqNum;
					cout<<"\tadd\t"<<ioTemp.destPosition<<endl;
				}
			}
			//not empty, 'issue'
			activeIO = sched->getIOAccordingPolicy(lastPosition, nowPosition);
			ioOngoing = true;
			int ioWait = systemTime - activeIO.timestep;
			totalWaitTime += ioWait;
			max_waittime = (ioWait > max_waittime? ioWait : max_waittime);
				
			if(b_print_detail)
			{
				cout<<systemTime<<":\t"<<activeIO.seqNum;
				cout<<"\tissue\t"<<activeIO.destPosition<<"\t"<<nowPosition<<endl;
			}
					
			vectorIO[activeIO.seqNum].issueTime = systemTime;
			int move = abs(nowPosition-activeIO.destPosition);
			tot_movement += move;
			nextTime = systemTime + move;
			
			if(nowPosition != activeIO.destPosition)
			{
				lastPosition = nowPosition;
				nowPosition = activeIO.destPosition;
			}	
		}
		//if io Ongoing
		else
		{
			//add 
			while(ioSeqNum<vectorIO.size() && vectorIO[ioSeqNum].timestep <= nextTime)
			{
				ioOperation ioTemp = vectorIO.at(ioSeqNum++);
				systemTime = ioTemp.timestep;
				
				if(s_argv != 'f')
					sched->runtimeQueue->push_back(ioTemp);
				else
					sched->anotherQueue->push_back(ioTemp);
				
				if(b_print_detail)
				{
					cout<<systemTime<<":\t"<<ioTemp.seqNum;
					cout<<"\tadd\t"<<ioTemp.destPosition<<endl;
				}
			}
			//finish 
			ioOngoing = false;
			ioNum--;
			if(s_argv == 'f' && sched->runtimeQueueEmpty())
			{
				sched->exchangeQueue();
				lastPosition = nowPosition;
			}
			
			systemTime = nextTime;
			int turnAround = systemTime - activeIO.timestep;
			vectorIO[activeIO.seqNum].finishTime = systemTime;
			totalTurnAround += turnAround;
			total_time = systemTime;
			
			if(b_print_detail)
			{
				cout<<systemTime<<":\t"<<activeIO.seqNum;
				cout<<"\tfinish\t"<<turnAround<<endl;
			}
		}
	}
	
	// print result														-(5)
	avg_turnaround = (double)totalTurnAround / vectorIO.size();
	avg_waittime = (double)totalWaitTime / vectorIO.size();
	
	if(b_print_result)
	{
		cout<<"IOREQS INFO"<<endl;
		for(unsigned int i=0; i<vectorIO.size(); i++)
		{
			cout<<i<<":\t"<<vectorIO[i].timestep;
			cout<<"\t"<<vectorIO[i].issueTime;
			cout<<"\t"<<vectorIO[i].finishTime<<endl;
		}
	}
	
	printf("SUM: %d %d %.2lf %.2lf %d\n",
		total_time,
		tot_movement,
		avg_turnaround,
		avg_waittime,
		max_waittime);
		
    return 0;
}
