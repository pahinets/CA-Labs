cmake_minimum_required(VERSION 3.10)
project(calculator CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(calculator STATIC calculator.cpp calculator.h)
target_include_directories(calculator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(calc main.cpp)
target_link_libraries(calc PRIVATE calculator)

if(MSVC)
    target_compile_options(calculator PRIVATE /W4)
else()
    target_compile_options(calculator PRIVATE -Wall -Wextra)
endif()
