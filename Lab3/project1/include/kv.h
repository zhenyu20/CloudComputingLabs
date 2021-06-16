//
// Created by wenkai on 2021/6/15.
//

#ifndef PROJECT1_KV_H
#define PROJECT1_KV_H

#include "headSet.h"
#include "encode_decode.h"
#include "phmap.h"

using phmap::parallel_flat_hash_map;

class KV
{
private:
    parallel_flat_hash_map<std::string, std::string> _kvdb;

public:

    ~KV();

    std::string kvGET(std::string & k);//get the value of key.If does'nt exist nil is returned.

    int kvDEL(std::string & k);//remove key-value
    int kvDELArry(std::vector<std::string> & ks);

    int kvSET(std::string & k, std::string & v);//set key to hold the string value(exist overwritten)

};


#endif //PROJECT1_KV_H
