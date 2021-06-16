//
// Created by wenkai on 2021/6/16.
//

#ifndef PROJECT1_NETWORK_H
#define PROJECT1_NETWORK_H


#include "headSet.h"
#include "ossSocket.hpp"
#include "util.h"

#define NETWORK_TIMEOUT     2000000
#define NETWORK_HEADER_LEN  (sizeof(char) + sizeof(uint64_t))

/*
    The network is designed to abstract the socket into the application
    level in case of the chaos of ptr.

    In the network module, all the information will be sent in the format
    like this:
        [  *   ] : char
        [ Size ] : uint64_t, the size of the packet
        [ Str  ] : char * , the content of the packet
*/

typedef enum NetworkMode : uint8_t
{
    NETWORK_MODE_INVALID,
    NETWORK_MODE_CLIENT,
    NETWORL_MODE_SERVER
} NetworkMode;


class Network
{
private:

    std::string NETWORK_CON_PACK = "~" + numtob<uint64_t>(0);

    ossSocket * _peer = nullptr;
    ossSocket * _server = nullptr;

    // for the coordinator
    std::string _peerAddress;
    unsigned int _peerPort;

    // for the participant
    unsigned int _port;

public:
    Network() {}
    Network(std::string address, unsigned int port)
    {
        _peerAddress = address;
        _peerPort = port;

    }

    Network(unsigned int port)
    {
        _port = port;
    }

    ~Network()
    {
        close();
    }

    // common use
    int close() {
        if(_peer != nullptr) {
            delete _peer;
            _peer = nullptr;
        }

        if(_server != nullptr) {
            delete _server;
            _server = nullptr;
        }
        return KV_OK;
    }

    int recvCommand(std::vector<std::string> & content);
    int sendResult(std::string str);
    int acceptWithoutCloseBind();
    int initBind();

    // for the participant
    int send(std::string msg);
    int recv(std::string & content);
    int reaccept();

    // for the coordinator
    int sendAndRecv(std::string smsg, std::string & rmsg);
    int reconnect();

};

#endif //PROJECT1_NETWORK_H
