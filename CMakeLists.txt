project( reconstruct )
cmake_minimum_required(VERSION 3.19)

file( GLOB SRCS *.c *.cpp *.cc *.h *.hpp )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3 -g -std=c++17 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -std=c++17 -Wall")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE RELEASE)
endif(NOT CMAKE_BUILD_TYPE)

add_executable( ${PROJECT_NAME} ${SRCS} )
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
