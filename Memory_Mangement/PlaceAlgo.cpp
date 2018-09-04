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
#include "PlaceAlgo.h"
using namespace std;
PlaceAlgo::PlaceAlgo(){
    ofs = 0;
    Frame_counter = 0;
    FrameIndex = 0;
    FrameSize = 0;
}

void PlaceAlgo::initialAge(){}

//int c::getPage(vector<int>& Frame,vector<int>& pagetable){};


int PlaceAlgo:: myRandom(int burst){
    if(ofs < randVals.size()){
        return (randVals[ofs++]%burst);
    }else{
        ofs = 0;
        return (randVals[ofs++]%burst);
    }
}

//NRU

int NRU:: getPage(vector<int>& Frame,vector<PTE>& pagetable){
    //initialize class
    vector<vector<PTE> > classes(4);
    for (int i = 0; i < pagetable.size();i++){
        if(pagetable[i].present == 1){
            int r = pagetable[i].referenced;
            int m = pagetable[i].modified;
            if (r == 0 && m== 0){
                classes[0].push_back(pagetable[i]);
            }else if(r== 0 && m == 1){
                classes[1].push_back(pagetable[i]);

            }else if(r==1 && m==0){
                classes[2].push_back(pagetable[i]);

            } else {
                classes[3].push_back(pagetable[i]);

            }
        }
    }
    

    //get Framenum
    for (int i = 0;i<classes.size(); i++) {
        if(classes[i].size() > 0){
            int ranNum = myRandom((int)classes[i].size());
            FrameIndex = (classes[i][ranNum]).f_index;

            break;
        }
    }
    clock++;
    if(clock == 10){
        clock = 0;
        for (int i = 0; i < pagetable.size(); i++){
            if((pagetable[i]).present){
                (pagetable[i]).referenced = 0;
            }
        }
    }

    return FrameIndex;
}


int Random::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    int FrameIndex = myRandom(FrameSize);
    return FrameIndex;
}

int FIFO::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    FrameIndex = Frame_counter;
    Frame_counter++;
    if(Frame_counter == FrameSize){
        Frame_counter = 0;
    }
    
    return FrameIndex;
}

int Second_Chance::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    bool flag = true;
    int pageIndex;
    while(flag){
        pageIndex = Frame[Frame_counter];
        FrameIndex = pagetable[pageIndex].f_index;
       if(pagetable[pageIndex].referenced == 0){
           flag = false;
        }else{
            pagetable[pageIndex].referenced = 0;
        }
        Frame_counter = (Frame_counter+1)%FrameSize;
    }
    return FrameIndex;
}


int Clock_P::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    bool flag = true;
    int pageIndex;
    FrameIndex = 0;
    while(flag){
        pageIndex = Frame[hand];
        FrameIndex = pagetable[pageIndex].f_index;
        if (pagetable[pageIndex].referenced == 1){
            pagetable[pageIndex].referenced = 0;

        } else{
            //FrameIndex = hand;
            flag = false;
        }
        hand = (hand+1)%FrameSize;
    }
    return FrameIndex;
}


int Clock_V::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    bool flag  = true;
    FrameIndex = 0;
    while(flag){
        if((pagetable[hand]).present == 1){
            if((pagetable[hand]).referenced == 0){
                FrameIndex = (pagetable[hand]).f_index;
                flag = false;
            } else{
                (pagetable[hand]).referenced = 0;
            }
        }
        hand = (hand+1)% pagetable.size();
    }
    return FrameIndex;
}

void Aging_P::initialAge(){
    //for(int i = 0; i < FrameSize;i++){
      //  cout<< "1000000000" << FrameSize<<endl;
    age = vector<unsigned int>(FrameSize, 0);

       // age[i] = 0;
    //    cout<< "0000000000" <<endl;
  //  }
}

int Aging_P::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    unsigned int minAge = 0xffffffff;
    int min = -1;
    int PtIndex;
   // cout <<"1"<<endl;
    for (int i = 0; i < FrameSize;i++){
        age[i] = age[i] >> 1;
        PtIndex = Frame[i];
        if (pagetable[PtIndex].referenced == 1) {
            age[i] |=(1 << 31);
            pagetable[PtIndex].referenced = 0;
          //  cout <<"2" <<endl;
        }
        if(age[i] < minAge){
            minAge = age[i];
            min = i;
        }
    }
   // counter = (counter+1)% FrameSize;
    if (min == -1){
        return 0;
    }
    age[min] = 0;
    return min;

}

int Aging_V::getPage(vector<int>& Frame,vector<PTE>& pagetable){
    int min = -1;
    unsigned int minAge = 0xffffffff;
    
    for(int i = 0; i <pagetable.size();i++){
        if ((pagetable[i]).present == 1) {
            age[i] = age[i] >> 1;
            
            if ((pagetable[i]).referenced == 1) {
                age[i] |= 1 << 31;
                (pagetable[i]).referenced = 0;
            }
            if(age[i] < minAge){
                minAge = age[i];
                min = i;
            }
        }
    }
    age[min] = 0;
    return (pagetable[min]).f_index;
}
