#include "line_buffer.hpp"
#include "console.hpp"
#include <iostream>

namespace input
{
    static constexpr size_t buffer_increments = 256;

    line_buffer::line_buffer() :
        line(),
        pos()
    {
        line.reserve( buffer_increments );
        pos = line.begin();
    }

    std::string line_buffer::cycle()
    {
        std::string result( line.begin(), line.end() );
        line.clear();
        pos = line.begin();
        return result;
    }

    size_t line_buffer::size() const
    {
        return line.size();
    }

    size_t line_buffer::position()
    {
        return static_cast< size_t >( std::distance( line.begin(), pos ) );
    }

    void line_buffer::insert( char input )
    {
        if( line.size() + 1 >= line.capacity() )
        {
            const auto offset = position();
            line.reserve( line.capacity() + buffer_increments );
            move( offset );
        }

        line.insert( pos++, input );
        std::cout << input << std::flush;
    }

    void line_buffer::move( size_t offset )
    {
        offset = std::min( line.size() + 1, offset );
        pos = line.begin() + offset;
        std::cout << console::cursor::to_column( offset ) << std::flush;
    }

    void line_buffer::restore()
    {
        std::cout << '\r'
            << std::string_view( line.begin(), line.end() )
            << console::cursor::to_column( position() ) << std::flush;
    }
}
