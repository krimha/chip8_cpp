#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <filesystem>

#include "chip8.h"


using namespace Chip8;

int main(int argc, char** argv)
{
    std::filesystem::path filename { argv[1] }; 

    std::ifstream inputfile;
    std::ofstream outputfile;
    inputfile.open(filename.c_str(), std::ios::in);

    filename.replace_extension(".rom");
    outputfile.open(filename.c_str(), std::ios::binary | std::ios::out);

    if (!inputfile) {
	std::cerr << "Could not open file\n";
	return 1;
    }

    size_t counter = 0x200;
    Instruction instruction; 

    std::string line;
    while (getline(inputfile, line)) {
	if (line.size() > 2 && line[0] == '/' && line[1] == '/')
	    continue;

	/* std::cout << std::hex << assemble(line) << '\t' << line << '\n'; */
	Instruction instruction = assemble(line);
	
	uint16_t rev_endian = ((instruction & 0xFF00) >> 8) | ((instruction & 0x00FF) << 8);

	outputfile.write(reinterpret_cast<const char*>(&rev_endian), sizeof(Instruction));
    }

    inputfile.close();
    outputfile.close();
	
    return 0;
}
