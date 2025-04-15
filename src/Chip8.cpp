#include "Chip8.h"

Chip8::Chip8(std::string rom_path) :
	m_memory{}, m_pixels{}, m_pc(512)
{
	// Copy font to memory
	std::copy(FONT.begin(), FONT.end(), m_memory.begin() + 80);

	// Read ROM
	std::ifstream stream(rom_path, std::ios::binary);
	const std::size_t rom_size = std::filesystem::file_size(rom_path);
	std::vector<uint8_t> buffer(rom_size);
	stream.read(reinterpret_cast<char*>(buffer.data()), rom_size);

	// Copy ROM to memory
	std::copy(buffer.begin(), buffer.end(), m_memory.begin() + 512);
}

void Chip8::update()
{
	if (m_delay_timer > 0) {
		m_delay_timer--;
	}
	if (m_sound_timer > 0) {
		m_sound_timer--;
	}

	// Fetch instructions that PC is currently pointing at from memory.
	uint16_t instruction = (m_memory[m_pc] << 8) | m_memory[m_pc + 1]; // Example : m_memory[m_pc] = 0xAB = 10101011    we move this 8 bits to the left so we have 10101011 00000000    then we can put the m_memory[m_pc+1] to the right to have 10101011 11001101
	if (instruction != 0)
		std::cout << std::hex << instruction << std::endl;
	m_pc += 2;
}

Chip8::~Chip8()
{

}
