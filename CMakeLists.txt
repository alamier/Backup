cmake_minimum_required(VERSION 3.8)
project(project3)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        Event.cc
        Event.h
        global.h
        Link.cc
        Link.h
        Node.cc
        Node.h
        RoutingProtocol.h
        RoutingProtocolImpl.cc
        RoutingProtocolImpl.h
        Simulator.cc
        Simulator.h PortTable.cpp PortTable.h ForwardTable.cpp ForwardTable.h)

add_executable(project3 ${SOURCE_FILES})