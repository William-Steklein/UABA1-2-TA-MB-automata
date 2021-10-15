cmake_minimum_required(VERSION 3.6)
project(UABA1_TA_Automata)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "-Wall -Werror -O2")
set(CMAKE_EXE_LINKER_FLAGS -pthread)
set(GRAPHVIZ_INCLUDE_DIR "/usr/include/graphviz/")
set(GRAPHVIZ_LIB_DIR "/usr/lib/x86_64-linux-gnu/graphviz/")

include_directories("${GRAPHVIZ_INCLUDE_DIR}")

## Set include dir
#include_directories(gtest/include)
#
## Set Library dir
#link_directories(gtest/lib)

set(RELEASE_SOURCE_FILES
        src/main.cpp
        src/Automaton.cpp
        src/Automaton.h
        src/DFA.cpp
        src/DFA.h
        src/NFA.cpp
        src/NFA.h
        src/ENFA.cpp
        src/ENFA.h
        src/RE.cpp
        src/RE.h
        src/Alphabet.cpp
        src/Alphabet.h
        src/AutomataUtils.cpp
        src/CFG.cpp
        src/CFG.h
        )

set(DEBUG_SOURCE_FILES
        src/testing/AutomataTests.cpp
        src/Automaton.cpp
        src/Automaton.h
        src/DFA.cpp
        src/DFA.h
        src/NFA.cpp
        src/NFA.h
        src/ENFA.cpp
        src/ENFA.h
        src/RE.cpp
        src/RE.h
        src/Alphabet.cpp
        src/Alphabet.h
        src/AutomataUtils.cpp
        src/CFG.cpp
        src/CFG.h
        )

add_executable(UABA1_TA_Automata ${RELEASE_SOURCE_FILES})
add_executable(UABA1_TA_Automata_debug ${DEBUG_SOURCE_FILES})

target_link_libraries(UABA1_TA_Automata "${GRAPHVIZ_LIB_DIR}" cgraph gvc)
target_link_libraries(UABA1_TA_Automata_debug "${GRAPHVIZ_LIB_DIR}" cgraph gvc)
#target_link_libraries(UABA1_TA_Automata_debug gtest)