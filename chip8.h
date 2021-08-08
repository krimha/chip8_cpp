#pragma once

#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <random>
#include <optional>

#include <iostream>

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
	    void jump_to_addr(uint16_t addr);

	    // util functions
	    uint16_t get_I_register() const { return I_register; }
	    uint16_t get_program_counter() const { return program_counter; }
	    uint16_t get_register(size_t i) const { return registers[i]; }
	    uint8_t get_memory(size_t addr) const { return memory[addr]; };
	    uint16_t stack_peek() const { return stack[stack_pointer]; }
	    uint8_t get_stack_pointer() const { return stack_pointer; }
	    uint8_t get_delay_register() const { return delay_register; }
	    uint8_t get_sound_register() const { return sound_register; }

	    void set_program_counter(uint16_t addr) { program_counter = addr; }
	    void set_display_row(size_t row, uint64_t value);
	    void set_register(uint8_t reg, uint8_t value) { registers[reg] = value; }
	    void set_I_register(uint16_t addr) { I_register = addr; }
	    void set_delay_register(uint8_t value) { delay_register = value; }
	    void set_sound_register(uint8_t value) { sound_register = value; }
	    void set_memory(uint16_t addr, uint8_t value) { memory[addr] = value; }

	    void wait_for_input() { waiting = true; }
	    void stop_waiting() { waiting = false; }
	    bool is_waiting() { return waiting; }

	    void set_key(uint8_t key, bool value) {
		std::cerr << "Setting key " << std::hex << static_cast<int>(key) << " to " << value << '\n';
		keyboard[key] = value; 

	    }
	    bool is_pressed(uint8_t key) const { return keyboard[key]; }

	    uint64_t get_display_row(size_t row) const;
	    void push_to_stack(uint16_t addr);

	    void seed(int s) { mt.seed(s); };

        private:
            std::array<uint16_t,16> stack;
	    std::array<uint8_t,memory_size> memory;
	    std::array<uint64_t,32> display;
	    std::array<bool,16> keyboard;

            // VF should never be used (used as flag in some programs
            std::array<uint8_t,16> registers;
            uint16_t I_register; // 12 lowest bits used
            uint8_t sound_register;
            uint8_t delay_register;

            uint16_t program_counter;
            uint8_t stack_pointer;

	    std::random_device rd;
	    std::mt19937 mt;
	    std::uniform_int_distribution<uint8_t> dist;

	    bool waiting;

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
            void render_display();

	    void print_registers();
	    WINDOW* window_ = nullptr;
    };


    // Free functions
    Instruction assemble(std::string_view instruction, const std::unordered_map<std::string,size_t>& label_map={});
    std::string disassemble(Instruction instuction);
    std::string get_name_from_hex(Instruction instruction);
    std::vector<std::string> split(std::string_view instruction);
    std::vector<Instruction> get_numbers(const std::vector<std::string>& tokens, const std::unordered_map<std::string,size_t>& label_map={});

    // Takes tokenized instruction
    std::string get_unique_name(const std::vector<std::string>& instruction);
    bool safe_match(const std::vector<std::string>& instruction, int index, const std::string& target);

    // Ignore optional Vy for now (set default to 0)

    // Lookup table for tokens
    //

    enum class Field { ADDR, X, Y, BYTE, NIBBLE, IGNORE };


    // Lookup table for constant terms

}
