#include "chip8.h"

using namespace Chip8;

int main(int argc, char* argv[])
{
    Chip8Runner runner;
    runner.run();
    runner.destroy();

    return 0;
}
