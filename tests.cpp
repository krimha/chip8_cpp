#include <iomanip>

#include <catch2/catch.hpp>

#include "chip8.h"

using namespace Chip8;

TEST_CASE("Tokenize instruction", "[tokenize-intr]")
{
    using VS = std::vector<std::string>;
    REQUIRE( split("") == VS{} );
    REQUIRE( split("CLS") == VS{"CLS"});
    REQUIRE( split("RET") == VS{"RET"});
    REQUIRE( split("JP 12") == VS{"JP", "12"});
    REQUIRE( split("SNE V1, 5") == VS{"SNE", "V1", "5"});
    REQUIRE( split("SNE V1, 5      ") == VS{"SNE", "V1", "5"});
}

TEST_CASE("Get numbers from tokens", "[numbers]")
{
    using VS = std::vector<uint16_t>;
    REQUIRE( get_numbers({""}) == VS{} );
    REQUIRE( get_numbers({"CLS"}) == VS{});
    REQUIRE( get_numbers({"RET"}) == VS{});
    REQUIRE( get_numbers({"JP", "12"}) == VS{12});
    REQUIRE( get_numbers({"SNE", "V1", "5"}) == VS{1, 5});
    REQUIRE( get_numbers({"SNE", "V1", "5"}) == VS{1, 5});
}

TEST_CASE("Test safe_match", "[safe_match]")
{
    REQUIRE( safe_match({"JP", "12"}, 0, "JP") );
    REQUIRE( safe_match({"SNE", "V1", "5"}, 1, "V"));
    REQUIRE( !safe_match({"SNE", "V1", "5"}, 1000, "V"));
    REQUIRE( !safe_match({"SNE", "V1", "5"}, 0, "ASD"));
}

TEST_CASE("Test get_unique_name", "[get_unique_name]")
{

    REQUIRE(get_unique_name({"SYS"}) ==  "SYS");
    REQUIRE(get_unique_name({"CLS"}) == "CLS");
    REQUIRE(get_unique_name({"RET"}) ==  "RET");
    REQUIRE(get_unique_name({"JP", "12"}) == "JPaddr");
    REQUIRE(get_unique_name({"CALL"}) == "CALL");
    REQUIRE(get_unique_name({"SE", "V5", "123"}) == "SEVxbyte");
    REQUIRE(get_unique_name({"SNE", "V5", "123"}) == "SNEVxbyte");
    REQUIRE(get_unique_name({"SE", "V6", "V3"}) == "SEVxVy");
    REQUIRE(get_unique_name({"ADD", "V2", "5"}) == "ADDVxbyte");
    REQUIRE(get_unique_name({"OR"}) == "OR");
    REQUIRE(get_unique_name({"AND"}) == "AND");
    REQUIRE(get_unique_name({"XOR"}) == "XOR");
    REQUIRE(get_unique_name({"ADD", "V1", "V2"}) == "ADDVxVy");
    REQUIRE(get_unique_name({"SUB"}) == "SUB");
    REQUIRE(get_unique_name({"SHR"}) == "SHR");
    REQUIRE(get_unique_name({"SUBN"}) == "SUBN");
    REQUIRE(get_unique_name({"SHL"}) == "SHL");
    REQUIRE(get_unique_name({"SNE", "V3", "V2"}) == "SNEVxVy");
    REQUIRE(get_unique_name({"JP", "V0", "12"}) == "JPV0addr");
    REQUIRE(get_unique_name({"RND"}) == "RND");
    REQUIRE(get_unique_name({"DRW"}) == "DRW");
    REQUIRE(get_unique_name({"SKP"}) == "SKP");
    REQUIRE(get_unique_name({"SKNP"}) == "SKNP");
    REQUIRE(get_unique_name({"ADD", "I", "V2"}) == "ADDIVx");

    REQUIRE(get_unique_name({"LD", "V3", "K"}) == "LDVxK");
    REQUIRE(get_unique_name({"LD", "V3", "V7"}) == "LDVxVy");
    REQUIRE(get_unique_name({"LD", "V3", "DT"}) == "LDVxDT");
    REQUIRE(get_unique_name({"LD", "V3", "[I]"}) == "LDVxI");

    REQUIRE(get_unique_name({"LD", "DT", "V3"}) == "LDDTVx");
    REQUIRE(get_unique_name({"LD", "ST", "V3"}) == "LDSTVx");
    REQUIRE(get_unique_name({"LD", "F", "V3"}) == "LDFVx");
    REQUIRE(get_unique_name({"LD", "B", "V3"}) == "LDBVx");

    REQUIRE(get_unique_name({"LD", "V4", "250"}) == "LDVxbyte");
    REQUIRE(get_unique_name({"LD", "I", "456"}) == "LDIaddr");
    REQUIRE(get_unique_name({"LD", "[I]", "V4"}) == "LDIVx");
}

