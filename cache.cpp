// Author: Sheng Wei
// File Name: cache.cpp
// File Description: implementation of functions in class Cache
//
//

#include "cache.h"

/* Constructor */

Cache::Cache()
{

}

/* Copy constructor */
Cache::Cache(Cache & c)
{

    
}

void Cache:: read_file(string filename)
{
    int index = 0;
    string line;
    string real_filename = filename+ ".trace";
    cout << real_filename << "\n";

    ifstream in_file (real_filename.c_str());

    for (line; getline(in_file, line);)
    {
       // cout << line << endl;
        line_vector.push_back(line);
        cout << line_vector[index] <<endl;
        index++;
    }    
}



void Cache::parse ()
{
    string in_string;
    string ins; // instruction parts of line
    string addr; // address part of line
    for (int i = 0; i < line_vector.size(); i++)
    {
       int flag = 0;
       in_string = line_vector[i];
       string buff; //buffer
       istringstream iss (in_string);
       
       while (iss >> buff)
       {
            cout << buff << endl;
            if (flag == 0)
            {
                 inst_vector.push_back(buff);
                 flag = 1;
            }
            else
            {
                 address_vector.push_back(buff);
            }
       }

 //      cout << "i: " <<inst_vector[i] << endl;
   //    cout << "a: " << address_vector[i] <<endl;
    }
         
}


void Cache::convert_str_to_int()
{
    
    for (int i = 0; i < address_vector.size(); i++)
    {
        int i_hex;
        i_hex = stoi(address_vector[i], nullptr, 0 );
        address_int_vector.push_back(i_hex);
//        cout << "the int address is: " << address_int_vector[i]<<endl;
    }


}


string Cache::get_write_policy(bool wt_policy)
{
   string str_wt_policy;
    if (wt_policy)
    {
        str_wt_policy = "WT";
    }
    else 
    {
        str_wt_policy = "WB";
    }
    return str_wt_policy;
}

void Cache::direct_map(int cache_size, int block_size, bool wt_policy)
{
    int _cache_size = cache_size;
    int _block_size = block_size;
    int _block_count =_cache_size/_block_size; //get block count
    int _cpr_count = 1;

    int _mem_to_cache = 0; // the byte transfer from memory to cache;
    int _cache_to_mem = 0; // the byte transfer from cache to memory;
   
    double _hit_count = 0;
    double _hit_ratio = 0;
    bool _wt_policy;
    string _str_wt_policy;
    string _mapping_type = "DM"; 
    vector<int> tag_vector(_block_count);
    vector<int> valid_vector(_block_count,0);
    
    _wt_policy = wt_policy;

    for (int i = 0; i < address_int_vector.size(); i++)
    {
      int curr_address = address_int_vector[i];
      string curr_inst = inst_vector[i];
      int block_index = (curr_address / _block_size ) % _block_count;
      int cache_tag = curr_address/_cache_size;
      
 //     cout << curr_address << endl;
   //   cout << block_index <<endl;
     // cout << cache_tag << endl;

      if (curr_inst == "read")
      {
       //     cout << "The instruction is read" << endl;
            if (tag_vector[block_index] == cache_tag)
            {
                _hit_count++;
                   
            }
            else
            {
                if (valid_vector[block_index] == 1)
                {
                    tag_vector[block_index] = cache_tag;
                    valid_vector[block_index] = 0;
                    _cache_to_mem += _block_size;
                    _mem_to_cache += _block_size;
                }
                else 
                {
                    tag_vector[block_index] = cache_tag;
                    _mem_to_cache += _block_size;
                }
            }
      }
      else if (curr_inst == "write")
      {
       //     cout << "The instruction is write" << endl;  
            if (_wt_policy)
            {
                if (tag_vector[block_index] == cache_tag) 
                {
                    _hit_count++;
                    _cache_to_mem += 4;
                }
                else 
                {
                   tag_vector[block_index] = cache_tag;
                   _mem_to_cache += _block_size;
                   _cache_to_mem += 4;
                }
            }

           else 
           {
            if (tag_vector[block_index] == cache_tag)
            {
                _hit_count++;
                valid_vector[block_index] = 1;
                   
            }
            else
            {
                if (valid_vector[block_index] == 1)
                {
                    tag_vector[block_index] = cache_tag;
                    _cache_to_mem += _block_size;
                    _mem_to_cache += _block_size;
                }
                else 
                {
                    tag_vector[block_index] = cache_tag;
                    valid_vector[block_index] = 1;
                    _mem_to_cache += _block_size;
                }
            }
           }
      }
      else
      {
            cerr << "incorrect instruction" << endl;
      }
    }

    for (int j = 0; j < valid_vector.size(); j++)
    {
        if (valid_vector[j] == 1)
        {
            _cache_to_mem += _block_size;
        }
    }

    _hit_ratio = _hit_count/address_int_vector.size();
    _str_wt_policy = get_write_policy(_wt_policy);

    cout << _cache_size << "  " << _block_size << "  " <<_mapping_type <<"  "<< _str_wt_policy << "  " << _hit_ratio << "  " << _mem_to_cache << "  " << _cache_to_mem << "  " << _cpr_count << endl;
    
    string out_line = get_line(_cache_size, _block_size, _mapping_type, _str_wt_policy, _hit_ratio, _mem_to_cache, _cache_to_mem, _cpr_count);
   // cout << out_line;
    add_line(out_line);
}


