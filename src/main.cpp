#include <cstdio>
#include <cstdlib>
#include <generator>
#include <format>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <termios.h>
#include "keyboard.hpp"

void signal_handler( int signal )
{
    std::puts("interrupted");
    exit(0);
}

int main( int argc, char **argv )
{
    std::signal(SIGINT, signal_handler);

    input::keyboard keyboard;
    std::cout << "hi\r\n";

    for( auto word : keyboard.lines() )
    {
        std::cout << "\rgot a word: " << word << "\r\n";
    }

    std::cout << "bye\r\n";
    return 0;
}
