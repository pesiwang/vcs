CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(echo)

SET(CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -DDEBUG")

INCLUDE_DIRECTORIES(include include/vcs)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

AUX_SOURCE_DIRECTORY(src SRC_LIST)

ADD_EXECUTABLE(echo_server.exe ${SRC_LIST})
TARGET_LINK_LIBRARIES(echo_server.exe vcs pthread jsoncpp)



