
#include <generator>

struct input
{
    bool active = true;
    std::generator< char > keys();
};
