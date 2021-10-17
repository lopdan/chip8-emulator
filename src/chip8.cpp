#include "../include/chip8.hpp"
#include <cstdint>
#include <fstream>

const unsigned int START_ADDRESS = 0x200;

Chip8::Chip8()
{
	// Initialize PC
	program_counter = START_ADDRESS;
}


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