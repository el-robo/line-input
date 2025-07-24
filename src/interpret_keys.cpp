#include "interpret_keys.hpp"
#include <charconv>
#include <map>

namespace input
{
    template< typename T >
    auto from_chars( const char *first, const char *last )
    {
        T result = 0;
        std::from_chars( first, last, result );
        return result;
    }

    namespace vt
    {
        static const std::map< int, key > keys
        {
            { 1, key::home },
            { 2, key::insert },
            { 3, key::delete_key },
            { 4, key::end },
            { 5, key::page_up },
            { 6, key::page_down },
            { 7, key::home },
            { 8, key::end },
            { 10, key::f0 },
            { 11, key::f1 },
            { 12, key::f2 },
            { 13, key::f3 },
            { 14, key::f4 },
            { 15, key::f5 },
            { 17, key::f6 },
            { 18, key::f7 },
            { 19, key::f8 },
            { 20, key::f9 },
            { 21, key::f10 },
            { 23, key::f11 },
            { 24, key::f12 }
        };

        special interpret( std::span< char > sequence )
        {
            const int vt_code = from_chars< int >(
                sequence.data() + 1,
                sequence.data() + sequence.size() - 1
            );

            auto found = vt::keys.find( vt_code );

            if( found == vt::keys.end() )
            {
                return {};
            }

            return { found->second };
        }
    }

    static const std::map< char, key > codes
    {
        { code::key_interrupt, key::interrupt },
        { code::key_end_of_input, key::eof },
        { code::key_tab, key::tab },
        { code::key_return, key::return_key },
        { code::escape_code, key::escape },
        { code::key_backspace, key::backspace },
        { code::key_up, key::arrow_up },
        { code::key_down, key::arrow_down },
        { code::key_right, key::arrow_right },
        { code::key_left, key::arrow_left },
        { code::key_home, key::home },
        { code::key_end, key::end },
        { code::key_insert, key::insert },
        { code::key_delete, key::delete_key }
    };

    special interpret_sequence( std::span< char > sequence )
    {
        if( sequence.empty() )
        {
            return { key::escape };
        }

        if( sequence.back() == '~' )
        {
            return vt::interpret( sequence );
        }

        auto key_it = sequence.begin() + sequence.size() - 1;
        int modifiers = 0;

        if( auto found = std::ranges::find( sequence, ';' ); found != sequence.end() )
        {
            modifiers = from_chars< int >(
                &*(found + 1),
                sequence.data() + sequence.size() - 2
            ) - 1;
        }

        return { interpret_code( *key_it ), modifiers };
    }

    key interpret_code( char code )
    {
        auto found = codes.find( code );

        if( found == codes.end() )
        {
            return key::invalid;
        }

        return found->second;
    }
}
