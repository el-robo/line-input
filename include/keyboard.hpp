
#include "interpret_keys.hpp"
#include "line_buffer.hpp"
#include "console.hpp"
#include <functional>
#include <generator>
#include <iostream>
#include <string_view>

namespace input
{
    enum class is_special
    {
        yes,
        no
    };

    constexpr char null_char = '\0';

    struct keyboard
    {
        std::function< void() > on_interrupt;
        line_buffer line;
        bool active = true;

        keyboard();

        enum class wait { yes, no };

        char read_key( wait = wait::yes );
        std::generator< char > keys();
        std::generator< std::variant< char, key::special > > filtered_keys();
        std::generator< std::string_view > lines();

        template< typename T >
        void read_available( T destination )
        {
            auto key = read_key( wait::no );

            while( key != null_char )
            {
                *destination++ = key;
                key = read_key( wait::no );
            }
        }

        void interject( auto ... args )
        {
            std::cout << console::erase::line() << '\r';
            console::write_line( args ... );
            line.restore();
        };
    };
}