int Cache::lru_replace(vector < vector<int> > vec, int row_num, int col_num )
{
    int index = 0;
    for (int i = 0; i <row_num-1; i++)
    {
        if (vec[index][col_num] < vec[i+1][col_num])
        {
            index = i;
        }
        else
        {
            index = i+1;
        }
    return index;
    }
}
    
void Cache::two_way(int cache_size, int block_size, bool wt_policy)
{
    #define NUM_OF_WAYS 2


    int _cache_size = cache_size;
    int _block_size = block_size;
    int _block_count =_cache_size/_block_size; //get block count
    int _set_count = _block_count/NUM_OF_WAYS;//number of sets
    int _cpr_count = _block_count/_set_count;

    int _mem_to_cache = 0; // the byte transfer from memory to cache;
    int _cache_to_mem = 0; // the byte transfer from cache to memory;
    

    double _hit_count = 0;
    double _hit_ratio = 0;
    bool _wt_policy = wt_policy;
    int age = 0;
    string _str_wt_policy;
    _wt_policy = wt_policy;
    
    /* Use vector of vector for tracking tag, valid bit, and age bit  */
    vector < vector <int> > tag_vector;
    vector < vector <int> > valid_vector;           
    vector < vector <int> > age_vector;          
    
    /*using a foor loop to initialize cache,since the cache is empty */
    for (int way_index = 0; way_index <  NUM_OF_WAYS; way_index++)
    {
        vector<int> row1;
        vector<int> row2;
        vector<int> row3;
        for (int set_index = 0; set_index < _set_count; set_index++)
        {
            row1.push_back(-1);
            row2.push_back(0);
            row3.push_back(0);
        }
        tag_vector.push_back(row1);
        valid_vector.push_back(row2);
        age_vector.push_back(row3);
    }


    for (int i = 0; i < address_int_vector.size(); i++)
    {
      int curr_address = address_int_vector[i];
      string curr_inst = inst_vector[i];
      int block_index = (curr_address / _block_size ) % _block_count;
      int cache_tag = curr_address/_cache_size;
      
     // cout << curr_address << endl;
     // cout << block_index <<endl;
     // cout << cache_tag << endl;

      if (inst_vector[i] == "read")
      {
       // cout << "The instruction is read" << endl;
        bool is_hit = false;
        bool is_empty = false;

        for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
        {
            if (tag_vector[i_way][block_index] == cache_tag)
            {
                _hit_count++;
                age_vector[i_way][block_index] = age;
                is_hit = true;
                age++;
                break;
            }
        }

        if (!is_hit)
        {
               for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
               {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                       // valid_vector[i_way][block_index] = 0;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
               }
                
      
            if(!is_empty)
            {
                int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                if (valid_vector[index][block_index] == 0)
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    age++;
                }
                else
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += _block_size;
                    age++;
                }   
             }
      }
     } // end of read

        else if (inst_vector[i] == "write")
        {
           // cout << "The instruction is write" << endl;
            if (_wt_policy) // start write through
            {
                bool is_hit = false;
                bool is_empty = false;
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if(tag_vector[i_way][block_index] == cache_tag)
                    {
                        
                        _hit_count++;
                        age_vector[i_way][block_index] = age;
                        is_hit = true;
                        age++;
                        _cache_to_mem += 4;
                        break;
                    }

                }
            if(!is_hit)
            {
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        valid_vector[i_way][block_index] = 0;
                        age_vector[i_way][block_index] = age;
                        age++;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += 4;
                        is_empty = true;
                        break;
                    }
                }
            
                if (!is_empty)
                {
                    int index = 0;
                    index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    tag_vector[index][block_index] = cache_tag; 
                    valid_vector[index][block_index] = 0;
                    age_vector[index][block_index] = age;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += 4;
                    age++;
                
                }
            }

           }

           else
           {
             bool is_hit = false;
             bool is_empty = false;

             for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
             {
                 if (tag_vector[i_way][block_index] == cache_tag)
                 {
                    _hit_count++;
                    age_vector[i_way][block_index] = age;
                    valid_vector[i_way][block_index] = 1;
                    is_hit = true;
                    age++;
                    break;
                }
            }

            if (!is_hit)
            {
                for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                        valid_vector[i_way][block_index] = 1;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
                }    
                
      
                if(!is_empty)
                {
                    int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    if (valid_vector[index][block_index] == 0)
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        age++;
                    }
                    else
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += _block_size;
                        age++;
                    }   
                }

            }
            }
        }     
           
      }



    if (!_wt_policy)
    {
        for (int row = 0; row < NUM_OF_WAYS; row++)
            {
                for (int col = 0; col < _set_count; col++)
                {
                    if ( valid_vector[row][col] == 1)
                    _cache_to_mem += block_size;

                }
            }
    }
    

    #undef NUM_OF_WAYS
    string  _mapping_type = "2W";
    _hit_ratio = _hit_count/address_int_vector.size();
    _str_wt_policy = get_write_policy(_wt_policy);

    
    string out_line = get_line(_cache_size, _block_size, _mapping_type, _str_wt_policy, _hit_ratio, _mem_to_cache, _cache_to_mem, _cpr_count);
    add_line(out_line);

     cout << _cache_size << "  " << _block_size << "  " <<_mapping_type <<"  "<< _str_wt_policy << "  " << _hit_ratio << "  " << _mem_to_cache << "  " << _cache_to_mem << "  " << _cpr_count << endl;
}

