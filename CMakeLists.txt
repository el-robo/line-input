cmake_minimum_required( VERSION 3.10 )
project( line-completer )
set( CMAKE_CXX_STANDARD 23 )

add_executable( completor
    src/main.cpp
    src/console.cpp
    src/interpret_keys.cpp
    src/keyboard.cpp
    src/line_buffer.cpp
)

target_include_directories( completor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
