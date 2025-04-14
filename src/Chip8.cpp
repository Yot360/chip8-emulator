#include "Chip8.h"

Chip8::Chip8() :
	m_memory{}, m_pixels{}
{
	// Copy font to memory
	std::copy(FONT.begin(), FONT.end(), m_memory.begin() + 80);
}

void Chip8::update()
{
	if (m_delay_timer > 0) {
		m_delay_timer--;
	}
	if (m_sound_timer > 0) {
		m_sound_timer--;
	}
}

Chip8::~Chip8()
{

}
