#include "chip8.h"

#include <unordered_map>
#include <string_view>
#include <string>
#include <sstream>
#include <iostream>

namespace Chip8 {
    static const std::unordered_map<int,char> scan_map = {
        {SDL_SCANCODE_1, '1'}, 
        {SDL_SCANCODE_2, '2'},
        {SDL_SCANCODE_3, '3'},
        {SDL_SCANCODE_4, 'C'},
        {SDL_SCANCODE_Q, '4'},
        {SDL_SCANCODE_W, '5'},
        {SDL_SCANCODE_E, '6'},
        {SDL_SCANCODE_R, 'D'},
        {SDL_SCANCODE_A, '7'},
        {SDL_SCANCODE_S, '8'},
        {SDL_SCANCODE_D, '9'},
        {SDL_SCANCODE_F, 'E'},
        {SDL_SCANCODE_Z, 'A'},
        {SDL_SCANCODE_X, '0'},
        {SDL_SCANCODE_C, 'B'},
        {SDL_SCANCODE_V, 'F'}
    };


    Chip8Runner::Chip8Runner()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return;

        SDL_CreateWindowAndRenderer(window_real_width, window_real_height, 0, &window, &renderer);
        SDL_RenderSetLogicalSize(renderer, window_width, window_height);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255,255, 255, 255);

        SDL_RenderPresent(renderer);
    }

    void Chip8Runner::run()
    {
        bool closed = false;
        while (!closed) {
            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        closed = true;
                        break;

                        // Handle keypresses
                    case SDL_KEYDOWN:
                        auto sc_code = event.key.keysym.scancode;
                        if (scan_map.find(sc_code) != scan_map.end()) {
                            auto symbol = scan_map.at(sc_code);
                            render_symbol(symbol); 
                        }
                }
            }
        }
    }


    void Chip8Runner::destroy()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }


    void Chip8Runner::render_symbol(char symbol) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        const std::unordered_map<char,std::array<uint16_t,5>> sprites = {
            { '0', { 0xF0, 0x90, 0x90, 0x90, 0xF0 }},
            { '1', { 0x20, 0x60, 0x20, 0x20, 0x70 }},
            { '2', { 0xF0, 0x10, 0xF0, 0x80, 0xF0 }},
            { '3', { 0xF0, 0x10, 0xF0, 0x10, 0xF0 }},
            { '4', { 0x90, 0x90, 0xF0, 0x10, 0x10 }},
            { '5', { 0xF0, 0x80, 0xF0, 0x10, 0xF0 }},
            { '6', { 0xF0, 0x80, 0xF0, 0x90, 0xF0 }},
            { '7', { 0xF0, 0x10, 0x20, 0x40, 0x40 }},
            { '8', { 0xF0, 0x90, 0xF0, 0x90, 0xF0 }},
            { '9', { 0xF0, 0x90, 0xF0, 0x10, 0xF0 }},
            { 'A', { 0xF0, 0x90, 0xF0, 0x90, 0x90 }},
            { 'B', { 0xE0, 0x90, 0xE0, 0x90, 0xE0 }},
            { 'C', { 0xF0, 0x80, 0x80, 0x80, 0xF0 }},
            { 'D', { 0xE0, 0x90, 0x90, 0x90, 0xE0 }},
            { 'E', { 0xF0, 0x80, 0xF0, 0x80, 0xF0 }},
            { 'F', { 0xF0, 0x80, 0xF0, 0x80, 0x80 }}};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        auto sprite = sprites.at(symbol);
        for (int row=0; row<sprite.size(); ++row) {
            auto cursor = 0x80;
            const auto x = sprite[row];
            for (int col=0; col<8; ++col) {
                if ((cursor & x) == cursor)
                    SDL_RenderDrawPoint(renderer, col, row);
                cursor >>= 1;
            }
        }
        SDL_RenderPresent(renderer);
    }

    std::unordered_map<std::string, uint16_t> base_map {
	{ "SYS"      , 0x0000 },
	    { "CLS"      , 0x00E0 },
	    { "RET"      , 0x00EE },
	    { "JPaddr"   , 0x1000 },
	    { "CALL"     , 0x2000 },
	    { "SEVxbyte" , 0x3000 },
	    { "SNEVxbyte", 0x4000 },
	    { "SEVxVy"   , 0x5000 },
	    { "LDVxbyte" , 0x6000 },
	    { "ADDVxbyte", 0x7000 },
	    { "LDVxVy"   , 0x8000 },
	    { "OR"       , 0x8001 },
	    { "AND"      , 0x8002 },
	    { "XOR"      , 0x8003 },
	    { "ADDVxVy"  , 0x8004 },
	    { "SUB"      , 0x8005 },
	    { "SHR"      , 0x8006 },
	    { "SUBN"     , 0x8007 },
	    { "SHL"      , 0x800E },
	    { "SNEVxVy"  , 0x9000 },
	    { "LDIaddr"  , 0xA000 },
	    { "JPV0addr" , 0xB000 },
	    { "RND"      , 0xC000 },
	    { "DRW"      , 0xD000 },
	    { "SKP"      , 0xE09E },
	    { "SKNP"     , 0xE0A1 },
	    { "LDVxDT"   , 0xF007 },
	    { "LDVxK"    , 0xF00A },
	    { "LDDTVx"   , 0xF015 },
	    { "LDSTVx"   , 0xF018 },
	    { "ADDIVx"   , 0xF01E },
	    { "LDFVx"    , 0xF029 },
	    { "LDBVx"    , 0xF033 },
	    { "LDIVx"    , 0xF055 },
	    { "LDVxI"    , 0xF065 }
    };

    std::vector<std::pair<uint16_t, std::string>> base_pairs {
	    { 0x00E0, "CLS" },
	    { 0x00EE, "RET" },
	    { 0x1000, "JPaddr" },
	    { 0x2000, "CALL" },
	    { 0x3000, "SEVxbyte" },
	    { 0x4000, "SNEVxbyte" },
	    { 0x5000, "SEVxVy" },
	    { 0x6000, "LDVxbyte" },
	    { 0x7000, "ADDVxbyte" },
	    { 0x8000, "LDVxVy" },
	    { 0x8001, "OR" },
	    { 0x8002, "AND" },
	    { 0x8003, "XOR" },
	    { 0x8004, "ADDVxVy" },
	    { 0x8005, "SUB" },
	    { 0x8006, "SHR" },
	    { 0x8007, "SUBN" },
	    { 0x800E, "SHL" },
	    { 0x9000, "SNEVxVy" },
	    { 0xA000, "LDIaddr" },
	    { 0xB000, "JPV0addr" },
	    { 0xC000, "RND" },
	    { 0xD000, "DRW" },
	    { 0xE09E, "SKP" },
	    { 0xE0A1, "SKNP" },
	    { 0xF007, "LDVxDT" },
	    { 0xF00A, "LDVxK" },
	    { 0xF015, "LDDTVx" },
	    { 0xF018, "LDSTVx" },
	    { 0xF01E, "ADDIVx" },
	    { 0xF029, "LDFVx" },
	    { 0xF033, "LDBVx" },
	    { 0xF055, "LDIVx" },
	    { 0xF065, "LDVxI" },
    };

    std::string get_name_from_hex(Instruction instruction)
    {
	if (instruction == 0x00E0) return "CLS";
	else if (instruction == 0x00EE) return "RET";
	else if (instruction >> 12 == 0x0) return "SYS";
	else if (instruction >> 12 == 0x1) return "JPaddr";
	else if (instruction >> 12 == 0x2) return "CALL";
	else if (instruction >> 12 == 0x3) return "SEVxbyte";  
	else if (instruction >> 12 == 0x4) return "SNEVxbyte"; 
	else if (instruction >> 12 == 0x5) return "SEVxVy";    
	else if (instruction >> 12 == 0x6) return "LDVxbyte";  
	else if (instruction >> 12 == 0x7) return "ADDVxbyte"; 
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x0) return "LDVxVy";
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x1) return "OR";   
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x2) return "AND";  
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x3) return "XOR";  
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x4) return "ADDVxVy";
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x5) return "SUB";  
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x6) return "SHR";  
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0x7) return "SUBN"; 
	else if (instruction >> 12 == 0x8 && (instruction & 0x000F) == 0xE) return "SHL";  
	else if (instruction >> 12 == 0x9)  return "SNEVxVy";  
	else if (instruction >> 12 == 0xA) return "LDIaddr" ;  
	else if (instruction >> 12 == 0xB) return "JPV0addr";  
	else if (instruction >> 12 == 0xC) return "RND"     ;  
	else if (instruction >> 12 == 0xD) return "DRW"     ;  
	else if (instruction >> 12 == 0xE && (instruction & 0x00FF) == 0x9E) return "SKP"     ;  
	else if (instruction >> 12 == 0xE && (instruction & 0x00FF) == 0xA1) return "SKNP"     ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x07) return "LDVxDT" ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x0A) return "LDVxK"  ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x15) return "LDDTVx" ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x18) return "LDSTVx" ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x1E) return "ADDIVx" ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x29) return "LDFVx"  ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x33) return "LDBVx"  ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x55) return "LDIVx"  ;  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x65) return "LDVxI"  ;  








	std::stringstream ss;
	ss << "Could not find match for instruction " << std::hex << instruction << '\n';
	throw std::runtime_error(ss.str());
    }

    std::unordered_map<std::string, std::vector<Field>> fields_map {
	{ "SYS"       , { Field::ADDR }}
	,{"CLS"       , { } }
	,{"RET"       , { }}
	,{"JPaddr"    , { Field::ADDR }}
	,{"CALL"      , { Field::ADDR }}
	,{"SEVxbyte"  , { Field::X, Field::BYTE }}
	,{"SNEVxbyte" , { Field::X, Field::BYTE }}
	,{"SEVxVy"    , { Field::X, Field::Y }}
	,{"LDVxbyte"  , { Field::X, Field::BYTE }}
	,{"ADDVxbyte" , { Field::X, Field::BYTE }}
	,{"LDVxVy"    , { Field::X, Field::Y }} 
	,{"OR"        , { Field::X, Field::Y }}
	,{"AND"       , { Field::X, Field::Y }}
	,{"XOR"       , { Field::X, Field::Y }}
	,{"ADDVxVy"   , { Field::X, Field::Y }}
	,{"SUB"       , { Field::X, Field::Y }}
	,{"SHR"       , { Field::X, Field::Y }}
	,{"SUBN"      , { Field::X, Field::Y }}
	,{"SHL"       , { Field::X, Field::Y }}
	,{"SNEVxVy"   , { Field::X, Field::Y }}
	,{"LDIaddr"   , { Field::ADDR }}
	,{"JPV0addr"  , { Field::IGNORE, Field::ADDR }}
	,{"RND"       , { Field::X, Field::BYTE }}
	,{"DRW"       , { Field::X, Field::Y, Field::NIBBLE }}
	,{"SKP"       , { Field::X }}
	,{"SKNP"      , { Field::X }}
	,{"LDVxDT"    , { Field::X }}
	,{"LDVxK"     , { Field::X }}
	,{"LDDTVx"    , { Field::X }}
	,{"LDSTVx"    , { Field::X }}
	,{"ADDIVx"    , { Field::X }}
	,{"LDFVx"     , { Field::X }}
	,{"LDBVx"     , { Field::X }}
	,{"LDIVx"     , { Field::X }}
	,{"LDVxI"     , { Field::X }}};



    Instruction assemble(std::string_view instruction)
    {


        auto tokens = split(instruction);
        auto numbers = get_numbers(tokens);
        auto name = get_unique_name(tokens);
        auto parts = fields_map.at(name.c_str());

        Instruction result = base_map.at(name.c_str());

        for (int i=0; i<parts.size(); ++i) {
            auto part = parts[i];
            auto num = numbers[i];

            switch (part) {
                case Field::ADDR:
                    result |= 0x0FFF & num;
                    break;
                case Field::BYTE:
                    result |= 0x00FF & num;
                    break;
                case Field::X:
                    result |= 0x0F00 & (num << 8);
                    break;
                case Field::Y:
                    result |= 0x00F0 & (num << 4);
                    break;
                case Field::NIBBLE:
                    result |= 0x000F & num;
                    break;
		case Field::IGNORE:
		    continue;
            }
        }

        return result;
    }

    std::vector<std::string> split(std::string_view instruction)
    {
        std::vector<std::string> result;
        std::string item;
        for (const auto& sym : instruction) {
            if (sym == ' ' || sym == ',') {
                if (item.size() > 0)
                    result.push_back(item);
                item.clear();
                continue;
            } else {
                item.push_back(sym);
            }
        }
        if (item.size() > 0)
            result.push_back(item);

        return result;
    }



    std::vector<Instruction> get_numbers(const std::vector<std::string>& tokens)
    {
        std::vector<Instruction> result;

        for (const auto& token : tokens) {
            if (token[0] >= '0' && token[0] <= '9') {
                uint16_t num;
                std::istringstream(token) >> num;
                result.push_back(num);
            } else if (token[0] == 'V') {
                uint16_t num;
                std::istringstream(token.substr(1)) >> num;
                result.push_back(num);
            }
        }

        return result;
    }

    std::string get_unique_name(const std::vector<std::string>& instruction)
    {
        if (instruction.size() < 1)
            throw std::runtime_error("Empty instruction");
        std::string name = instruction[0];
        if (name == "ADD") {
            if (safe_match(instruction, 1, "I") && safe_match(instruction, 2, "V"))
                name = "ADDIVx";
            else if (safe_match(instruction, 2, "V"))
                name = "ADDVxVy";
            else if (safe_match(instruction, 1, "V"))
                name = "ADDVxbyte";
        }
        else if (name == "JP") {
            if (safe_match(instruction, 1, "V0"))
                name = "JPV0addr";
            else
                name = "JPaddr";
        }
        else if (name == "LD") {
            if (safe_match(instruction, 1, "V") && safe_match(instruction, 2, "K"))
                name = "LDVxK";
            else if (safe_match(instruction, 1, "V") && safe_match(instruction, 2, "V"))
                name = "LDVxVy";
            else if (safe_match(instruction, 1, "V") && safe_match(instruction, 2, "DT"))
                name = "LDVxDT";
            else if (safe_match(instruction, 1, "V") && safe_match(instruction, 2, "[I]"))
                name = "LDVxI";
            else if (safe_match(instruction, 1, "DT") && safe_match(instruction, 2, "V"))
                name = "LDDTVx";
            else if (safe_match(instruction, 1, "ST") && safe_match(instruction, 2, "V"))
                name = "LDSTVx";
            else if (safe_match(instruction, 1, "F") && safe_match(instruction, 2, "V"))
                name = "LDFVx";
            else if (safe_match(instruction, 1, "B") && safe_match(instruction, 2, "V"))
                name = "LDBVx";
            else if (safe_match(instruction, 1, "[I]") && safe_match(instruction, 2, "V"))
                name = "LDIVx";
            else if (safe_match(instruction, 1, "V"))
                name = "LDVxbyte";
            else if (safe_match(instruction, 1, "I"))
                name = "LDIaddr";
        }
        else if (name == "SE") {
            if (safe_match(instruction, 2, "V"))
                name = "SEVxVy";
            else if (safe_match(instruction, 1, "V"))
                name = "SEVxbyte";
        }
        else if (name == "SNE") {
            if (safe_match(instruction, 2, "V"))
                name = "SNEVxVy";
            else if (safe_match(instruction, 1, "V"))
                name = "SNEVxbyte";
        }

        return name;
    }

    bool safe_match(const std::vector<std::string>& instruction, int index, const std::string& target) 
    {
        if (index < 0 || index >= instruction.size())
            return false;

        return instruction[index].rfind(target, 0) == 0;
    }
}
