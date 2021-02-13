cmake_minimum_required(VERSION 3.17)
project(TA_DFA)

set(CMAKE_CXX_STANDARD 14)
set(GRAPHVIZ_INCLUDE_DIR "/usr/include/graphviz/")
set(GRAPHVIZ_LIB_DIR "/usr/lib/x86_64-linux-gnu/graphviz/")


include_directories("${GRAPHVIZ_INCLUDE_DIR}")

add_executable(TA_DFA main.cpp DFA.h DFA.cpp main.cpp)

target_link_libraries(TA_DFA "${GRAPHVIZ_LIB_DIR}" cgraph gvc)

