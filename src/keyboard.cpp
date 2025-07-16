#include "keyboard.hpp"
#include "console.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
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

    keyboard::keyboard()
    {
        tcgetattr( STDIN_FILENO, &orig_termios );
        std::atexit(disable_raw_mode);

        enable_raw_mode();
    }

    char keyboard::read_key()
    {
        char input = '\0';

        while( active )
        {
            if( read( STDIN_FILENO, &input, 1 ) == 1 )
            {
                break;
            }
        }

        return input;
    }

    std::generator< char > keyboard::keys()
    {
        while( active )
        {
            co_yield read_key();
        }
    }

    namespace keycode
    {
        static constexpr char key_interrupt = 3;
        static constexpr char key_end_of_input = 4;
        static constexpr char key_tab = 9;
        static constexpr char key_return = 13;
        static constexpr char read_more = 27;
        static constexpr char key_backspace = 127;

        // keys with the '[' special prefix
        static constexpr char key_up = 'A';
        static constexpr char key_down = 'B';
        static constexpr char key_right = 'C';
        static constexpr char key_left = 'D';
        static constexpr char key_home = 'H';
        static constexpr char key_end = 'F';
        static constexpr char key_insert = '3';
        static constexpr char key_delete = '3';
    }

    std::generator< std::pair<char, bool> > keyboard::filtered_keys()
    {
        for( char input : keys() )
        {
            if( !std::iscntrl( input ) )
            {
                co_yield { input, false };
                continue;
            }
            else if( input == keycode::read_more )
            {
                const std::array< char, 2 > special { read_key(), read_key() };
                co_yield { special[1], true };
            }
            else
            {
                co_yield { input, true };
            }
        }
    }

    struct buffer
    {
        std::string line;
        std::string::iterator pos = line.begin();

        std::string cycle()
        {
            std::string result = line;
            line.clear();
            pos = line.begin();
            return result;
        }

        size_t size() const
        {
            return line.size();
        }

        size_t position()
        {
            return static_cast< size_t >( std::distance< std::string::iterator >( line.begin(), pos ) );
        }

        void insert( char input )
        {
            line.insert( pos++, input );
            std::cout << input << std::flush;
        }

        void move( size_t offset )
        {
            pos = line.begin() + offset;
            std::cout << console::cursor::to_column( offset ) << std::flush;
        }

        void restore()
        {
            // restore state
        }
    };

    std::generator< std::string_view > keyboard::lines()
    {
        buffer line;

        for( auto [input, is_special] : filtered_keys() )
        {
            if( !is_special )
            {
                // std::cout << input << std::flush;
                line.insert( input );
                continue;
            }

            switch( input )
            {
                case keycode::key_end_of_input:
                case keycode::key_interrupt:
                {
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
                case keycode::key_home:
                {
                    line.move( 0 );
                    break;
                }
                case keycode::key_end:
                {
                    line.move( line.size() + 1 );
                    break;
                }
                case keycode::key_left:
                {
                    line.move( std::max< int >( 0, line.position() - 1 ) );
                    break;
                }
                case keycode::key_right:
                {
                    line.move( std::min< int >( line.size() + 1, line.position() + 1 ) );
                    break;
                }
                case keycode::key_backspace:
                {
                    break;
                }
                case keycode::key_return:
                {
                    co_yield line.cycle();
                    continue;
                }
                case keycode::key_tab:
                {
                    continue;
                }
                default:
                {
                    console::write_line( "\r{} is a control character", static_cast< int >( input ) );
                    continue;
                }
            }
        }
    }
}
