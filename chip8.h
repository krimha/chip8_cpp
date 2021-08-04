#pragma once

#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <ncurses.h>


namespace Chip8 {

    using Instruction = uint16_t;

    class Chip8State {
	public:
	    Chip8State();
	    static constexpr unsigned int memory_size = 0x1000;
	    static constexpr unsigned int program_start = 0x200;

	    void load_file(std::string filename);
	    /* void print_memory(); */

	    // Limited to the non IO things
	    void interpret(Instruction instruction);

	    // Instruction functions
	    void clear_display();
	    void subroutine_return();

	    // util functions
	    uint16_t get_I_register() { return I_register; }
	    uint16_t get_program_counter() { return program_counter; }
	    uint16_t get_register(size_t i) { return registers[i]; }
	    void set_display_row(size_t row, uint64_t value);
	    uint8_t get_memory(size_t addr) { return memory[addr]; };
	    uint64_t get_display_row(size_t row);
	    void push_to_stack(uint16_t addr);

        private:
            std::array<uint16_t,16> stack;
	    std::array<uint8_t,memory_size> memory;
	    std::array<uint64_t,32> display;

            // VF should never be used (used as flag in some programs
            std::array<uint8_t,16> registers;
            uint16_t I_register; // 12 lowest bits used
            uint8_t sound_register;
            uint8_t delay_register;

            uint16_t program_counter;
            uint8_t stack_pointer;

    };


    class Chip8Runner : public Chip8State {

        public:
            Chip8Runner();
            void run();
            void destroy();


        private:
            SDL_Window* window = nullptr;
            SDL_Renderer* renderer = nullptr;


            const unsigned int window_width = 64;
            const unsigned int window_height = 32;
            const unsigned int window_scale = 16;
            const unsigned int window_real_width = window_width * window_scale;
            const unsigned int window_real_height = window_height * window_scale;

            void render_symbol(uint8_t symbol);

	    void print_registers();
	    WINDOW* window_ = nullptr;
    };


    // Free functions
    Instruction assemble(std::string_view instruction);
    std::string disassemble(Instruction instuction);
    std::string get_name_from_hex(Instruction instruction);
    std::vector<std::string> split(std::string_view instruction);
    std::vector<Instruction> get_numbers(const std::vector<std::string>& tokens);

    // Takes tokenized instruction
    std::string get_unique_name(const std::vector<std::string>& instruction);
    bool safe_match(const std::vector<std::string>& instruction, int index, const std::string& target);

    // Ignore optional Vy for now (set default to 0)

    // Lookup table for tokens
    //

    enum class Field { ADDR, X, Y, BYTE, NIBBLE, IGNORE };


    // Lookup table for constant terms

}
