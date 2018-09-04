#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class ioOperation
{
	public:
		int seqNum; //sequence number
		int timestep;
		int destPosition;
		//int addTime; is timestep
		int issueTime;
		int finishTime;
		ioOperation()
		{
			this->seqNum = 0;
			this->timestep = 0;
			this->destPosition = 0;
			this->issueTime = 0;
			this->finishTime = 0;
		}
		//overload '<'
		bool operator < (const ioOperation &v)const {
                return this->destPosition < v.destPosition;
        }
};
class schedulers
{
	public:
		vector<ioOperation> firstQueue;
		vector<ioOperation> secondQueue;
		
		vector<ioOperation>* runtimeQueue;
		vector<ioOperation>* anotherQueue;
		int flag; // runtimeQueue pointer to 1queue or 2queue
		schedulers()
		{
			this->runtimeQueue = &(this->firstQueue);
			this->anotherQueue = &(this->secondQueue);
		}
		void exchangeQueue(){
			vector<ioOperation>* tmp = this->runtimeQueue;
			this->runtimeQueue = this->anotherQueue;
			this->anotherQueue = tmp;
		}
		
		bool runtimeQueueEmpty()
		{
			if(this->runtimeQueue->empty())
				return true;
			else
				return false;
		}
		virtual ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0) = 0;
		
};

class fifo : public schedulers
{
	public:
		ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0)
		{
			ioOperation ioo = this->runtimeQueue->front();
			this->runtimeQueue->erase(this->runtimeQueue->begin());
			return ioo;
		}
};

class sstf : public schedulers
{
	public:
		ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0)
		{
			int minimizeSeekTime = abs(this->runtimeQueue->at(0).destPosition - nowPos);
			int num = 0;
			for(unsigned int i=1; i<this->runtimeQueue->size(); i++)
			{
				int time = abs(this->runtimeQueue->at(i).destPosition - nowPos);
				if(time < minimizeSeekTime)
				{
					minimizeSeekTime = time;
					num = i;
				}
			}
			ioOperation ioo = this->runtimeQueue->at(num);
			this->runtimeQueue->erase(this->runtimeQueue->begin() + num);
			return ioo;
		}
};

class scan : public schedulers
{
	public:
		ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0)
		{
			//int direct = lastPos<=nowPos? 1:0; //right?left?
			sort(this->runtimeQueue->begin(), this->runtimeQueue->end());			
			unsigned int i = 0;
			for(; i<this->runtimeQueue->size(); i++)
			{
				if(this->runtimeQueue->at(i).destPosition >= nowPos)
					break;
			}
			
			int num = 0;
			if(i == this->runtimeQueue->size())					//last
				num = i -1;
			else if(i == 0 || this->runtimeQueue->at(i).destPosition == nowPos) //fist
				num = i;
			else 														//middle
				num = lastPos<=nowPos? i:i-1;
				
			ioOperation ioo = this->runtimeQueue->at(num);
			this->runtimeQueue->erase(this->runtimeQueue->begin() + num);
			return ioo;
		}
};

class cscan : public schedulers
{
	public:
		ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0)
		{
			//int direct = lastPos<=nowPos? 1:0; //right?left?
			sort(this->runtimeQueue->begin(), this->runtimeQueue->end());			
			unsigned int i = 0;
			for(; i<this->runtimeQueue->size(); i++)
			{
				if(this->runtimeQueue->at(i).destPosition >= nowPos)
					break;
			}
					
			if(i == this->runtimeQueue->size())					//last
				i = 0;
				
			ioOperation ioo = this->runtimeQueue->at(i);
			this->runtimeQueue->erase(this->runtimeQueue->begin() + i);
			return ioo;
		}
};
class fscan : public schedulers
{
	public:
		ioOperation getIOAccordingPolicy(int lastPos = 0, int nowPos = 0)
		{
			//int direct = lastPos<=nowPos? 1:0; //right?left?
			sort(this->runtimeQueue->begin(), this->runtimeQueue->end());			
			unsigned int i = 0;
			for(; i<this->runtimeQueue->size(); i++)
			{
				if(this->runtimeQueue->at(i).destPosition >= nowPos)
					break;
			}

			int num = 0;
			if(i == this->runtimeQueue->size())					//last
				num = i -1;
			else if(i == 0 || this->runtimeQueue->at(i).destPosition == nowPos) //fist
				num = i;
			else 														//middle
				num = lastPos<=nowPos? i:i-1;
				
			ioOperation ioo = this->runtimeQueue->at(num);
			this->runtimeQueue->erase(this->runtimeQueue->begin() + num);
			
			return ioo;
		}
};

