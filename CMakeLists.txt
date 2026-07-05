cmake_minimum_required(VERSION 3.13)

project(shell-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

find_path(READLINE_INCLUDE_DIR readline/readline.h
    PATHS ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/include
    NO_DEFAULT_PATH
)
find_library(READLINE_LIBRARY NAMES readline
    PATHS ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/lib
    NO_DEFAULT_PATH
)
find_library(NCURSES_LIBRARY NAMES ncurses ncursesw
    PATHS ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-linux/lib
    NO_DEFAULT_PATH
)

add_executable(shell ${SOURCE_FILES})

target_include_directories(shell PRIVATE ${READLINE_INCLUDE_DIR})
target_link_libraries(shell PRIVATE ${READLINE_LIBRARY} ${NCURSES_LIBRARY})
