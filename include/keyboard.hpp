
#include <functional>
#include <generator>
#include <string_view>

namespace input
{
    enum class is_special
    {
        yes,
        no
    };

    struct keyboard
    {
        std::function< void() > on_interrupt;
        bool active = true;

        keyboard();

        char read_key();
        std::generator< char > keys();
        std::generator< std::pair< char, is_special > > filtered_keys();
        std::generator< std::string_view > lines();
    };
}
