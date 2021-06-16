//
// Created by wenkai on 2021/6/16.
//

#ifndef PROJECT1_CONF_H
#define PROJECT1_CONF_H
#include "headSet.h"

std::string GetOptLong(int argc, char *argv[]);
bool readConf(std::string conf,
              std::vector<NodeInfo> & p,
              NodeInfo & c,
              Mode & m);

void showInfo(std::vector<NodeInfo>  p,
              NodeInfo  c,
              Mode  m);
#endif //PROJECT1_CONF_H
