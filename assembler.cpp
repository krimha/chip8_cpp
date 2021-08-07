#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "chip8.h"


using namespace Chip8;

int main(int argc, char** argv)
{
    std::string filename { argv[1] }; 

    std::ifstream inputfile;
    inputfile.open(argv[1], std::ios::in);

    if (!inputfile) {
	std::cerr << "Could not open file\n";
	return 1;
    }

    size_t counter = 0x200;
    Instruction instruction; 

    std::string line;
    while (getline(inputfile, line)) {
	std::cout << std::hex << assemble(line) << '\t' << line << '\n';
    }
	
    return 0;
}
