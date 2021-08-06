#include "chip8.h"

#include <unordered_map>
#include <string_view>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <ncurses.h>

namespace Chip8 {
    static const std::unordered_map<int,uint8_t> scan_map = {
        {SDL_SCANCODE_1, 0x1}, 
        {SDL_SCANCODE_2, 0x2},
        {SDL_SCANCODE_3, 0x3},
        {SDL_SCANCODE_4, 0xC},
        {SDL_SCANCODE_Q, 0x4},
        {SDL_SCANCODE_W, 0x5},
        {SDL_SCANCODE_E, 0x6},
        {SDL_SCANCODE_R, 0xD},
        {SDL_SCANCODE_A, 0x7},
        {SDL_SCANCODE_S, 0x8},
        {SDL_SCANCODE_D, 0x9},
        {SDL_SCANCODE_F, 0xE},
        {SDL_SCANCODE_Z, 0xA},
        {SDL_SCANCODE_X, 0x0},
        {SDL_SCANCODE_C, 0xB},
        {SDL_SCANCODE_V, 0xF}
    };


    Chip8State::Chip8State()
	: I_register{0}
	, sound_register{0}
	, delay_register{0}
	, program_counter{0x200}
	, stack_pointer{0}
	, dist{0, 255}
    {

	for (size_t i=0; i<registers.size(); ++i)
	    registers[i] = 0;

	for (size_t i=0; i<stack.size(); ++i)
	    stack[i] = 0;

	for (size_t i=0; i<memory.size(); ++i)
	    memory[i] = 0;

	for (size_t i=0; i<display.size(); ++i)
	    display[i] = 0;

	for (size_t i=0; i<keyboard.size(); ++i)
	    keyboard[i] = 0;

        std::array<uint8_t,80> sprites = { 0xF0, 0x90, 0x90, 0x90, 0xF0,
					 0x20, 0x60, 0x20, 0x20, 0x70, 
					 0xF0, 0x10, 0xF0, 0x80, 0xF0, 
					 0xF0, 0x10, 0xF0, 0x10, 0xF0,
					 0x90, 0x90, 0xF0, 0x10, 0x10,
					 0xF0, 0x80, 0xF0, 0x10, 0xF0,
					 0xF0, 0x80, 0xF0, 0x90, 0xF0,
					 0xF0, 0x10, 0x20, 0x40, 0x40,
					 0xF0, 0x90, 0xF0, 0x90, 0xF0,
					 0xF0, 0x90, 0xF0, 0x10, 0xF0,
					 0xF0, 0x90, 0xF0, 0x90, 0x90,
					 0xE0, 0x90, 0xE0, 0x90, 0xE0,
					 0xF0, 0x80, 0x80, 0x80, 0xF0,
					 0xE0, 0x90, 0x90, 0x90, 0xE0,
					 0xF0, 0x80, 0xF0, 0x80, 0xF0,
					 0xF0, 0x80, 0xF0, 0x80, 0x80 };

	for (size_t i=0; i<sprites.size(); ++i)
	    memory[i] = sprites[i];

	// Use with dist(mt);
	mt.seed(rd());
    }


    void Chip8State::load_file(std::string filename)
    {
	std::ifstream inputfile;
	inputfile.open(filename, std::ios::in | std::ios::binary);

	/* if (!inputfile) */
	/*     throw std::exception("Could not open file"); */

	// TODO: What to do if file does not fit in memory
	size_t counter = 0x200;
	Instruction instruction; 
	while (inputfile.good() && !inputfile.eof()) {
	    inputfile.read((char*)(&instruction), sizeof(Instruction)); 
	    uint16_t rev_endian = ((instruction & 0x00ff) << 8 ) | ((instruction & 0xff00) >> 8);

	    // TODO: Possible to simplify this?
	    memory[counter] = (0xFF00 & rev_endian) >> 8;
	    memory[counter+1] = 0x00FF & rev_endian;
	    counter += 2;
	}
    }

    void Chip8State::set_display_row(size_t row, uint64_t value)
    {
	display[row] = value;
    }

    uint64_t Chip8State::get_display_row(size_t row)
    {
	return display[row];
    }


    void Chip8State::push_to_stack(uint16_t addr)
    {
	stack_pointer++;
	stack[stack_pointer] = addr;
    }
    

    // Instruction specific

    void Chip8State::clear_display()
    {
	for (size_t i=0; i<display.size(); ++i)
	    display[i] = 0;
    }


    void Chip8State::subroutine_return()
    {
	program_counter = stack[stack_pointer--];
    }

    void Chip8State::jump_to_addr(uint16_t addr)
    {
	program_counter = addr;
    }

