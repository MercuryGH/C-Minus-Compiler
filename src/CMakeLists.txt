cmake_minimum_required(VERSION 3.10)
project(c_minus)

# https://stackoverflow.com/questions/3897839/how-to-link-c-program-with-boost-using-cmake

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
find_package(jsoncpp REQUIRED)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include)
include_directories(.)

aux_source_directory(frontend FRONT_END_SRC)
aux_source_directory(backend BACK_END_SRC)
aux_source_directory(ast_vis AST_VIS_SRC)

add_executable(cmc cmc.cpp Compiler.cpp ${FRONT_END_SRC} ${BACK_END_SRC} ${AST_VIS_SRC})
add_executable(cm cm.cpp Runtime.cpp ${BACK_END_SRC})

target_link_libraries(cmc Boost::program_options jsoncpp_lib)
target_link_libraries(cm Boost::program_options)
