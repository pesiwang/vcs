CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(vcs)

SET(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(include)

AUX_SOURCE_DIRECTORY(src/event_base SRC_LIST)
AUX_SOURCE_DIRECTORY(src/worker SRC_LIST)
AUX_SOURCE_DIRECTORY(src/doctor SRC_LIST)
AUX_SOURCE_DIRECTORY(src/util SRC_LIST)

ADD_LIBRARY(vcs ${SRC_LIST})

