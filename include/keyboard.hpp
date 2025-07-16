
#include <functional>
#include <generator>
#include <string_view>

namespace input {

struct keyboard
{
    std::function< void() > on_interrupt;
    bool active = true;

    keyboard();

    char read_key();
    std::generator< char > keys();
    std::generator< std::pair<char, bool> > filtered_keys();
    std::generator< std::string_view > lines();
};

}
