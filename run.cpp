#include <ncurses.h>

#include "chip8.h"

using namespace Chip8;

int main(int argc, char* argv[])
{
    /* initscr(); */
    /* refresh(); */

    Chip8Runner runner;
    runner.run();
    runner.destroy();

    /* endwin(); */

    return 0;
}
