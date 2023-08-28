cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
#set(SOURCES BleDeviceFinder.cpp display.cpp Config.cpp main.cpp)
idf_component_register(SRC_DIRS "." INCLUDE_DIRS ".") 