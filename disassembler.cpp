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
    inputfile.open(argv[1], std::ios::in | std::ios::binary);

    if (!inputfile) {
	std::cerr << "Could not open file\n";
	return 1;
    }

    size_t counter = 0x200;
    Instruction instruction; 
    while (inputfile.good() && !inputfile.eof()) {

	inputfile.read((char*)(&instruction), sizeof(Instruction)); 
	uint16_t rev_endian = ((instruction & 0x00ff) << 8 ) | ((instruction & 0xff00) >> 8);

	std::string text_other;
	try {
	    text_other = disassemble(rev_endian);
	} catch (std::exception e) {
	    // TODO: Write out binary representation (must be sprite?)
	    text_other = "";
	}

	std::cout << std::hex << counter << ' ' << std::hex << rev_endian << ' ' << text_other << '\n';
	counter += 2;
    }
    return 0;
}
