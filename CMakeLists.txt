cmake_minimum_required(VERSION 3.7)
project(WinImmImeSample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 收集所有源文件
file(GLOB_RECURSE SRC "${PROJECT_SOURCE_DIR}/*.cpp" "${PROJECT_SOURCE_DIR}/*.h" "${PROJECT_SOURCE_DIR}/*.def" "${PROJECT_SOURCE_DIR}/*.rc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /utf-8")
add_definitions(-DUNICODE -D_UNICODE -DXXIME_EXPORTS)


# 制定资源编译器
set(CMAKE_RC_COMPILER_INIT windres)
SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")


add_library(WinImmImeSample SHARED ${SRC})
target_link_libraries(WinImmImeSample imm32) # 链接imm32.lib
set_target_properties(WinImmImeSample PROPERTIES OUTPUT_NAME "WinImmImeSample" SUFFIX ".ime" ) # 输出dll文件改名为WinImmImeSample.ime
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
