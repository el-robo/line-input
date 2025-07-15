cmake_minimum_required( VERSION 3.10 )
project( line-completer )
set( CMAKE_CXX_STANDARD 23 )

add_executable( completor
    src/main.cpp
    src/input.cpp
)

target_include_directories( completor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
