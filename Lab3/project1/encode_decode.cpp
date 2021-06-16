// Created by wenkai on 2021/6/15.
// encode&decode RESP message
//eg.
//SET CS06142 "Cloud Computing" -> `*4\r\n$3\r\nSET\r\n$7\r\nCS06142\r\n$5\r\nCloud\r\n$9\r\nComputing\r\n`

#include "include/encode_decode.h"//include more comment

std::string De_encode::getStringMessage(std::string & raw)
{
    std::string rc = "$";
    int length = (int) raw.length();
    rc += std::to_string(length);
    rc += "\r\n";
    rc += raw;
    rc += "\r\n";
    return rc;
}


std::string De_encode::getSuccessMessage()
{
    std::string rc = "+OK\r\n";
    return rc;
}

std::string De_encode::getErrorMessage()
{
    std::string rc = "-ERROR\r\n";
    return rc;
}


std::string De_encode::getIntegerMessage(int raw)
{
    std::string rc = ":";
    rc += std::to_string(raw);
    rc += "\r\n";
    return rc;
}

std::string De_encode::getRESPArry(std::vector<std::string> & raw)
{
    std::string rc = "*";
    int number = raw.size();

    rc += std::to_string(number);
    rc += "\r\n";

    for(size_t i = 0; i < raw.size(); i++)
    {
        rc += getStringMessage(raw[i]);
    }

    return rc;
}

std::string De_encode::getRESPArry(std::string raw)
{
    std::string rc = "*";
    int number = 1;

    rc += std::to_string(number);
    rc += "\r\n";
    rc += getStringMessage(raw);

    return rc;
}


std::string De_encode::de_encodeStringMessage(char * message, int * length)
{
    std::string rc = "";
    int strl = 0;
    int pos = 0;
    assert(message[0] == '$');

    strl = atoi(message + 1);
    pos = 3 + std::to_string(strl).size();
    rc = std::string(message + pos, strl);

    if(length) {
        *length = pos + strl + 2;
    }

    return rc;
}


int De_encode::de_encodeIntegerMessage(char * message, int * length)
{
    int rc = 0;
    assert(message[0] == ':');
    rc = atoi(message + 1);

    if(length) {
        *length = 3 + std::to_string(rc).length();
    }
    return rc;
}


bool De_encode::de_encodeStatusMessage(char * message, int * length)
{
    assert(message[0] == '+' || message[0] == '-');
    if(message[0] == '+') {
        if(length) {
            *length = 5;
        }
        return true;
    }
    else {
        if(length) {
            *length = 8;
        }
        return false;
    }
}

void De_encode::de_encodeRESPArry(char * message, std::vector<std::string> & rc, int * length)
{
    int number = 0;
    int pos = 0;

    assert(message[0] == '*');
    number = atoi(message + 1);
    pos += std::to_string(number).length() + 3;

    for(int i = 0; i < number; i++) {
        int len = 0;
        std::string tmp = de_encodeStringMessage(message + pos, &len);
        rc.push_back(tmp);
        pos += len;
    }

    if(length) {
        *length = pos;
    }
}