void Cache::four_way(int cache_size, int block_size, bool wt_policy)
{
    #define NUM_OF_WAYS 4

    int _cache_size = cache_size;
    int _block_size = block_size;
    int _block_count =_cache_size/_block_size; //get block count
    int _set_count = _block_count/NUM_OF_WAYS;//number of sets
    int _cpr_count = _block_count/_set_count;

    int _mem_to_cache = 0; // the byte transfer from memory to cache;
    int _cache_to_mem = 0; // the byte transfer from cache to memory;

    double _hit_count = 0;
    double _hit_ratio = 0;
    bool _wt_policy = wt_policy;
    int age = 0;
    string _str_wt_policy;
    _wt_policy = wt_policy;
    
    /* Use vector of vector for tracking tag, valid bit, and age bit  */
    vector < vector <int> > tag_vector;
    vector < vector <int> > valid_vector;           
    vector < vector <int> > age_vector;          
    
    /*using a foor loop to initialize cache,since the cache is empty */
    for (int way_index = 0; way_index <  NUM_OF_WAYS; way_index++)
    {
        vector<int> row1;
        vector<int> row2;
        vector<int> row3;
        for (int set_index = 0; set_index < _set_count; set_index++)
        {
            row1.push_back(-1);
            row2.push_back(0);
            row3.push_back(0);
        }
        tag_vector.push_back(row1);
        valid_vector.push_back(row2);
        age_vector.push_back(row3);
    }


    for (int i = 0; i < address_int_vector.size(); i++)
    {
      int curr_address = address_int_vector[i];
      string curr_inst = inst_vector[i];
      int block_index = (curr_address / _block_size ) % _block_count;
      int cache_tag = curr_address/_cache_size;
      
     // cout << curr_address << endl;
     // cout << block_index <<endl;
     // cout << cache_tag << endl;

      if (inst_vector[i] == "read")
      {
       // cout << "The instruction is read" << endl;
        bool is_hit = false;
        bool is_empty = false;

        for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
        {
            if (tag_vector[i_way][block_index] == cache_tag)
            {
                _hit_count++;
                age_vector[i_way][block_index] = age;
                is_hit = true;
                age++;
                break;
            }
        }

        if (!is_hit)
        {
               for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
               {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                       // valid_vector[i_way][block_index] = 0;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
               }
                
      
            if(!is_empty)
            {
                int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                if (valid_vector[index][block_index] == 0)
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    age++;
                }
                else
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += _block_size;
                    age++;
                }   
             }
      }
     } // end of read

        else if (inst_vector[i] == "write")
        {
           // cout << "The instruction is write" << endl;
            if (_wt_policy) // start write through
            {
                bool is_hit = false;
                bool is_empty = false;
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if(tag_vector[i_way][block_index] == cache_tag)
                    {
                        
                        _hit_count++;
                        age_vector[i_way][block_index] = age;
                        is_hit = true;
                        age++;
                        _cache_to_mem += 4;
                        break;
                    }

                }
            if(!is_hit)
            {
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        valid_vector[i_way][block_index] = 0;
                        age_vector[i_way][block_index] = age;
                        age++;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += 4;
                        is_empty = true;
                        break;
                    }
                }
            
                if (!is_empty)
                {
                    int index = 0;
                    index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    tag_vector[index][block_index] = cache_tag; 
                    valid_vector[index][block_index] = 0;
                    age_vector[index][block_index] = age;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += 4;
                    age++;
                
                }
            }

           }

           else
           {
             bool is_hit = false;
             bool is_empty = false;

             for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
             {
                 if (tag_vector[i_way][block_index] == cache_tag)
                 {
                    _hit_count++;
                    age_vector[i_way][block_index] = age;
                    valid_vector[i_way][block_index] = 1;
                    is_hit = true;
                    age++;
                    break;
                }
            }

            if (!is_hit)
            {
                for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                        valid_vector[i_way][block_index] = 1;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
                }    
                
      
                if(!is_empty)
                {
                    int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    if (valid_vector[index][block_index] == 0)
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        age++;
                    }
                    else
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += _block_size;
                        age++;
                    }   
                }

            }
            }
        }     
           
      }


    if (!_wt_policy)
    {
        for (int row = 0; row < NUM_OF_WAYS; row++)
            {
                for (int col = 0; col < _set_count; col++)
                {
                    if ( valid_vector[row][col] == 1)
                    _cache_to_mem += block_size;

                }
            }
    }

    #undef NUM_OF_WAYS
    string  _mapping_type = "4W";

    _hit_ratio = _hit_count/address_int_vector.size();
    _str_wt_policy = get_write_policy(_wt_policy);

    
    string out_line = get_line(_cache_size, _block_size, _mapping_type, _str_wt_policy, _hit_ratio, _mem_to_cache, _cache_to_mem, _cpr_count);
    add_line(out_line);

     cout << _cache_size << "  " << _block_size << "  " <<_mapping_type <<"  "<< _str_wt_policy << "  " << _hit_ratio << "  " << _mem_to_cache << "  " << _cache_to_mem << "  " << _cpr_count << endl;
}




