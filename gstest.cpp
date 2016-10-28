
//#define DK_TEST_TAG_DONOT_CALCULATE
#include "codesim.hxx"
#include "preprocessor.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
using namespace std;

char inp[505000]; //inited by 0.

int main()
{
    freopen("in.txt","r",stdin);
    int n=0;
    while((inp[n++]=(char)getchar())!=-1);
    inp[n-1]=0;
    string input(inp);
    //for(int i=0;i<input.size();i++) printf("%d ",(int)input[i]);
    preprocessor prox(input);
    string x=prox.processed_code();
    printf("%s\n==END==\n",x.data());
    //for(int i=0;i<x.size();i++) printf("%d ",(int)x[i]);
    freopen("out.txt","w",stdout);
    Simulator sim;
    sim.bind(x);
    vector<int> outs;
    sim.runSimulation(outs);


    if(outs.size()!=0) printf("%d",outs[0]);
    for(int i=1;i<(int)outs.size();i++) printf(" %d",outs[i]);

    return 0;
}
