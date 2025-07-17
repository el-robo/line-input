#pragma once

#include <format>
#include <string>
#include <string_view>

namespace console
{
    void write_line( const std::string_view &line );

    void write_line( const char *format, auto ... args )
    {
        write_line( std::vformat( format, std::make_format_args( args... ) ) );
    }

    constexpr std::string esc( std::string_view code )
    {
        return std::format( "\e{}", code );
    }

    namespace cursor
    {
        constexpr std::string move_home()
        {
            return esc( "[H" );
        }

        constexpr std::string move_to( int line, int column )
        {
            return esc( std::format( "[{};{}H", line, column ) );
        }

        constexpr std::string move_up( int lines )
        {
            return esc( std::format( "[{}A", lines ) );
        }

        constexpr std::string move_down( int lines )
        {
            return esc( std::format( "[{}B", lines ) );
        }

        constexpr std::string move_left( int columns )
        {
            return esc( std::format( "[{}D", columns ) );
        }

        constexpr std::string move_right( int columns )
        {
            return esc( std::format( "[{}C", columns ) );
        }

        constexpr std::string to_start_of_next( int lines )
        {
            return esc( std::format( "[{}E", lines ) );
        }

        constexpr std::string to_start_of_previous( int lines )
        {
            return esc( std::format( "[{}F", lines ) );
        };

        constexpr std::string to_column( int column )
        {
            return esc( std::format( "[{}G", column ) );
        }

        constexpr std::string cursor_up()
        {
            return esc( " M" );
        }

        constexpr std::string save()
        {
            return esc( " 7" );
        }

        constexpr std::string restore()
        {
            return esc( " 8" );
        }
    }

    namespace erase
    {
        constexpr std::string cursor_to_end_of_screen()
        {
            return esc( "[0J" );
        }

        constexpr std::string cursor_to_start_of_screen()
        {
            return esc( "[1J" );
        }

        constexpr std::string screen()
        {
            return esc( "[2J" );
        }

        constexpr std::string saved_lines()
        {
            return esc( "[3J" );
        }

        constexpr std::string in_line()
        {
            return esc( "[K" );
        }

        constexpr std::string cursor_to_end_of_line()
        {
            return esc( "[0K" );
        }

        constexpr std::string cursor_to_start_of_line()
        {
            return esc( "[1K" );
        }

        constexpr std::string line()
        {
            return esc( "[2K" );
        }
    }

}