    void Chip8State::interpret(Instruction instruction)
    {
	const uint8_t first = (instruction & 0xF000) >> 12;
	const uint8_t x = (instruction & 0x0F00) >> 8;
	const uint8_t y = (instruction & 0x00F0) >> 4;
	const uint8_t nibble = instruction & 0x000F;
	const uint8_t kk = instruction & 0x00FF;
	const uint16_t addr = instruction & 0x0FFF;

	const auto val_x = get_register(x);
	const auto val_y = get_register(y);
	
	if (instruction == 0x00E0)
	    clear_display();
	else if (instruction == 0x00EE)
	    subroutine_return();
	else if (first == 1)
	    jump_to_addr(addr);
	else if (first == 2) { // CALL addr
	    push_to_stack(program_counter);
	    program_counter = addr;
	}
	else if (first == 3) { // SE Vx, byte
	    if (registers[x] == kk)
		program_counter += 2;
	}
	else if (first == 4) {
	    if (registers[x] != kk)
		program_counter += 2;
	}
	else if (first == 5) {
	    if (registers[x] == registers[y])
		program_counter += 2;
	}
	else if (first == 6) {
	    set_register(x, kk);
	}
	else if (first == 7) {
	    const auto old_value = get_register(x);
	    set_register(x, old_value+kk);
	}
	else if (first == 8 && nibble == 0) {
	    set_register(x, get_register(y));
	}
	else if (first == 8 && nibble == 1) {
	    set_register(x, get_register(x) | get_register(y));
	}
	else if (first == 8 && nibble == 2) {
	    set_register(x, get_register(x) & get_register(y));
	}
	else if (first == 8 && nibble == 3) {
	    set_register(x, get_register(x) ^ get_register(y));
	}
	else if (first == 8 && nibble == 4) {
	    auto result = get_register(x) + get_register(y);
	    auto vf_res = result > 255 ? 1 : 0;

	    set_register(x, static_cast<uint8_t>(result));
	    set_register(0xF, vf_res);
	}
	else if (first == 8 && nibble == 5) { // Assumed that underflow is allowed (desired)
	    auto val_x = get_register(x);
	    auto val_y = get_register(y);
	    set_register(x, val_x - val_y);

	    auto vf_res = val_x < val_y ? 1 : 0;
	    set_register(0xF, 1);
	}
	else if (first == 8 & nibble == 6) {
	    const auto res_vf = (val_x & 0x01) == 0 ? 0 : 1;
	    set_register(0xF, res_vf);
	    set_register(x, val_x >> 1);
	}
	else if (first == 8 && nibble == 7) {
	    set_register(x, val_y - val_x);

	    auto vf_res = val_x < val_y ? 1 : 0;
	    set_register(0xF, vf_res);
	}
	else if (first == 8 && nibble == 0xE) {
	    set_register(0xF, (val_x & 0x80) >> 7);
	    set_register(x, val_x << 1);
	}
	else if (first == 9 && nibble == 0) {
	    program_counter += (val_x == val_y) ? 0 : 2;
	}
	else if (first == 0xA) {
	    I_register = addr;
	}
	else if (first == 0xB) {
	    program_counter = addr + get_register(0);
	}
	else if (first == 0xC) {
	    set_register(x, dist(mt) & kk);
	}
	else if (first == 0xD) {
	    const auto x_pos = val_x % 64;
	    const auto y_pos = val_y % 32;

	    bool collision = 0;

	    for (uint16_t i=0; i<nibble; ++i) {
		auto sprite_row = get_memory(get_I_register() + i);
		
		// Move sprite to beginning of line
		uint64_t row = static_cast<uint64_t>(sprite_row) << ((14-x_pos)*4);
		auto curr_row = i+y_pos;
		if (curr_row >= display.size())
		    break;

		collision |= (row & display[curr_row]) > 0;
		display[curr_row] ^= row;
	    }
	    set_register(0xF, collision ? 1 : 0); 
	}
	else if (first == 0xE && kk == 0x9E) {
	    if (is_pressed(val_x))
		program_counter += 2;
	}
	else if (first == 0xE && kk == 0xA1) {
	    if (!is_pressed(val_x))
		program_counter += 2;
	}
	else if (first == 0xF && kk == 0x07) {
	    set_register(x, get_delay_register());
	}
	else if (first == 0xF && kk == 0x0A) {
	    std::cerr << "Instruction 0xFx0A has not been implemented\n";
	}
	else if (first == 0xF && kk == 0x15) {
	    set_delay_register(val_x);
	}
	else if (first == 0xF && kk == 0x18) {
	    set_sound_register(val_x);
	}
	else if (first == 0xF && kk == 0x1E)
	{
	    set_I_register(get_I_register() + val_x);
	}
	else if (first == 0xF && kk == 0x29)
	{
	    set_I_register(5*val_x);
	}

    }

