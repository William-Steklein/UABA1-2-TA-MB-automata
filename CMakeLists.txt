cmake_minimum_required(VERSION 3.17)
project(TA_Automata)

set(CMAKE_CXX_STANDARD 14)
set(GRAPHVIZ_INCLUDE_DIR "/usr/include/graphviz/")
set(GRAPHVIZ_LIB_DIR "/usr/lib/x86_64-linux-gnu/graphviz/")


include_directories("${GRAPHVIZ_INCLUDE_DIR}")

add_executable(TA_DFA src/main.cpp src/DFA.h src/DFA.cpp src/main.cpp src/NFA.cpp src/NFA.h src/Automaton.cpp src/Automaton.h src/ENFA.cpp src/ENFA.h src/RE.cpp src/RE.h)

target_link_libraries(TA_DFA "${GRAPHVIZ_LIB_DIR}" cgraph gvc)

