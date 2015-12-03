//
// Author: Sheng Wei
// File Name: main.cpp
// File Description: main file of cache simulator project.
//
//
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "cache.h"
using namespace std;


int main( int argc, char* argv[] )
{
    if  (argc != 2)
    cerr << "The arguments count is not correct." << endl;
    else
    {
        Cache c;
        c.read_file(argv[1]);
        c.parse();
        c.convert_str_to_int();
        c.simulate(1024,8);
        c.simulate(1024,16);
        c.simulate(1024,32);
        c.simulate(1024,128);
        c.simulate(4096,8);
        c.simulate(4096,16);
        c.simulate(4096,32);
        c.simulate(4096,128);
        c.simulate(65536,8);
        c.simulate(65536,16);
        c.simulate(65536,32);
        c.simulate(65536,128);
        c.simulate(131072,8);
        c.simulate(131072,16);
        c.simulate(131072,32);
        c.simulate(131072,128);
        c.print_line(argv[1]);
    }
    return 0;

}
