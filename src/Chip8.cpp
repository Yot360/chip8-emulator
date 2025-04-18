#include "Chip8.h"

Chip8::Chip8(std::string rom_path) : m_memory{}, pixels{}, m_pc(512), keys{}, delay_timer(0), sound_timer(0) {
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
	if (m_waiting_for_key) {
		for (int i = 0; i < 16; ++i) {
			if (keys[i]) {
				m_V[m_waiting_register] = i;
				m_waiting_for_key = false;
				keys[i] = false;
				break;
			}
		}
		return; // halt execution until key is pressed
	}

	if (m_pc >= 4095) {
		std::cerr << "Invalid PC: " << std::hex << m_pc << " (too close to memory end)\n";
		exit(1);
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
			if (instruction == 0x00E0)
				std::memset(pixels, 0, sizeof(pixels));
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
				case 0x0004:{
					uint16_t sum = m_V[X] + m_V[Y]; // temp
					uint8_t vf = (sum > 0xFF) ? 1 : 0; // Checks if the result overflows 255, if yes VF -> 1
					m_V[X] = sum & 0xFF; // Store low 8 bits in VX
					m_V[0xF] = vf;
					break;
				}
				// Substract
				case 0x0005: {
					uint8_t vf = (m_V[X] >= m_V[Y]) ? 1 : 0; // Checks if VX is >= than VY if not we need to borrow, if yes VF -> 1
					uint8_t sub = m_V[X] - m_V[Y];
					m_V[X] = sub;
					m_V[0xF] = vf;
					break;
				}
				// Shift right
				case 0x0006: {
					uint8_t res = m_V[Y];
					uint8_t vf = res & 0x1;
					m_V[X] = res >> 1;
					m_V[0xF] = vf;
					break;
				}
				// Substract
				case 0x0007: {
					uint8_t vf = (m_V[Y] >= m_V[X]) ? 1 : 0; // Checks if VY is >= than VX if not we need to borrow, if yes VF -> 1
					uint8_t sub = m_V[Y] - m_V[X];
					m_V[X] = sub;
					m_V[0xF] = vf;
					break;
				}
				// Shift left
				case 0x000E: {
					uint8_t res = m_V[Y];
					uint8_t vf = (res >> 7) & 0x1;
					m_V[X] = res << 1; // Optional
					m_V[0xF] = vf;
					break;
				}
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
					if (sprite_pixel && pixels[y_coordinate*64 + x_coordinate]) {
						pixels[y_coordinate*64 + x_coordinate] = 0;
						m_V[0xF] = 1;
					}
					else if (sprite_pixel && !pixels[y_coordinate*64 + x_coordinate]) {
						pixels[y_coordinate*64 + x_coordinate] = 1;
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
			if ((instruction & 0x00FF) == 0x009E) {
				if (keys[m_V[X]])
					m_pc += 2;
			}
			else if ((instruction & 0x00FF) == 0x00A1) {
				if (!keys[m_V[X]])
					m_pc += 2;
			}
			break;
		// Decode all instructions starting with 0xF
		case 0xF:
			switch (instruction & 0x00FF){
				// Timers
				case 0x07:
					m_V[X] = delay_timer;
					break;
				case 0x15:
					delay_timer = m_V[X];
					break;
				case 0x18:
					sound_timer = m_V[X];
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
				// Binary-coded decimal conversion
				case 0x33: {
					uint8_t number = m_V[X];
					m_memory[m_I] = number/100; // Hundreds
					m_memory[m_I+1] = (number/10) % 10; // Tens
					m_memory[m_I+2] = number % 10; // Ones
					break;
				}
				// Store memory
				case 0x55:
					for (int i = 0; i <= X; i++) {
						m_memory[m_I+i] = m_V[i];
					}
					break;
				// Load memory
				case 0x65: {
					uint16_t temp_I = m_I;
					for (int i = 0; i <= X; i++) {
						m_V[i] = m_memory[temp_I+i];
					}
					break;
				}
			}
			break;

	}
}
