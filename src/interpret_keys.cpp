#include "interpret_keys.hpp"

namespace input::key
{
    special interpret( std::span< char > sequence )
    {
        return { sequence[ 1 ] };
    }
}
