#include "../include/chip8.hpp"
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;




Chip8::Chip8():randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
	// Initialize PC
	program_counter = START_ADDRESS;
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

	// Extract the bitfields from the opcode
	unsigned int u 		= (opcode >> 12) & 0xF;
	unsigned int p 		= (opcode >> 0)  & 0xF;
	unsigned int y 		= (opcode >> 4)  & 0xF;
	unsigned int x 		= (opcode >> 8)  & 0xF;
	unsigned int kk 	= (opcode >> 0)  & 0xFF;
	unsigned int nnn 	= (opcode >> 0)  & 0xFFF;

	// Load fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; i++)
	{
		memory[FONTSET_START_ADDRESS + i] = chip8_fontset[i];
	}
}

unsigned char chip8_fontset[FONTSET_SIZE] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



/** Load instructions from ROM file. */
void Chip8::LoadROM(char const* filename)
{
	// Open file as binary and position the cursor at the end of the file.
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of the file (std::ios::ate)
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Position the cursor at the beginning of the file to fill the buffer.
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8 memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}
		// Free the buffer
		delete[] buffer;
	}
}

/** Clear the display (00E0) */
void Chip8::CLS()
{
	memset(video, 0, sizeof(video));
}

/** Return from a subroutine (00EE) */
void Chip8::RET()
{
	stack_pointer--;
	program_counter = stack[stack_pointer];
}

/** Jump to location nnn (1NNN) */
void Chip8::JUMP()
{
	uint16_t address = (opcode>>0) & 0x0FFFu;

	program_counter = address;
}

/** Call subroutine at nnn (2nnn) */
void Chip8::EXE()
{
	uint16_t address = (opcode>>0) & 0x0FFFu;

	++stack_pointer;
	program_counter = stack[stack_pointer];
	program_counter = address;
}

/** Call subroutine at nnn (3xkk) */
void Chip8::SEI()
{
	uint8_t byte = (opcode>>0) & 0x00FFu;
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if(registers[Vx] == byte)
	{
		program_counter += 2;
	}
}

/** Skip next instruction if Vx != kk (4xkk) */
void Chip8::SNEI()
{
	uint8_t byte = (opcode>>0) & 0x00FFu;
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (registers[Vx] != byte)
	{
		program_counter += 2;
	}
}

/** Skip next instruction if Vx = Vy (5xy0) */
void Chip8::SE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] == registers[Vy])
	{
		program_counter += 2;
	}
}


/** Set Vx = kk (6xkk) */
void Chip8::STRI()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = byte;
}

/** Set Vx = Vx + kk (7xkk) */
void Chip8::ADDI()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] += byte;
}

/** Set Vx = Vy (8xy0) 	*/
void Chip8::COPY()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

/** Set Vx = Vx OR Vy (8xy1) */
void Chip8::OR()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] |= registers[Vy];
}

/** Set Vx = Vx AND Vy (8xy2) */
void Chip8::AND()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] &= registers[Vy];
}

/** Set Vx = Vx XOR Vy (8xy3) */
void Chip8::XOR()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] ^= registers[Vy];
}

/** Set Vx = Vx + Vy, set VF = carry (8xy4)*/
void Chip8::ADD()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = registers[Vx] + registers[Vy];

	if (sum > 255U)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0xFFu;
}

/** Set Vx = Vx - Vy, set VF = NOT borrow (8xy5)*/
void Chip8::SUB()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];
}

/** Set Vx = Vx SHR 1 (8xy6)*/
void Chip8::RSH()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

/** Set Vx = Vy - Vx, set VF = NOT borrow (8xy7)*/
void Chip8::SUBR()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];
}

/** Set Vx = Vx SHL 1 (8xyE)*/
void Chip8::LSH()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

/** Skip next instruction if Vx != Vy (9xy0)*/
void Chip8::SNE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy])
	{
		program_counter += 2;
	}
}

/** Set I = nnn (Annn)*/
void Chip8::STR()
{
	uint16_t address = (opcode>>0) & 0x0FFFu;

	index_register = address;
}

/** Jump to location nnn + V0 (Bnnn)*/
void Chip8::BR()
{
	uint16_t address = (opcode>>0) & 0x0FFFu;

	program_counter = registers[0] + address;
}

/** Set Vx = random byte AND kk (Cxkk)*/
void Chip8::RND()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

/** Display n-byte sprite starting at memory location I at (Vx, Vy), 
 *  set VF = collision (Dxyn) 
 * 
 *  The interpreter reads n bytes from memory, starting at the address stored in I. 
 *  These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
 *  Sprites are XORed onto the existing screen. 
 *  If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. 
 *  If the sprite is positioned so part of it is outside the coordinates of the display, 
 *  it wraps around to the opposite side of the screen. 
 * 
*/
void Chip8::DRAW()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint16_t address = index_register;

	uint8_t x = registers[Vx] % VIDEO_WIDTH;
	uint8_t y = registers[Vy] % VIDEO_HEIGHT;
	int height = opcode & 0x000F; //N
	int width = 8;
	registers[0x0F] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index_register + row];
		for (unsigned int col = 0; col < 8; ++col)
		{
			uint32_t* screenPixel = &video[(y + row) * VIDEO_WIDTH + (x + col)];
			// ON
			if (spriteByte & (0x80u >> col))
			{
				// ON & collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}
				// XOR the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}