#pragma once

#include <cstdint>
#include <random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;

class Chip8{
	public:
		Chip8();
		void LoadROM(char const* file);
		void Cycle();

		uint8_t input_keys[KEY_COUNT]{};
		uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

	private:
		void Table0();
		void Table8();
		void TableE();
		void TableF();

		/** Components from the Chip-8 */
		uint8_t registers[16];
		uint8_t memory[4096];
		uint16_t index_register;
		uint16_t program_counter;
		uint16_t stack[16];
		uint8_t stack_pointer;
		uint8_t delay_timer;
		uint8_t sound_timer;
		uint16_t opcode;

		/** http://devernay.free.fr/hacks/chip8/C8TECH10.HTM */
		void CLS(); 	void RET(); 	void OEXE();
		void JUMP(); 	void EXE(); 	void SEI();
		void SNEI(); 	void SE(); 		void STRI();
		void ADDI(); 	void COPY(); 	void OR();
		void AND(); 	void XOR(); 	void ADD();
		void SUB(); 	void RSH(); 	void SUBR();
		void LSH(); 	void SNE(); 	void STR();
		void BR(); 		void RND(); 	void DRAW();
		void SP(); 		void SNP(); 	void STRD();
		void WAIT(); 	void SETD(); 	void SETS();
		void OFFS(); 	void NUM(); 	void BCD();
		void STRM(); 	void LDM(); 
		
		void XXX(); // Illegal opcode

		//std::default_random_engine randGen;
		//std::uniform_int_distribution<uint8_t> randByte;

		typedef void (Chip8::*Chip8Func)();
		Chip8Func table[0xF + 1]{&Chip8::XXX};
		Chip8Func table0[0xE + 1]{&Chip8::XXX};
		Chip8Func table8[0xE + 1]{&Chip8::XXX};
		Chip8Func tableE[0xE + 1]{&Chip8::XXX};
		Chip8Func tableF[0x65 + 1]{&Chip8::XXX};
};