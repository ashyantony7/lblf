cmake_minimum_required(VERSION 3.29)

project(lblf VERSION 0.3 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#include(FetchContent)

#FetchContent_Declare(
#    zlib
#    URL https://zlib.net/zlib1211.zip
#)
#FetchContent_MakeAvailable(zlib)

find_package(ZLIB REQUIRED)

if (MSVC)
    # warning level 4
    #add_compile_options(/W4)
else()
    # additional warnings
    #add_compile_options(-Wall -Wno-switch -Wno-deprecated-copy -Wdeprecated-enum-enum-conversion -Wmisleading-indentation -Wc++20-extensions  -Wshadow -Wconversion)
endif()

message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")


add_executable(read read.cc print.cc blf_reader.cc)
target_link_libraries(read PRIVATE ZLIB::ZLIB )

add_executable(object_types object_types.cc print.cc blf_reader.cc)
target_link_libraries(object_types PRIVATE ZLIB::ZLIB )

add_executable(read_blf read_blf.cc print.cc blf_reader.cc)
target_link_libraries(read_blf PRIVATE ZLIB::ZLIB )

add_executable(read_can read_can.cc print.cc blf_reader.cc)
target_link_libraries(read_can PRIVATE ZLIB::ZLIB )

add_executable(read_filestatistics read_filestatistics.cc print.cc blf_reader.cc)
target_link_libraries(read_filestatistics PRIVATE ZLIB::ZLIB )

enable_testing()
add_executable(test_can_fd test_can_fd.cc)
add_test(NAME can_fd COMMAND test_can_fd)
