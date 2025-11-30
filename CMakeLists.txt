cmake_minimum_required(VERSION 3.31)
project(csapp)

#set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB_RECURSE CSAPP_SOURCES "src/*.cpp" "src/*.c")

# 递归查找当前目录及其所有子目录下的 .h 和 .hpp 文件
file(GLOB_RECURSE CSAPP_HEADERS "include/*.h" "include/*.hpp")

add_library(csapp)
target_sources(csapp
        PRIVATE
            ${CSAPP_SOURCES}
        PUBLIC
            FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            ${CSAPP_HEADERS}
)

add_subdirectory(ecf)
