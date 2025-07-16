#include "keyboard.hpp"
#include "console.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>

namespace input
{
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

        console::write_line( "key inputs done" );
    }

    namespace keycode
    {
        static constexpr char key_interrupt = 3;
        static constexpr char key_tab = 9;
        static constexpr char key_return = 13;
        static constexpr char read_more = 27;
        static constexpr char key_backspace = 127;
    }

    std::generator< std::string_view > keyboard::lines()
    {
        std::stringstream stream;
        bool read_more = false;

        while( active )
        {
            for( char key : keys() )
            {
                if( read_more )
                {
                    read_more = false;
                    console::write_line( "\r\nspecial thing found: {}", key );
                    continue;
                }

                if( !std::iscntrl( key ) )
                {
                    std::cout << key << std::flush;
                    stream << key;
                    continue;
                }

                switch( key )
                {
                    case keycode::key_interrupt:
                    {
                        console::write_line( "\r\ninterrupt" );

                        if( on_interrupt )
                        {
                            on_interrupt();
                        }
                        else
                        {
                            active = false;
                        }
                        break;
                    }
                    case keycode::key_return:
                    {
                        co_yield stream.str();
                        stream.str("");
                        continue;
                    }
                    case keycode::key_tab:
                    {
                        std::cout << "tab!\r\n";
                        co_yield stream.str();
                        stream.str("");
                        continue;
                    }
                    case keycode::read_more:
                    {
                        read_more = true;
                        break;
                    }
                    default:
                    {
                        console::write_line( "{} is a control character", static_cast< int >( key ) );
                        continue;
                    }
                }
            }
        }
    }
}
