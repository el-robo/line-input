#include <cstdio>
#include <cstdlib>
#include <generator>
#include <format>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <termios.h>
#include "console.hpp"
#include "keyboard.hpp"

void signal_handler( int signal )
{
    std::puts("interrupted");
    exit(0);
}

int main( int argc, char **argv )
{
    input::keyboard keyboard;

    console::write_line( "hi" );

    for( auto line : keyboard.lines() )
    {
        console::write_line( "\rgot a line: {}", line );
    }

    std::cout << "\r\nbye\r\n";

    return 0;
}
