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
#include <string_view>
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

    std::generator< std::variant< char, special > > keyboard::filtered_keys()
    {
        std::vector< char > buffer;
        buffer.reserve( 10 );

        for( char input : keys() )
        {
            if( !std::iscntrl( input ) )
            {
                co_yield input;
                continue;
            }

            switch( input )
            {
                case code::escape_code:
                {
                    buffer.clear();
                    read_available( std::back_inserter( buffer ) );
                    interject( "seq: {}", std::string_view( buffer.begin(), buffer.end() ) );
                    co_yield interpret_escape_sequence( buffer );
                    break;
                }

                default:
                {
                    interject( "code: {}", static_cast< int >( input ) );
                    co_yield special { interpret_code( input ) };
                    break;
                }
            }
        }
    }

    bool is_boundary( char in )
    {
        return in == ' ';
    }

    template< int direction >
    inline constexpr auto sentinel( const auto &range )
    {
        if constexpr( direction > 0 )
        {
            return range.end();
        }
        else
        {
            return range.begin();
        }
    }

    template< int direction >
    auto find_boundary( line_buffer &line )
    {
        using namespace std::ranges;

        auto it = line.iterator();
        const auto end = sentinel< direction >( line.buffer );
        advance( it, direction, end );

        while( it != end )
        {
            advance( it, direction, end );

            if( is_boundary( *it ) )
            {
                if constexpr( direction < 0 )
                {
                    advance( it, -1 * direction );
                }
                break;
            }
        }

        return it;
    }

    std::generator< std::string_view > keyboard::lines()
    {
        for( auto variant : filtered_keys() )
        {
            bool cycle_buffer = false;

            const auto handle_value = [ & ]( auto&& value )
            {
                using T = std::decay_t< decltype( value ) >;

                if constexpr( std::is_same_v< T, char > )
                {
                    line.insert( value );
                    return;
                }
                else
                {
                    // is a special key
                    switch( value.input )
                    {
                        case key::eof:
                        case key::interrupt:
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
                        case key::home:
                        {
                            line.move( 0 );
                            break;
                        }
                        case key::end:
                        {
                            line.move( line.size() + 1 );
                            break;
                        }
                        case key::arrow_left:
                        {
                            if( value.held( modifier::control ) )
                            {
                                line.move( find_boundary< -1 >( line ) );
                                break;
                            }

                            line.move( std::max< int >( 0, line.cursor - 1 ) );
                            break;
                        }
                        case key::arrow_right:
                        {
                            if( value.held( modifier::control ) )
                            {
                                line.move( find_boundary< 1 >( line ) );
                                break;
                            }

                            line.move( line.cursor + 1 );
                            break;
                        }
                        case key::backspace:
                        {
                            if( value.held( modifier::control ) )
                            {
                                auto start = line.iterator();
                                line.erase( find_boundary< -1 >( line ), start );
                                break;
                            }

                            line.erase( line.cursor - 1 );
                            break;
                        }
                        case key::delete_key:
                        {
                            if( value.held( modifier::control ) )
                            {
                                auto start = line.iterator();
                                line.erase( start, find_boundary< 1 >( line ) );
                                break;
                            }

                            line.erase( line.cursor );
                            break;
                        }
                        case key::return_key:
                        {
                            cycle_buffer = true;
                            break;
                        }
                        case key::tab:
                        {
                            break;
                        }
                        default:
                        {
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
