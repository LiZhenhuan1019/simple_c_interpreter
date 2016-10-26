
#define DK_TEST_TAG_DONOT_CALCULATE
#include "codesim.hxx"

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
    printf("%s\n==END==\n",inp);
    freopen("out.txt","w",stdout);
    Simulator sim;
    sim.bind(string(inp));
    //vector<int> outs;
    //sim.runSimulation(outs);

    //while(true);







    return 0;
}
