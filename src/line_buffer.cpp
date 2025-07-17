#include "line_buffer.hpp"
#include "console.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

namespace input
{
    static constexpr size_t buffer_increments = 256;

    void debug_buffer(
        std::vector<char> &line,
        size_t cursor,
        std::string additional = {} )
    {
        static std::ofstream debug = std::ofstream( "buffer-contents" );

        const auto midpoint = line.begin() + cursor;
        const auto pre = std::string( line.begin(), midpoint );
        const auto post = std::string( midpoint, line.end() );

        if( !pre.empty() ) debug << pre;
        debug << "|";
        if( !post.empty() ) debug << post;
        debug << " " << additional;
        debug << std::endl;
    }

    std::function< void( std::string ) > print_debug;

    line_buffer::line_buffer() :
        line(),
        cursor( 0 )
    {
        print_debug = [ & ]( std::string add )
        {
            debug_buffer( line, cursor, add );
        };

        line.reserve( buffer_increments );
        cursor = 0;
    }

    std::string line_buffer::cycle()
    {
        std::string result( line.begin(), line.end() );
        line.clear();
        cursor = 0;
        print_debug( "cycle" );
        return result;
    }

    size_t line_buffer::size() const
    {
        return line.size();
    }

    void line_buffer::insert( char input )
    {
        if( line.size() + 1 >= line.capacity() )
        {
            const auto offset = cursor;
            line.reserve( line.capacity() + buffer_increments );
            move( offset );
        }

        line.insert( line.begin() + cursor++, input );

        if( cursor < line.size() )
        {
            restore();
        }
        else
        {
            std::cout << input << std::flush;
        }

        print_debug( "insert" );
    }

    void line_buffer::erase( size_t index )
    {
        const auto offset = cursor;

        if( index >= line.size() )
        {
            return;
        }

        line.erase( line.begin() + index );
        move( index < offset ? offset - 1 : offset );
        print_debug( std::format( "erase {} from {}", index, offset ) );
        restore();
    }

    void line_buffer::move( size_t offset )
    {
        const auto given = offset;
        const auto start = cursor;
        cursor = std::min( line.size(), offset );
        std::cout << console::cursor::to_column( cursor + 1 ) << std::flush;

        print_debug( std::format( "move({}) {} -> {}", given, start, cursor ) );
    }

    void line_buffer::restore()
    {
        std::cout << '\r'
            << std::string_view( line.begin(), line.end() )
            << console::erase::cursor_to_end_of_line()
            << console::cursor::to_column( cursor + 1 )
            << std::flush;
    }
}
