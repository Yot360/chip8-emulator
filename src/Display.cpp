#include "Display.h"

Display::Display() : m_window(nullptr), m_renderer(nullptr), m_screen_surface(nullptr), m_last_timer_update(0), is_running(true)
{
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer("Chip-8 Emulator", 800, 400, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

	m_screen_surface = SDL_GetWindowSurface(m_window);
}

void Display::update_timers(uint8_t& delay_timer, uint8_t& sound_timer)
{
	uint32_t now = SDL_GetTicks(); // milliseconds since SDL init
	if (now - m_last_timer_update >= 1000 / 60) { // ~16ms
		if (delay_timer > 0)
			delay_timer--;

		if (sound_timer > 0) {
			sound_timer--;
			if (sound_timer == 0) {
				// Stop sound here
			}
		}

		m_last_timer_update = now;
	}
}

void Display::poll_events(bool* keys)
{
	while (SDL_PollEvent(&m_event) != 0) {
		if (m_event.type == SDL_EVENT_QUIT) {
			is_running = false;
		}
		else if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP) {
			bool is_down = (m_event.type == SDL_EVENT_KEY_DOWN);
			switch (m_event.key.scancode) {
				case SDL_SCANCODE_1:
					keys[0x1] = is_down;
					break;
				case SDL_SCANCODE_2:
					keys[0x2] = is_down;
					break;
				case SDL_SCANCODE_3:
					keys[0x3] = is_down;
					break;
				case SDL_SCANCODE_4:
					keys[0xC] = is_down;
					break;
				case SDL_SCANCODE_Q:
					keys[0x4] = is_down;
					break;
				case SDL_SCANCODE_W:
					keys[0x5] = is_down;
					break;
				case SDL_SCANCODE_E:
					keys[0x6] = is_down;
					break;
				case SDL_SCANCODE_R:
					keys[0xD] = is_down;
					break;
				case SDL_SCANCODE_A:
					keys[0x7] = is_down;
					break;
				case SDL_SCANCODE_S:
					keys[0x8] = is_down;
					break;
				case SDL_SCANCODE_D:
					keys[0x9] = is_down;
					break;
				case SDL_SCANCODE_F:
					keys[0xE] = is_down;
					break;
				case SDL_SCANCODE_Z:
					keys[0xA] = is_down;
					break;
				case SDL_SCANCODE_X:
					keys[0x0] = is_down;
					break;
				case SDL_SCANCODE_C:
					keys[0xB] = is_down;
					break;
				case SDL_SCANCODE_V:
					keys[0xF] = is_down;
					break;
			}
		}
	}
}

void Display::render(uint8_t* buffer)
{
	SDL_GetWindowSize(m_window, &window_width, &window_height);
	// Determine the scale factor and the destination offset to keep aspect ratio
	float scale = std::min((float)window_width / CHIP8_WIDTH, (float)window_height / CHIP8_HEIGHT);
	int render_width = (int)(CHIP8_WIDTH * scale);
	int render_height = (int)(CHIP8_HEIGHT * scale);
	int offset_x = (window_width - render_width) / 2;
	int offset_y = (window_height - render_height) / 2;

	SDL_RenderClear(m_renderer);

	// Draw black background
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	// Set pixel color (e.g. white for on-pixels)
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

	// Draw each pixel from CHIP-8 buffer
	for (int y = 0; y < CHIP8_HEIGHT; ++y)
	{
		for (int x = 0; x < CHIP8_WIDTH; ++x)
		{
			int pixel_index = y * CHIP8_WIDTH + x;
			if (buffer[pixel_index]) {
				SDL_FRect pixel_rect = {
					offset_x + x * scale,
					offset_y + y * scale,
					scale,
					scale
				};
				SDL_RenderFillRect(m_renderer, &pixel_rect);
			}
		}
	}

	SDL_RenderPresent(m_renderer);
}

Display::~Display()
{

}
