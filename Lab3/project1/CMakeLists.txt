cmake_minimum_required(VERSION 3.10.2)
project(project1)

aux_source_directory(. DIR_SRCS)

set(CMAKE_CXX_FLAGS "-D_FILE_OFFSET_BITS=64 -ggdb -Wall -O0 ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS " -fPIC -rdynamic -pthread ${CMAKE_EXE_LINKER_FLAGS}")


INCLUDE_DIRECTORIES( ./include ./third_party/parallel_hashmap ./third_party/ossSocket)

add_executable(kvstore2pcsystem main.cpp encode_decode.cpp network.cpp conf.cpp kv.cpp myLog.cpp
        include/encode_decode.h
        include/headSet.h
        include/kv.h
        include/myLog.h
        include/util.h
        third_party/ossSocket/pd.h
        include/network.h
        include/conf.h
        third_party/ossSocket/ossSocket.cc
        participant.cpp participant.h coordinator.cpp coordinator.h include/lock.h)

