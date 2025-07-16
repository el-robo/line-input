#include "console.hpp"
#include <iostream>

namespace console
{
    void write_line( const std::string_view &line )
    {
        std::cout << line << "\r\n";
    }
}
