#pragma once

#include <bitset>
#include <span>
namespace input::key
{
    namespace code
    {
        static constexpr char key_interrupt = 3;
        static constexpr char key_end_of_input = 4;
        static constexpr char key_tab = 9;
        static constexpr char key_return = 13;
        static constexpr char key_ctrl_backspace = 23;
        static constexpr char key_modifier_combo = 49;
        static constexpr char escape_code = 27;
        static constexpr char key_backspace = 127;

        // keys with the '[' special prefix
        static constexpr char key_up = 'A';
        static constexpr char key_down = 'B';
        static constexpr char key_right = 'C';
        static constexpr char key_left = 'D';
        static constexpr char key_home = 'H';
        static constexpr char key_end = 'F';
        static constexpr char key_insert = '3';
        static constexpr char key_delete = '3';
    }

    /* values correspond to the bitmask in the modifier value */
    enum class modifier
    {
        shift = 0,
        alt = 1,
        control = 2,
        meta = 3
    };

    struct special
    {
        char code = '\0';
        std::bitset< 4 > modifiers = 0;
    };

    special interpret( std::span< char > sequence );
}
