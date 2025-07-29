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

include( FetchContent )

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable( Catch2 )

list( APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras )
include( Catch )


add_executable( tests
    src/interpret_keys.cpp
    src/tests/interpret_keys.cpp
)

target_include_directories( tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries( tests PRIVATE Catch2::Catch2WithMain )

enable_testing()
add_test( NAME tests COMMAND tests )

add_custom_command(
    TARGET tests
    POST_BUILD
    COMMAND ctest --output-on-failure
)