    void Chip8Runner::print_registers()
    {
	constexpr size_t padding = 6;
	size_t curr_y = 1;
	static constexpr size_t start_x = 5;

	{
	    // I registers
	    wmove(window_, curr_y, start_x);
	    waddstr(window_, "I:");
	    std::stringstream ss;
	    ss << std::setfill('0') << std::setw(4) << std::hex << static_cast<int>(get_I_register());
	    waddstr(window_, ss.str().c_str());
	}

	curr_y += 2;

	{
	    wmove(window_, curr_y, start_x);
	    waddstr(window_, "PC:");
	    std::stringstream ss;
	    ss << std::setfill('0') << std::setw(4) << std::hex << static_cast<int>(get_program_counter());
	    waddstr(window_, ss.str().c_str());
	}

	curr_y += 2;

	for (size_t i=0; i<=0xF; ++i) {
	    std::stringstream ss;
	    ss << "V" << std::hex << i;

	    wmove(window_, curr_y, start_x+i*padding);
	    waddstr(window_, ss.str().c_str());
	}

	curr_y += 1;

	for (size_t i=0; i<=0xF; ++i) {
	    std::stringstream ss;
	    ss << std::setfill('0') << std::setw(4) <<  std::hex << static_cast<int>(get_register(i));

	    wmove(window_, curr_y, start_x+i*padding);
	    waddstr(window_, ss.str().c_str());
	}

	curr_y += 2;

	const size_t y_mem_start = curr_y;

	const size_t mem_start = 0x200; 
	const size_t mem_padding = 0x4; 

	const size_t per_row = 32;

	size_t curr_mem = mem_start;
	for (size_t num_row=0; num_row<10; ++num_row) {
	    std::stringstream ss;
	    ss << std::hex << std::setfill('0') << std::setw(3) << static_cast<int>(curr_mem) << "  ";
	    for (size_t i=curr_mem; i<curr_mem+per_row; ++i) {
		ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(get_memory(i)) << ' ';
	    }
	    wmove(window_, curr_y, start_x);
	    waddstr(window_, ss.str().c_str());

	    curr_y++;
	    curr_mem += per_row;
	}

	wrefresh(window_);
    }

    Chip8Runner::Chip8Runner() : Chip8State::Chip8State()
			       , window_{initscr()}
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

	print_registers();

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

