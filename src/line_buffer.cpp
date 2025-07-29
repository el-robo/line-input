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
        buffer(),
        cursor( 0 )
    {
        print_debug = [ & ]( std::string add )
        {
            debug_buffer( buffer, cursor, add );
        };

        buffer.reserve( buffer_increments );
        cursor = 0;
    }

    std::string line_buffer::cycle()
    {
        std::string result( buffer.begin(), buffer.end() );
        buffer.clear();
        cursor = 0;
        print_debug( "cycle" );
        return result;
    }

    size_t line_buffer::size() const
    {
        return buffer.size();
    }

    void line_buffer::insert( char input )
    {
        if( buffer.size() + 1 >= buffer.capacity() )
        {
            const auto offset = cursor;
            buffer.reserve( buffer.capacity() + buffer_increments );
            move( offset );
        }

        buffer.insert( buffer.begin() + cursor++, input );

        if( cursor < buffer.size() )
        {
            restore();
        }
        else
        {
            std::cout << input << std::flush;
        }

        print_debug( "insert" );
    }

    void line_buffer::erase( vector::iterator begin, vector::iterator end )
    {
        auto offset = cursor;

        if( offset >= std::distance( buffer.begin(), end ) )
        {
            offset -= std::distance( begin, end );
        }
        else if( offset > std::distance( buffer.begin(), begin ) )
        {
            offset = std::distance( buffer.begin(), begin );
        }

        print_debug( std::format( "erase {} from {}", std::string_view( begin, end ), offset ) );
        buffer.erase( begin, end );
        move( offset );
        restore();
    }

    void line_buffer::erase( size_t index )
    {
        const auto offset = cursor;

        if( index >= buffer.size() )
        {
            return;
        }

        buffer.erase( buffer.begin() + index );
        move( index < offset ? offset - 1 : offset );
        print_debug( std::format( "erase {} from {}", index, offset ) );
        restore();
    }


    void line_buffer::move( size_t offset )
    {
        const auto given = offset;
        const auto start = cursor;
        cursor = std::min( buffer.size(), offset );
        std::cout << console::cursor::to_column( cursor + 1 ) << std::flush;

        print_debug( std::format( "move({}) {} -> {}", given, start, cursor ) );
    }

    void line_buffer::move( std::vector< char >::const_iterator offset )
    {
        move( std::distance( buffer.cbegin(), offset ) );
    }

    void line_buffer::move( std::vector< char >::iterator offset )
    {
        move( std::distance( buffer.begin(), offset ) );
    }

    void line_buffer::restore()
    {
        std::cout << '\r'
            << std::string_view( buffer.begin(), buffer.end() )
            << console::erase::cursor_to_end_of_line()
            << console::cursor::to_column( cursor + 1 )
            << std::flush;
    }

    std::vector< char >::iterator line_buffer::iterator()
    {
        return buffer.begin() + cursor;
    }
}
