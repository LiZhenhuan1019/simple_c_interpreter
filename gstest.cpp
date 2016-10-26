
#define DK_TEST_TAG_DONOT_CALCULATE
#include "codesim.hxx"

#include <iostream>
#include <fstream>
#include <cstdio>
using namespace std;

int main()
{
    freopen("in.txt","r",stdin);
    string inp;
    char c;
    while((c=getchar())!=-1)
    inp.append(1,c);

    vector<int> outs;

    Simulator sim;
    sim.bind(inp);
    sim.runSimulation(outs);

    return 0;
}