void Cache::full_associative(int cache_size, int block_size, bool wt_policy)
{
    


    int _cache_size = cache_size;
    int _block_size = block_size;
    int _block_count =_cache_size/_block_size; //get block count
    int NUM_OF_WAYS = _block_count;
    int _set_count = _block_count/NUM_OF_WAYS;//number of sets
    int _cpr_count = _block_count/_set_count;
    int _mem_to_cache = 0; // the byte transfer from memory to cache;
    int _cache_to_mem = 0; // the byte transfer from cache to memory;
    

    double _hit_count = 0;
    double _hit_ratio = 0;
    bool _wt_policy = wt_policy;
    int age = 0;
    string _str_wt_policy;
    _wt_policy = wt_policy;
    
    /* Use vector of vector for tracking tag, valid bit, and age bit  */
    vector < vector <int> > tag_vector;
    vector < vector <int> > valid_vector;           
    vector < vector <int> > age_vector;          
    
    /*using a foor loop to initialize cache,since the cache is empty */
    for (int way_index = 0; way_index <  NUM_OF_WAYS; way_index++)
    {
        vector<int> row1;
        vector<int> row2;
        vector<int> row3;
        for (int set_index = 0; set_index < _set_count; set_index++)
        {
            row1.push_back(-1);
            row2.push_back(0);
            row3.push_back(0);
        }
        tag_vector.push_back(row1);
        valid_vector.push_back(row2);
        age_vector.push_back(row3);
    }


    for (int i = 0; i < address_int_vector.size(); i++)
    {
      int curr_address = address_int_vector[i];
      string curr_inst = inst_vector[i];
      int block_index = (curr_address / _block_size ) % _block_count;
      int cache_tag = curr_address/_cache_size;
      
     // cout << curr_address << endl;
     // cout << block_index <<endl;
     // cout << cache_tag << endl;

      if (inst_vector[i] == "read")
      {
       // cout << "The instruction is read" << endl;
        bool is_hit = false;
        bool is_empty = false;

        for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
        {
            if (tag_vector[i_way][block_index] == cache_tag)
            {
                _hit_count++;
                age_vector[i_way][block_index] = age;
                is_hit = true;
                age++;
                break;
            }
        }

        if (!is_hit)
        {
               for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
               {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                       // valid_vector[i_way][block_index] = 0;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
               }
                
      
            if(!is_empty)
            {
                int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                if (valid_vector[index][block_index] == 0)
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    age++;
                }
                else
                {
                    tag_vector[index][block_index] = cache_tag;
                    age_vector[index][block_index] = age;
                    valid_vector[index][block_index] = 0;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += _block_size;
                    age++;
                }   
             }
      }
     } // end of read

        else if (inst_vector[i] == "write")
        {
           // cout << "The instruction is write" << endl;
            if (_wt_policy) // start write through
            {
                bool is_hit = false;
                bool is_empty = false;
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if(tag_vector[i_way][block_index] == cache_tag)
                    {
                        
                        _hit_count++;
                        age_vector[i_way][block_index] = age;
                        is_hit = true;
                        age++;
                        _cache_to_mem += 4;
                        break;
                    }

                }
            if(!is_hit)
            {
                for (int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        valid_vector[i_way][block_index] = 0;
                        age_vector[i_way][block_index] = age;
                        age++;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += 4;
                        is_empty = true;
                        break;
                    }
                }
            
                if (!is_empty)
                {
                    int index = 0;
                    index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    tag_vector[index][block_index] = cache_tag; 
                    valid_vector[index][block_index] = 0;
                    age_vector[index][block_index] = age;
                    _mem_to_cache += _block_size;
                    _cache_to_mem += 4;
                    age++;
                
                }
            }

           }

           else
           {
             bool is_hit = false;
             bool is_empty = false;

             for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
             {
                 if (tag_vector[i_way][block_index] == cache_tag)
                 {
                    _hit_count++;
                    age_vector[i_way][block_index] = age;
                    valid_vector[i_way][block_index] = 1;
                    is_hit = true;
                    age++;
                    break;
                }
            }

            if (!is_hit)
            {
                for(int i_way = 0; i_way < NUM_OF_WAYS; i_way++)
                {
                    if (tag_vector[i_way][block_index] == -1)
                    {
                        tag_vector[i_way][block_index] = cache_tag;
                        age_vector[i_way][block_index] = age;
                        valid_vector[i_way][block_index] = 1;
                        age++;
                        _mem_to_cache += _block_size;
                        is_empty = true;
                        break;
                    }
                }    
                
      
                if(!is_empty)
                {
                    int index = lru_replace(age_vector, NUM_OF_WAYS, block_index);
                    if (valid_vector[index][block_index] == 0)
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        age++;
                    }
                    else
                    {
                        tag_vector[index][block_index] = cache_tag;
                        age_vector[index][block_index] = age;
                        valid_vector[index][block_index] = 1;
                        _mem_to_cache += _block_size;
                        _cache_to_mem += _block_size;
                        age++;
                    }   
                }

            }
            }
        }     
           
      }

    if (!_wt_policy)
    {
        for (int row = 0; row < NUM_OF_WAYS; row++)
            {
                for (int col = 0; col < _set_count; col++)
                {
                    if ( valid_vector[row][col] == 1)
                    _cache_to_mem += block_size;

                }
            }
    }


    string  _mapping_type = "FA";

    

    _hit_ratio = _hit_count/address_int_vector.size();
    _str_wt_policy = get_write_policy(_wt_policy);

    
    string out_line = get_line(_cache_size, _block_size, _mapping_type, _str_wt_policy, _hit_ratio, _mem_to_cache, _cache_to_mem, _cpr_count);
    add_line(out_line);

     cout << _cache_size << "  " << _block_size << "  " <<_mapping_type <<"  "<< _str_wt_policy << "  " << _hit_ratio << "  " << _mem_to_cache << "  " << _cache_to_mem << "  " << _cpr_count << endl;
}



