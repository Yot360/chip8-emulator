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

  // Seed RNG
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Chip8::update()
{
	if (m_delay_timer > 0) {
		m_delay_timer--;
	}
	if (m_sound_timer > 0) {
		m_sound_timer--;
	}

	if (m_waiting_for_key) {
		for (int i = 0; i < 16; ++i) {
			if (keys[i]) {
				m_V[m_waiting_register] = i;
				m_waiting_for_key = false;
				m_pc += 2;
				break;
			}
		}
		return; // halt execution until key is pressed
	}

	if (m_pc >= 4095) {
		std::cerr << "Invalid PC: " << std::hex << m_pc << " (too close to memory end)\n";
		exit(1);
	}

	uint16_t opcode = (m_memory[m_pc] << 8) | m_memory[m_pc + 1];
	std::cout << "Executing opcode: " << std::hex << opcode << " at PC: " << m_pc << "\n";
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
			std::cout << std::hex << instruction << std::endl;
			if (instruction == 0x00E0)
				std::memset(m_pixels, 0, sizeof(m_pixels));
			// Returns from subroutine
			else if (instruction == 0x00EE) {
				m_pc = m_stack.top();
				m_stack.pop();
			}
			break;
		// Jump
		case 0x1:
			m_pc = NNN;
			break;
		// Calls subroutine
		case 0x2:
			m_stack.push(m_pc);
			m_pc = NNN;
			break;
		// Skip conditionally
		case 0x3:
			if (m_V[X] == NN)
				m_pc += 2;
			break;
		case 0x4:
			if (m_V[X] != NN)
				m_pc += 2;
			break;
		case 0x5:
			if (m_V[X] == m_V[Y])
				m_pc += 2;
			break;
		// Set register VX
		case 0x6:
			m_V[X] = NN;
			break;
		// Add value to register VX
		case 0x7:
			m_V[X] += NN;
			break;
		// Logical and arithmetic instructions
		case 0x8:
			switch (instruction & 0x000F) {
				// Set
				case 0x0000:
					m_V[X] = m_V[Y];
					break;
				// Binary OR
				case 0x0001:
					m_V[X] = m_V[X] | m_V[Y];
					break;
				// Binary AND
				case 0x0002:
					m_V[X] = m_V[X] & m_V[Y];
					break;
				// Logical XOR
				case 0x0003:
					m_V[X] = m_V[X] ^ m_V[Y];
					break;
				// Add
				case 0x0004:
					m_V[X] = m_V[X] + m_V[Y];
					if (m_V[X] + m_V[Y] > 255)
						m_V[0xF] = 1;
					else
						m_V[0xF] = 0;
					break;
				// Substract
				case 0x0005:
					m_V[0xF] = 1;
					if (m_V[X] < m_V[Y])
						m_V[0xF] = 0;
					m_V[X] = m_V[X] - m_V[Y];
					break;
				// Shift
				case 0x0006:
					m_V[X] = m_V[Y]; // Optional
					if ((m_V[X] & 00000001) == 1)
						m_V[0xF] = 1;
					else
						m_V[0xF] = 0;
					m_V[X] = m_V[X] >> 1;
					break;
				// Substract
				case 0x0007:
					m_V[0xF] = 1;
					if (m_V[Y] < m_V[X])
						m_V[0xF] = 0;
					m_V[X] = m_V[Y] - m_V[X];
					break;
				// Shift
				case 0x000E:
					m_V[X] = m_V[Y]; // Optional
					if ((m_V[X] & 10000000) == 1)
						m_V[0xF] = 1;
					else
						m_V[0xF] = 0;
					m_V[X] = m_V[X] << 1;
					break;
			}
			break;
		// Skip conditionally
		case 0x9:
			if (m_V[X] != m_V[Y])
				m_pc += 2;
			break;
		// Set index register I
		case 0xA:
			m_I = NNN;
			break;
		// Jump with offset
		case 0xB:
			m_pc = NNN + m_V[0];
			break;
		// Random
		case 0xC:
			m_V[X] = (std::rand() % 256) & NN;
			break;
		// Draw on screen
		case 0xD: {
			uint8_t x = m_V[X];
			uint8_t y = m_V[Y];
			m_V[0xF] = 0;
			for (int row = 0; row < N; row++) {
				uint8_t sprite_byte = m_memory[m_I + row];
				for (int col = 0; col < 8; col++) {
					uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 1;
					int x_coordinate = (x+col)%64;
					int y_coordinate = (y+row)%32;
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
		// Skip if key
		case 0xE:
			if ((instruction & 0x000F) == 0x000E) {
				if (keys[m_V[X]])
					m_pc += 2;
			}
			else if ((instruction & 0x000F) == 0x0001) {
				if (!keys[m_V[X]])
					m_pc += 2;
			}
			break;
		// Decode all instructions starting with 0xF
		case 0xF:
			switch (instruction & 0x00FF){
				// Timers
				case 0x07:
				case 0x15:
				case 0x18:
					break;
				// Add to index
				case 0x1E:
					m_I += m_V[X];
					break;
				// Get key
				case 0x0A:
					m_waiting_for_key = true;
					m_waiting_register = X;
					return; // halt CPU cycle here
				// Font character
				case 0x29:
					m_I = 0x50 + (m_V[X] & 0x0F) * 5;
					break;
			}
			break;

	}
}

Chip8::~Chip8()
{

}
