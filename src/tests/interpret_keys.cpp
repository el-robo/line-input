#include <catch2/catch_all.hpp>
#include "interpret_keys.hpp"

template< typename T >
struct expectation
{
    T sequence;
    input::special result;
};

using namespace input;

SCENARIO( "interpreting key sequences" )
{
    GIVEN( "input from the user" )
    {
        std::vector< expectation< std::string > > cases
        {
            { "[1;5D", key::arrow_left, modifier_mask( modifier::control ) },
            { "[1;5C", key::arrow_right, modifier_mask( modifier::control ) },
            { "d", key::delete_key, modifier_mask( modifier::control ) }
        };

        for( auto expect : cases )
        {
            const auto parsed = input::interpret_escape_sequence( { expect.sequence.begin(), expect.sequence.end() } );

            REQUIRE( parsed.input == expect.result.input );
            REQUIRE( parsed.modifiers == expect.result.modifiers );
        }
    }
}

SCENARIO( "interpreting key codes" )
{
    GIVEN( "input from the user" )
    {
        std::vector< expectation< char > > cases
        {
            { code::key_ctrl_backspace, { key::backspace, modifier_mask( modifier::control ) } },
            { code::key_ctrl_backspace2, { key::backspace, modifier_mask( modifier::control ) } }
        };

        for( auto expect : cases )
        {
            const auto parsed = input::interpret_code( expect.sequence );

            REQUIRE( parsed.input == expect.result.input );
            REQUIRE( parsed.modifiers == expect.result.modifiers );
        }
    }
}