TEST_CASE("Instruction to name", "[instr-name]")
{

    REQUIRE( get_unique_name(split("CLS"               )) == "CLS"       );
    REQUIRE( get_unique_name(split("RET"               )) == "RET"       );
    REQUIRE( get_unique_name(split("SYS addr"          )) == "SYS"       );
    REQUIRE( get_unique_name(split("JP addr"           )) == "JPaddr"    );
    REQUIRE( get_unique_name(split("CALL addr"         )) == "CALL"      );
    REQUIRE( get_unique_name(split("SE Vx, byte"       )) == "SEVxbyte"  );
    REQUIRE( get_unique_name(split("SNE Vx, byte"      )) == "SNEVxbyte" );
    REQUIRE( get_unique_name(split("SE Vx, Vy"         )) == "SEVxVy"    );
    REQUIRE( get_unique_name(split("LD Vx, byte"       )) == "LDVxbyte"  );
    REQUIRE( get_unique_name(split("ADD Vx, byte"      )) == "ADDVxbyte" );
    REQUIRE( get_unique_name(split("LD Vx, Vy"         )) == "LDVxVy"    );
    REQUIRE( get_unique_name(split("OR Vx, Vy"         )) == "OR"        );
    REQUIRE( get_unique_name(split("AND Vx, Vy"        )) == "AND"       );
    REQUIRE( get_unique_name(split("XOR Vx, Vy"        )) == "XOR"       );
    REQUIRE( get_unique_name(split("ADD Vx, Vy"        )) == "ADDVxVy"   );
    REQUIRE( get_unique_name(split("SUB Vx, Vy"        )) == "SUB"       );
    REQUIRE( get_unique_name(split("SHR Vx {, Vy}"     )) == "SHR"       );
    REQUIRE( get_unique_name(split("SUBN Vx, Vy"       )) == "SUBN"      );
    REQUIRE( get_unique_name(split("SHL Vx {, Vy}"     )) == "SHL"       );
    REQUIRE( get_unique_name(split("SNE Vx, Vy"        )) == "SNEVxVy"   );
    REQUIRE( get_unique_name(split("LD I, addr"        )) == "LDIaddr"   );
    REQUIRE( get_unique_name(split("JP V0, addr"       )) == "JPV0addr"  );
    REQUIRE( get_unique_name(split("RND Vx, byte"      )) == "RND"       );
    REQUIRE( get_unique_name(split("DRW Vx, Vy, nibble")) == "DRW"       );
    REQUIRE( get_unique_name(split("SKP Vx"            )) == "SKP"       );
    REQUIRE( get_unique_name(split("SKNP Vx"           )) == "SKNP"      );
    REQUIRE( get_unique_name(split("LD Vx, DT"         )) == "LDVxDT"    );
    REQUIRE( get_unique_name(split("LD Vx, K"          )) == "LDVxK"     );
    REQUIRE( get_unique_name(split("LD DT, Vx"         )) == "LDDTVx"    );
    REQUIRE( get_unique_name(split("LD ST, Vx"         )) == "LDSTVx"    );
    REQUIRE( get_unique_name(split("ADD I, Vx"         )) == "ADDIVx"    );
    REQUIRE( get_unique_name(split("LD F, Vx"          )) == "LDFVx"     );
    REQUIRE( get_unique_name(split("LD B, Vx"          )) == "LDBVx"     );
    REQUIRE( get_unique_name(split("LD [I], Vx"        )) == "LDIVx"     );
    REQUIRE( get_unique_name(split("LD Vx, [I]"        )) == "LDVxI"     );
}

namespace Catch {

    template<>
    struct StringMaker<Chip8::Instruction> {
	static std::string convert(Chip8::Instruction const& value) {
	    std::stringstream stream;
	    stream << std::hex << value;
	    return stream.str();
	}
    };
}

TEST_CASE ("Assemble single instruction", "[asm-instr]")
{
    CHECK_THROWS( assemble(""));
    CHECK( assemble("CLS") == 0x00E0);
    CHECK( assemble("RET") == 0x00EE);
    CHECK( assemble("SYS 291") == 0x0123);
    CHECK( assemble("JP 291") == 0x1123);
    CHECK( assemble("CALL 291") == 0x2123);
    CHECK( assemble("SE V3, 244") == 0x33F4);
    CHECK( assemble("SNE V4, 244") == 0x44F4);
    CHECK( assemble("SE V1, V2") == 0x5120);
    CHECK( assemble("LD V1, 244") == 0x61F4);
    CHECK( assemble("ADD V1, 244") == 0x71F4);
    CHECK( assemble("LD V2, V4") == 0x8240);
    CHECK( assemble("OR V2, V4") == 0x8241);
    CHECK( assemble("AND V2, V4") == 0x8242);
    CHECK( assemble("XOR V2, V4") == 0x8243);
    CHECK( assemble("ADD V2, V4") == 0x8244);
    CHECK( assemble("SUB V2, V4") == 0x8245);
    CHECK( assemble("SHR V3, V4") == 0x8346);
    CHECK( assemble("SUBN V3, V4") == 0x8347);
    CHECK( assemble("SHL V3 , V4") == 0x834E);
    CHECK( assemble("SNE V5, V6") == 0x9560);
    CHECK( assemble("LD I, 291") == 0xA123);
    CHECK( assemble("JP V0, 291") == 0xB123);
    CHECK( assemble("RND V4, 244") == 0xC4F4);
    CHECK( assemble("DRW V3, V4, 10") == 0xD34A);
    CHECK( assemble("SKP V10") == 0xEA9E);
    CHECK( assemble("SKNP V10") == 0xEAA1);
    CHECK( assemble("LD V4, DT") == 0xF407);
    CHECK( assemble("LD V4, K") == 0xF40A);
    CHECK( assemble("LD DT, V4") == 0xF415);
    CHECK( assemble("LD ST, V4") == 0xF418);
    CHECK( assemble("ADD I, V4") == 0xF41E);
    CHECK( assemble("LD F, V4") == 0xF429);
    CHECK( assemble("LD B, V4") == 0xF433);
    CHECK( assemble("LD [I], V4") == 0xF455);
    CHECK( assemble("LD V4, [I]") == 0xF465);
}
