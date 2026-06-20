cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

find_path(READLINE_INCLUDE_DIR readline/readline.h)
find_library(READLINE_LIBRARY NAMES readline)
find_library(NCURSES_LIBRARY NAMES ncurses)

add_executable(shell ${SOURCE_FILES})

target_include_directories(shell PRIVATE ${READLINE_INCLUDE_DIR})
target_link_libraries(shell PRIVATE ${READLINE_LIBRARY} ${NCURSES_LIBRARY})