void Cache::simulate(int in_cache_size, int in_block_size)
{
    bool in_wt_policy = false;
    direct_map(in_cache_size, in_block_size, in_wt_policy);
    direct_map(in_cache_size, in_block_size, !in_wt_policy);
    two_way(in_cache_size, in_block_size, in_wt_policy);
    two_way(in_cache_size, in_block_size, !in_wt_policy);
    four_way(in_cache_size, in_block_size, in_wt_policy);
    four_way(in_cache_size, in_block_size, !in_wt_policy);
    full_associative(in_cache_size, in_block_size, in_wt_policy);
    full_associative(in_cache_size, in_block_size, !in_wt_policy);
}




string Cache::double_to_str(const double f_val)
{
    ostringstream out;
    out << setprecision (2) << f_val;
    return out.str();
}


string Cache::get_line(int cache_size, int block_size, string map_type, string wt_policy, double hit_rate, int mem_to_cache, int cache_to_mem, int num_of_comp)
{
string single_line;
string str_two_dec = double_to_str(hit_rate);
single_line = to_string(cache_size) + '\t' + to_string (block_size) + '\t' + map_type +'\t'+ wt_policy+'\t' + str_two_dec+'\t' + to_string(mem_to_cache) +'\t'+ to_string(cache_to_mem) +'\t'+to_string (num_of_comp) + '\n';
return single_line;

}



void Cache::add_line(string wholeline)
{
    everything_vector.push_back(wholeline);   
}


void Cache:: print_line(string filename)
{
    string realname = filename+".result";
    ofstream out_file;
    out_file.open (realname);
    for (int i = 0; i < everything_vector.size(); i++)
    {
        out_file << everything_vector[i];
    }
    cout << "All lines have been printed out" << endl;

}
