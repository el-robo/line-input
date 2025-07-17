#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace  input
{
    struct line_buffer
    {
        std::vector< char > line;
        std::vector< char >::iterator pos;

        line_buffer();

        std::string cycle();
        size_t size() const;
        size_t position();
        void insert( char input );
        void move( size_t offset );
        void restore();
    };

}
