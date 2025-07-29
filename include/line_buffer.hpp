#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace  input
{
    struct line_buffer
    {
        using vector = std::vector< char >;

        vector buffer;
        size_t cursor;

        line_buffer();

        std::string cycle();
        size_t size() const;

        void insert( char input );

        void erase( vector::iterator begin, vector::iterator end );
        void erase( size_t position );

        void move( size_t offset );
        void move( vector::iterator );
        void move( vector::const_iterator );

        void restore();

        vector::iterator iterator();
    };
}
