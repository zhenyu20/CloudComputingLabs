//
// Created by wenkai on 2021/6/15.
//

#ifndef PROJECT1_ENCODE_DECODE_H
#define PROJECT1_ENCODE_DECODE_H

#include "headSet.h"

class De_encode
{
private:
public:

    //return a bulk string message  eg. CS06142 -> $7\r\nCS06142\r\n
    std::string getStringMessage(std::string & raw);

    //return a RESP message eg. Cloud Computing-> *2\r\n$5\r\nCloud\r\n$9\r\nComputing\r\n
    std::string getRESPArry(std::vector<std::string> & raw);
    std::string getRESPArry(std::string raw);

    std::string getSuccessMessage();//return a success message


    std::string getErrorMessage();//return an error message


    //return a integer message  eg. 10 -> :10\r\n
    std::string getIntegerMessage(int raw);


    //decode the raw bulk message received eg. $7\r\nCS06142\r\n -> CS06142
    std::string de_encodeStringMessage(char * message, int * length=nullptr);

    //decode the raw integer message received eg. :10\r\n -> 10
    int de_encodeIntegerMessage(char * message, int * length=nullptr);

    //decode the raw status message received eg. +OK\r\n -> true  -ERROR\r\n -> false
    bool de_encodeStatusMessage(char * message, int * length=nullptr);

    //decode the raw RESP message received eg. *2\r\n$5\r\nCloud\r\n$9\r\nComputing\r\n -> Cloud Computing
    void de_encodeRESPArry(char * message, std::vector<std::string> & rc, int * length=nullptr);

};


#endif //PROJECT1_ENCODE_DECODE_H