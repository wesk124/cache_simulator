// Author: Sheng Wei
// File Name: cache.h
// File Description: Header File of Cache class
//





#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>
using namespace std;

class Cache
{


    
    private:

    vector <string> line_vector;
    vector <string> inst_vector;
    vector <string> address_vector;
    vector <int> address_int_vector;
    vector <string> everything_vector;


    public:
    Cache();
   
    Cache(Cache & c);

    void read_file(string filename );
    
    void parse();

    void convert_str_to_int();
    
    string get_write_policy(bool wt_policy);

    void direct_map(int cache_size, int block_size, bool wt_policy);
    
    int lru_replace(vector< vector<int> > vec, int row_num, int col_num);

    void two_way(int cache_size, int block_size, bool _wt_policy);

    void four_way(int cache_size, int block_size, bool _wt_policy);

    void full_associative(int cache_size, int block_size, bool _wt_policy);
   

    void simulate(int in_cache_size, int in_block_size);

    string double_to_str(const double f_value);
    
    string get_line(int cache_size, int block_size, string map_type, string wt_policy, double hit_rate, int mem_to_cache, int cache_to_mem, int num_of_comp );

    void add_line(string wholeline);

    void print_line(string filename);

};

#endif

