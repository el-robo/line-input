
#include <functional>
#include <generator>
#include <string_view>

namespace input {

struct keyboard
{
    std::function< void() > on_interrupt;
    bool active = true;

    std::generator< char > keys();
    std::generator< std::string_view > lines();
};

}
