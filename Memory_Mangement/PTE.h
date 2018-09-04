#ifndef PDE_h
#define PDE_h

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


using namespace std;

struct PTE{
   unsigned int present:1;
   unsigned int modified:1;
    unsigned int referenced:1;
    unsigned int pageout:1;
    unsigned int f_index:28;
    
    
};
#endif
