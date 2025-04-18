#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <cstring>
#include <array>
#include <stack>
#include <filesystem>
#include <fstream>
#include <vector>
#include <iostream>
#include <ranges>

class Chip8
{
private:
	// Memory: CHIP-8 has direct access to up to 4 kilobytes of RAM
	std::array<uint8_t, 4096> m_memory;

	// A program counter, often called just “PC”, which points at the current instruction in Memory
	int m_pc;

	// One 16-bit index register called “I” which is used to point at locations in Memory
	uint16_t m_I;

	// A stack for 16-bit addresses, which is used to call subroutines/functions and return from them
	std::stack<uint16_t> m_stack;

	// 16 8-bit (one byte) general-purpose variable registers numbered 0 through F hexadecimal, ie. 0 through 15 in decimal, called V0 through VF
	std::array<uint8_t, 16> m_V;

	// Font stored at 050-09F
	static constexpr std::array<uint8_t, 80> FONT = {
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

	static constexpr int SCREEN_WIDTH = 64;
	static constexpr int SCREEN_HEIGHT = 32;

	bool m_waiting_for_key = false;
	uint8_t m_waiting_register = 0;


public:
	Chip8(std::string rom_path);
	virtual ~Chip8();

	void update();

	bool is_waiting_for_key() const { return m_waiting_for_key; }

	bool keys[16];
	bool keys_down[16];

	// Display: 64 x 32 pixels (or 128 x 64 for SUPER-CHIP) monochrome, ie. black or white
	uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

	// An 8-bit delay timer which is decremented at a rate of 60 Hz (60 times per second) until it reaches 0
	uint8_t delay_timer;

	// An 8-bit sound timer which functions like the delay timer, but which also gives off a beeping sound as long as it’s not 0
	uint8_t sound_timer;
};

#endif /* CHIP8_H */
