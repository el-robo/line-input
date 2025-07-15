#include <cstdio>
#include <cstdlib>
#include <generator>
#include <format>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <termios.h>
#include "input.hpp"

void signal_handler( int signal )
{
    // gSignalStatus = signal;
    std::puts("interrupted");
    exit(0);
}

int main( int argc, char **argv )
{
    std::signal(SIGINT, signal_handler);

    input input;
    std::puts( "hi" );

    for( char key : input.keys() )
    {
        std::cout << std::format( "input: {}\n", key );
    }

    std::puts( "bye" );
    return 0;
}
