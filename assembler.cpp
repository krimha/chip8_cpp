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

    size_t addr = 0x200;
    Instruction instruction; 

    std::string line;

    std::unordered_map<std::string,size_t> labels;
    std::vector<std::string> lines;

    while (getline(inputfile, line)) {
	// Detect comments
	
	if (line[line.find_first_not_of(' ')] == '#') {
	    continue;
	}
	else if (line[line.find_first_not_of(' ')] == ':') {
	    labels.insert({line, addr}); 
	} else {
	    lines.push_back(line);
	    addr += 2;
	}
    }

    for (const auto& line : lines) {
	Instruction instruction = assemble(line, labels);
	
	uint16_t rev_endian = ((instruction & 0xFF00) >> 8) | ((instruction & 0x00FF) << 8);

	outputfile.write(reinterpret_cast<const char*>(&rev_endian), sizeof(Instruction));
    }

    inputfile.close();
    outputfile.close();
	
    return 0;
}
