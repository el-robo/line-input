
#include <generator>
#include <string_view>

namespace input {

struct keyboard
{
    bool active = true;
    std::generator< char > keys();
    std::generator< std::string_view > lines();
};

}
