#include <string>

#include <ncurses.h>

#include "chip8.h"

using namespace Chip8;

int main(int argc, char* argv[])
{
    Chip8Runner runner;
    runner.load_file(std::string(argv[1]));
    runner.run();
    runner.destroy();

    return 0;
}
