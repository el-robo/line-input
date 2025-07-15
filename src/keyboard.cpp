#include "keyboard.hpp"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>

using namespace input;

struct termios orig_termios;

void disable_raw_mode()
{
    tcsetattr( STDIN_FILENO, TCSAFLUSH, &orig_termios );
}

void enable_raw_mode()
{
    struct termios raw;
    tcgetattr( STDIN_FILENO, &raw );

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr( STDIN_FILENO, TCSAFLUSH, &raw );
}

std::generator< char > keyboard::keys()
{
    tcgetattr( STDIN_FILENO, &orig_termios );
    std::atexit(disable_raw_mode);

    enable_raw_mode();

    char input;
    while( active )
    {
        if( read( STDIN_FILENO, &input, 1 ) == 1 )
        {
            co_yield input;
        }
    }

    std::cout << "key inputs done\n";
}

std::generator< std::string_view > keyboard::lines()
{
    std::stringstream stream;
    while( active )
    {
        for( char key : keys() )
        {
            if( std::iscntrl( key ) )
            {
                if( key == 13 )
                {
                    std::cout << "\r" << static_cast< int >( key ) << " is control character\r\n";
                    co_yield stream.str();
                    stream.str("");
                    continue;
                }

                std::cout << "\r" << static_cast< int >( key ) << " is control character\r\n";
                continue;
            }

            std::cout << key << std::flush;
            stream << key;

        }
    }
}
