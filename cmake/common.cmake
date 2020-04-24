cmake_minimum_required(VERSION 2.8.8)
set(PROJECT_NAME_STR skip_list)

project(${PROJECT_NAME_STR} C CXX)

include_directories("${DEPS_ROOT}/include")
include_directories("${PROJECT_SOURCE_DIR}")
link_directories("${DEPS_ROOT}/lib")
link_directories("${DEPS_ROOT}/lib64")

set (CMAKE_CXX_STANDARD 11)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -Wno-deprecated -std=c++0x")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -ansi -Wno-deprecated -std=c++0x")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_VARIADIC_MAX=10")
endif() 

if(WIN32)
    set(_OPT_CMAKE_ARGS "-Dgtest_force_shared_crt=ON;-DCMAKE_SH=CMAKE_SH-NOTFOUND")
else()
    set(_OPT_CMAKE_ARGS "")
endif()


