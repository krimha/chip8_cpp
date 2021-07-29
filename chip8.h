#pragma once

#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>


namespace Chip8 {

    using Instruction = uint16_t;

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


    // Free functions
    Instruction assemble(std::string_view instruction);
    std::vector<std::string> split(std::string_view instruction);

    // Ignore optional Vy for now (set default to 0)

    // Lookup table for tokens
    //

    enum class Field { ADDR, X, Y, BYTE, NIBBLE };


    static const std::unordered_map<const char*, std::vector<Field>> fields {
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
        ,{"JPV0addr"  , { Field::ADDR }}
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


    // Lookup table for constant terms
    static const std::unordered_map<const char*, uint16_t> base {
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

}
