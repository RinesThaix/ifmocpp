cmake_minimum_required(VERSION 3.4) 

set(HEADERS lazy_string.h) 
set(SOURCES lazy_string.cpp) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_library(lazy_string ${SOURCES} ${HEADERS}) 
install (TARGETS lazy_string DESTINATION ${CMAKE_INSTALL_PREFIX}/lib) 
install (FILES lazy_string.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include) 
install (FILES lazy_string.cpp DESTINATION ${CMAKE_INSTALL_PREFIX}/include)