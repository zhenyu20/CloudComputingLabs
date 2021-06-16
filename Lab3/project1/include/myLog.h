//
// Created by wenkai on 2021/6/16.
//

#ifndef PROJECT1_MYLOG_H
#define PROJECT1_MYLOG_H
#include "headSet.h"

#define TXID_START          5
#define SUPER_TXID          0
#define RECOVERY_TXID       1
#define ASK_DATA_TXID       2

typedef enum logState : uint8_t
{
    INVALID,
    LOG_PRE,
    LOG_COMMIT,
    LOG_ABORT
} logState;

/*
    The logger is designed for consistency insurance. The log will be
    stored in the memory, which means the log will not be persisted.
    Therefore, no insurance for the data persistance.

    The logger will be used when the system in the recovery mode. The
    coordinator and participants will communicate through the coordinator,
    and they will choose the latest transaction ID. At last, the system
    will be recovered until the newest transaction.
*/

/*
        The log's format is like this:
        [ ID    ]
        [ STATE ]
        [ EVENT ]
*/

typedef struct Log
{
    txid ID;
    logState state;
    std::string event;

    Log() {}

    Log(std::string str)
    {
        strToLog(str);
    }

    Log(txid id, logState state, std::string event) :
            ID(id), state(state), event(event) { }

    void changeState(logState newState)
    {
        state = newState;
    }

    void strToLog(std::string str);
    std::string logToStr();

} Log;


class Mylog
{
private:
    std::map<txid, Log> _l;
public:
    void writeLog(txid ID, logState state, std::string event);
    void writeLog(std::string lg);
    void writeLog(Log lg);
    void changeLogState(txid ID, logState newState);
    Log getLogByTXID(txid ID);
};
#endif //PROJECT1_MYLOG_H
