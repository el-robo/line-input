#include "input.hpp"
#include <cstdlib>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disable_raw_mode()
{
    tcsetattr( STDIN_FILENO, TCSAFLUSH, &orig_termios );
}

void enable_raw_mode()
{
    struct termios raw;
    tcgetattr( STDIN_FILENO, &raw );
    raw.c_lflag &= ~( ECHO | ICANON );
    tcsetattr( STDIN_FILENO, TCSAFLUSH, &raw );
}

std::generator< char > input::keys()
{
    tcgetattr( STDIN_FILENO, &orig_termios );
    std::atexit(disable_raw_mode);

    enable_raw_mode();

    char input;
    while( active && read( STDIN_FILENO, &input, 1 ) == 1 )
    {
        co_yield input;
    }
}
