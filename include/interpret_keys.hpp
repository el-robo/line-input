#pragma once

#include <bitset>
#include <span>

namespace input
{
    enum class key
    {
        invalid,
        eof,
        interrupt,
        home,
        end,
        backspace,
        insert,
        delete_key,
        return_key,
        escape,
        page_up,
        page_down,
        arrow_up,
        arrow_down,
        arrow_left,
        arrow_right,
        tab,
        f0,
        f1,
        f2,
        f3,
        f4,
        f5,
        f6,
        f7,
        f8,
        f9,
        f10,
        f11,
        f12
    };

    namespace code
    {
        static constexpr char key_interrupt = 3;
        static constexpr char key_end_of_input = 4;
        static constexpr char key_tab = 9;
        static constexpr char key_return = 13;
        static constexpr char escape_code = 27;
        static constexpr char key_ctrl_backspace = 8;
        static constexpr char key_ctrl_backspace2 = 23;
        static constexpr char key_backspace = 127;

        static constexpr char key_up = 'A';
        static constexpr char key_down = 'B';
        static constexpr char key_right = 'C';
        static constexpr char key_left = 'D';
        static constexpr char key_home = 'H';
        static constexpr char key_end = 'F';
        static constexpr char key_insert = '3';
        static constexpr char key_delete = 'd';
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
        key input = key::invalid;
        std::bitset< 4 > modifiers = 0;

        bool held( modifier mod ) const
        {
            return modifiers.test( static_cast< int >( mod ) );
        }
    };

    std::bitset< 4 > modifier_mask( modifier modifier, auto ... modifiers, std::bitset< 4 > mask = {} )
    {
        mask.set( static_cast< int >( modifier ) );

        if constexpr( sizeof...( modifiers ) )
        {
            return modifier_mask( modifiers..., mask );
        }
        else
        {
            return mask;
        }
    }

    special interpret_escape_sequence( std::span< char > sequence );
    special interpret_code( char code, int modifiers = 0 );
}