	endwin();
    }


    void Chip8Runner::render_symbol(uint8_t symbol) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        const std::unordered_map<char,std::array<uint8_t,5>> sprites = {
            { 0x0, { 0xF0, 0x90, 0x90, 0x90, 0xF0 }},
            { 0x1, { 0x20, 0x60, 0x20, 0x20, 0x70 }},
            { 0x2, { 0xF0, 0x10, 0xF0, 0x80, 0xF0 }},
            { 0x3, { 0xF0, 0x10, 0xF0, 0x10, 0xF0 }},
            { 0x4, { 0x90, 0x90, 0xF0, 0x10, 0x10 }},
            { 0x5, { 0xF0, 0x80, 0xF0, 0x10, 0xF0 }},
            { 0x6, { 0xF0, 0x80, 0xF0, 0x90, 0xF0 }},
            { 0x7, { 0xF0, 0x10, 0x20, 0x40, 0x40 }},
            { 0x8, { 0xF0, 0x90, 0xF0, 0x90, 0xF0 }},
            { 0x9, { 0xF0, 0x90, 0xF0, 0x10, 0xF0 }},
            { 0xA, { 0xF0, 0x90, 0xF0, 0x90, 0x90 }},
            { 0xB, { 0xE0, 0x90, 0xE0, 0x90, 0xE0 }},
            { 0xC, { 0xF0, 0x80, 0x80, 0x80, 0xF0 }},
            { 0xD, { 0xE0, 0x90, 0x90, 0x90, 0xE0 }},
            { 0xE, { 0xF0, 0x80, 0xF0, 0x80, 0xF0 }},
            { 0xF, { 0xF0, 0x80, 0xF0, 0x80, 0x80 }}};

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
	    { "CLS"      , 0x00E0 },
	    { "RET"      , 0x00EE },
	    { "SYS"      , 0x0000 },
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



    std::unordered_map<std::string,std::pair<std::vector<std::string>,std::vector<Field>>> formats {
	    {"CLS",       {{ "CLS" }, {}}}
	    ,{"RET",       {{ "RET" }, {}}}
            ,{"SYS",       {{"SYS "}, {Field::ADDR}} }
	    ,{"JPaddr"   , {{"JP "}, {Field::ADDR}}}
	    ,{"CALL"     , {{"CALL "}, {Field::ADDR}}}
	    ,{"SEVxbyte" , {{"SE V", ", "},  {Field::X, Field::BYTE}}}
	    ,{"SNEVxbyte", {{"SNE V", ", "}, {Field::X, Field::BYTE}}}
	    ,{"SEVxVy"   , {{"SE V", ", V"}, {Field::X, Field::Y}}}
	    ,{"LDVxbyte" , {{"LD V", ", "}, {Field::X, Field::BYTE}}}
	    ,{"ADDVxbyte", {{"ADD V", ", "}, {Field::X, Field::BYTE}}}
	    ,{"LDVxVy"   , {{"LD V", ", V"}, {Field::X, Field::Y}}}
	    ,{"OR"       , {{"OR V", ", V"}, {Field::X, Field::Y}}}
	    ,{"AND"      , {{"AND V", ", V"}, {Field::X, Field::Y}}}
	    ,{"XOR"      , {{"XOR V", ", V"}, {Field::X, Field::Y}}}
	    ,{"ADDVxVy"  , {{"ADD V", ", V"}, {Field::X, Field::Y}}}
	    ,{"SUB"      , {{"SUB V", ", V"}, {Field::X, Field::Y}}}
	    ,{"SHR"      , {{"SHR V", ", V"}, {Field::X, Field::Y}}}
	    ,{"SUBN"     , {{"SUBN V", ", V"}, {Field::X, Field::Y}}}
	    ,{"SHL"      , {{"SHL V", ", V"}, {Field::X , Field::Y}}}
	    ,{"SNEVxVy"  , {{"SNE V", ", V"}, {Field::X, Field::Y}}}
	    ,{"LDIaddr"  , {{"LD I, "}, {Field::ADDR}}}
	    ,{"JPV0addr" , {{"JP V0, "}, {Field::ADDR}}}
	    ,{"RND"      , {{"RND V", ", "}, {Field::X, Field::BYTE}}}
	    ,{"DRW"      , {{"DRW V", ", V", ", "}, {Field::X, Field::Y, Field::NIBBLE}}}
	    ,{"SKP"      , {{"SKP V"}, {Field::X}}}
	    ,{"SKNP"     , {{"SKNP V"}, {Field::X}}}
	    ,{"LDVxDT"   , {{"LD V", ", DT"}, {Field::X}}}
	    ,{"LDVxK"    , {{"LD V", ", K"}, {Field::X}}}
	    ,{"LDDTVx"   , {{"LD DT, V"}, {Field::X}}}
	    ,{"LDSTVx"   , {{"LD ST, V"}, {Field::X}}}
	    ,{"ADDIVx"   , {{"ADD I, V"}, {Field::X}}}
	    ,{"LDFVx"    , {{"LD F, V"}, {Field::X}}}
	    ,{"LDBVx"    , {{"LD B, V"},{Field::X}}}
	    ,{"LDIVx"    , {{"LD [I], V"}, {Field::X}}}
	    ,{"LDVxI"    , {{"LD V", ", [I]"}, {Field::X}}}
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
	else if (instruction >> 12 == 0x9) return "SNEVxVy";  
	else if (instruction >> 12 == 0xA) return "LDIaddr";  
	else if (instruction >> 12 == 0xB) return "JPV0addr";  
	else if (instruction >> 12 == 0xC) return "RND";  
	else if (instruction >> 12 == 0xD) return "DRW";  
	else if (instruction >> 12 == 0xE && (instruction & 0x00FF) == 0x9E) return "SKP";  
	else if (instruction >> 12 == 0xE && (instruction & 0x00FF) == 0xA1) return "SKNP";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x07) return "LDVxDT";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x0A) return "LDVxK";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x15) return "LDDTVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x18) return "LDSTVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x1E) return "ADDIVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x29) return "LDFVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x33) return "LDBVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x55) return "LDIVx";  
	else if (instruction >> 12 == 0xF && (instruction & 0x00FF) == 0x65) return "LDVxI";  

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

    int get_field(Instruction instruction, Field field) 
    {
            switch (field) {
		// 0x0nnn
                case Field::ADDR:
		    return 0x0FFF & instruction;
                case Field::BYTE:
		    return 0x00FF & instruction;
                case Field::X:
		    return (0x0F00 & instruction) >> 8;
                case Field::Y:
		    return (0x00F0 & instruction) >> 4;
                case Field::NIBBLE:
		    return 0x000F & instruction;
		case Field::IGNORE:
		    return 0;
            }
	    return 0;
    }

    std::string disassemble(Instruction instruction)
    {
	std::stringstream ss;

	std::string instruction_name = get_name_from_hex(instruction);
	const auto& [literals, fields] = formats.at(instruction_name);
	

	// Alternate between the two arrays
	size_t literal_index = 0;
	size_t field_index = 0;
	while (literal_index < literals.size() || field_index < fields.size()) {
	    if (literal_index < literals.size()) {
		ss << literals[literal_index];	
		++literal_index;
	    }

	    if (field_index < fields.size()) {
		ss << get_field(instruction, fields[field_index]);
		++field_index;
	    }
	}

	return ss.str();
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

