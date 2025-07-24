#include "keyboard.hpp"
#include "console.hpp"
#include "interpret_keys.hpp"
#include "line_buffer.hpp"

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

    char keyboard::read_key( wait on_no_input  )
    {
        char input = null_char;

        while( active )
        {
            if( read( STDIN_FILENO, &input, 1 ) == 1 || on_no_input == wait::no )
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

    // namespace key::code
    // {
    //     static constexpr char key_interrupt = 3;
    //     static constexpr char key_end_of_input = 4;
    //     static constexpr char key_tab = 9;
    //     static constexpr char key_return = 13;
    //     static constexpr char key_ctrl_backspace = 23;
    //     static constexpr char key_modifier_combo = 49;
    //     static constexpr char escape_code = 27;
    //     static constexpr char key_backspace = 127;

    //     // keys with the '[' special prefix
    //     static constexpr char key_up = 'A';
    //     static constexpr char key_down = 'B';
    //     static constexpr char key_right = 'C';
    //     static constexpr char key_left = 'D';
    //     static constexpr char key_home = 'H';
    //     static constexpr char key_end = 'F';
    //     static constexpr char key_insert = '3';
    //     static constexpr char key_delete = '3';
    // }

    std::generator< std::variant< char, key::special > > keyboard::filtered_keys()
    {
        std::vector< char > buffer;
        buffer.reserve( 10 );

        for( char input : keys() )
        {
            int val( input );
            if( !std::iscntrl( input ) )
            {
                co_yield input;
                continue;
            }

            switch( input )
            {
                case key::code::escape_code:
                {
                    buffer.clear();
                    read_available( std::back_inserter( buffer ) );

                    interject( "escaped: {}", std::string( buffer.begin(), buffer.end() ) );
                    co_yield key::interpret( buffer );
                    break;
                }

                default:
                {
                    co_yield key::special { input };
                }
            }
        }
    }

    std::generator< std::string_view > keyboard::lines()
    {
        // for( auto [input, type] : filtered_keys() )
        for( auto variant : filtered_keys() )
        {
            bool cycle_buffer = false;

            const auto handle_value = [ & ]( auto&& input )
            {
                using T = std::decay_t< decltype( input ) >;

                if constexpr( std::is_same_v< T, char > )
                {
                    line.insert( input );
                    return;
                }
                else
                {
                    switch( input.code )
                    {
                        case key::code::key_end_of_input:
                        case key::code::key_interrupt:
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
                        case key::code::key_home:
                        {
                            line.move( 0 );
                            break;
                        }
                        case key::code::key_end:
                        {
                            line.move( line.size() + 1 );
                            break;
                        }
                        case key::code::key_left:
                        {
                            line.move( std::max< int >( 0, line.cursor - 1 ) );
                            break;
                        }
                        case key::code::key_right:
                        {
                            line.move( line.cursor + 1 );
                            break;
                        }
                        case key::code::key_backspace:
                        {
                            line.erase( line.cursor - 1 );
                            break;
                        }
                        case key::code::key_delete:
                        {
                            line.erase( line.cursor );
                            read_key();
                            break;
                        }
                        case key::code::key_return:
                        {
                            cycle_buffer = true;
                            break;
                        }
                        case key::code::key_tab:
                        {
                            break;
                        }
                        default:
                        {
                            interject( "{} is a control character", static_cast< int >( input.code ) );
                            break;
                        }
                    }
                }
            };

            std::visit( handle_value, variant );

            if( cycle_buffer )
            {
                co_yield line.cycle();
            }
        }
    }
}
