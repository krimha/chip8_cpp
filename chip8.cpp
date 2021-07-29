#include "chip8.h"

#include <unordered_map>
#include <string_view>
#include <string>
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


    Instruction assemble(std::string_view instruction)
    {
        return 0;
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
}
