#pragma once

#include <array>
#include <SDL2/SDL.h>

namespace Chip8 {
    class Chip8State {

        private:
            // VF should never be used (used as flag in some programs
            std::array<uint8_t,16> registers;
            uint16_t I_register; // 12 lowest bits used
            uint8_t sound_register;
            uint8_t delay_register;

            uint16_t program_counter;
            uint8_t stack_pointer;

            std::array<uint16_t,16> stack;
    };


    class Chip8Runner {

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

            void render_symbol(char symbol);


    };
}
