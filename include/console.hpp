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
}
