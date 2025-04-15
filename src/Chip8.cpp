#include "Chip8.h"

Chip8::Chip8(std::string rom_path) : m_memory{}, m_pixels{}, m_pc(512) {
  // Copy font to memory
  std::copy(FONT.begin(), FONT.end(), m_memory.begin() + 80);

  // Read ROM
  std::ifstream stream(rom_path, std::ios::binary);
  const std::size_t rom_size = std::filesystem::file_size(rom_path);
  std::vector<uint8_t> buffer(rom_size);
  stream.read(reinterpret_cast<char *>(buffer.data()), rom_size);

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
	// Example :
	// m_memory[m_pc] = 0xAB = 10101011
	// m_memory[m_pc+1] = 0xCD = 11001101
	// we move this 8 bits to the left so we have 10101011 00000000
	// then we can put the m_memory[m_pc+1] to the right to have 10101011 11001101 or 0xABCD
	uint16_t instruction = (m_memory[m_pc] << 8) | m_memory[m_pc + 1];
	m_pc += 2; // Go to next instruction

	// Get the nibbles
	const uint8_t X = (instruction >> 8) & 0x000F;
	const uint8_t Y = (instruction >> 4) & 0x000F;
	const uint8_t N = instruction & 0x000F;
	const uint8_t NN = instruction & 0x00FF;
	const uint16_t NNN = instruction & 0x0FFF;

	// Decode instruction fetched
	switch (instruction >> 12) {
		// Clear screen
		case 0x0:
			std::memset(m_pixels, 0, sizeof(m_pixels));
			break;
		// Jump
		case 0x1:
			m_pc = NNN;
			break;
		// Set register VX
		case 0x6:
			m_V[X] = NN;
			break;
		// Add value to register VX
		case 0x7:
			m_V[X] += NN;
			break;
		// Set index register I
		case 0xA:
			m_I = NNN;
			break;
		// Draw on screen
		case 0xD:
			uint8_t x = m_V[X] % 64;
			uint8_t y = m_V[Y] % 32;
			m_V[0xF] = 0;
			for (int row = 0; row < N; row++) {
				uint8_t sprite_byte = m_memory[m_I + row];
				for (int col = 0; col < 8; col++) {
					uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 1;
					int x_coordinate = x+col;
					int y_coordinate = y+row;
					if (sprite_pixel && m_pixels[y_coordinate*64 + x_coordinate]) {
						m_pixels[y_coordinate*64 + x_coordinate] = 0;
						m_V[0xF] = 1;
					}
					else if (sprite_pixel && !m_pixels[y_coordinate*64 + x_coordinate]) {
						m_pixels[y_coordinate*64 + x_coordinate] = 1;
					}

					if (x_coordinate >= SCREEN_WIDTH) {
						break;
					}
				}
			}
			break;
	}
}

Chip8::~Chip8()
{

}
