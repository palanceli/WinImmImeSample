cmake_minimum_required(VERSION 3.7)
project(WinImmImeSample)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 收集所有源文件
file(GLOB SRC "${PROJECT_SOURCE_DIR}/*.cpp" "${PROJECT_SOURCE_DIR}/*.h" "${PROJECT_SOURCE_DIR}/*.def" "${PROJECT_SOURCE_DIR}/*.rc")
list(REMOVE_ITEM SRC "${PROJECT_SOURCE_DIR}/installer.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /utf-8")
add_definitions(-DUNICODE -D_UNICODE -DXXIME_EXPORTS)


# 制定资源编译器
set(CMAKE_RC_COMPILER_INIT windres)
SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        )
foreach(CompilerFlag ${CompilerFlags})
  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
endforeach()

add_library(WinImmImeSample SHARED ${SRC})
target_link_libraries(WinImmImeSample imm32) # 链接imm32.lib
set_target_properties(WinImmImeSample PROPERTIES OUTPUT_NAME "WinImmImeSample" SUFFIX ".ime" ) # 输出dll文件改名为WinImmImeSample.ime
SET_TARGET_PROPERTIES(WinImmImeSample PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='false'")
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#静态链接boost，并指定需要的头文件和lib库

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(Boost_LIBRARY_DIR "$ENV{BOOST_ROOT}\\stage\\x64\\lib")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(Boost_LIBRARY_DIR "$ENV{BOOST_ROOT}\\stage\\win32\\lib")
endif()

message(STATUS "Boost_LIBRARY_DIR=${Boost_LIBRARY_DIR}")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost COMPONENTS program_options log REQUIRED)

if(NOT Boost_FOUND)
  message("Boost not be found!")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_executable(ImeInstaller "${PROJECT_SOURCE_DIR}/installer.cpp")
target_link_libraries(ImeInstaller ${Boost_LIBRARIES} imm32)
SET_TARGET_PROPERTIES(ImeInstaller PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='false'")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)