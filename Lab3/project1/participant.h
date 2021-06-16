//
// Created by wenkai on 2021/6/16.
//

#ifndef PROJECT1_PARTICIPANT_H
#define PROJECT1_PARTICIPANT_H


#include "headSet.h"
#include "myLog.h"
#include "network.h"
#include "encode_decode.h"
#include "kv.h"

class Participant
{
private:

    // the local txid
    txid                _TXID;

    // network
    Network             _net;

    // the log
    Mylog              _lg;

    // kv
    KV                  _kvdb;

    // helper
    De_encode              de_en;

private:
    int keepAlive();
    int Working();

    std::string pWorker(std::string task);
    std::string eventParser(std::string event);

public:
    Participant() {};
    void Init(NodeInfo info);
    int Launch();
};


#endif //PROJECT1_PARTICIPANT_H
