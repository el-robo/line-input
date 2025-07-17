#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace  input
{
    struct line_buffer
    {
        std::vector< char > line;
        size_t cursor;

        line_buffer();

        std::string cycle();
        size_t size() const;

        void insert( char input );
        void erase( size_t position );
        void move( size_t offset );
        void restore();
    };
}
