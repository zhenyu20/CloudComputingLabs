//
// Created by wenkai on 2021/6/15.
//
#include "include/headSet.h"
#include "include/kv.h"//include more comments


KV:: ~KV()
{
    _kvdb.clear();
}

int KV::kvSET(std::string & k, std::string & v)
{
    _kvdb[k] = v;
    return KV_OK;
}


int KV::kvDEL(std::string & k)
{
    auto iter = _kvdb.find(k);
    if(iter != _kvdb.end()) {
        _kvdb.erase(iter);
        return KV_OK;
    } else {
        return KV_DEL;
    }
}

int KV::kvDELArry(std::vector<std::string> & ks)
{
    int number = 0;
    for(auto & k : ks)
    {
        if(kvDEL(k) == KV_OK)
            number ++;
    }
    return number;
}


std::string KV::kvGET(std::string & k)
{
    auto iter = _kvdb.find(k);
    if(iter == _kvdb.end()) return "nil";
    else return iter->second;
}