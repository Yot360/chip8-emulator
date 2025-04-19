#include "Display.h"

Display::Display() : m_window(nullptr), m_renderer(nullptr), m_screen_surface(nullptr), m_last_timer_update(0), m_is_running(true)
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

void Display::poll_events(bool* keys, bool* keys_down, bool waiting_for_key)
{
	while (SDL_PollEvent(&m_event) != 0) {
		if (m_event.type == SDL_EVENT_QUIT) {
			m_is_running = false;
		}
		else if (m_event.type == SDL_EVENT_KEY_DOWN || m_event.type == SDL_EVENT_KEY_UP) {
			int chip8_key = -1;

			switch (m_event.key.scancode) {
				case SDL_SCANCODE_1:
					chip8_key = 0x1;
					break;
				case SDL_SCANCODE_2:
					chip8_key = 0x2;
					break;
				case SDL_SCANCODE_3:
					chip8_key = 0x3;
					break;
				case SDL_SCANCODE_4:
					chip8_key = 0xC;
					break;
				case SDL_SCANCODE_Q:
					chip8_key = 0x4;
					break;
				case SDL_SCANCODE_W:
					chip8_key = 0x5;
					break;
				case SDL_SCANCODE_E:
					chip8_key = 0x6;
					break;
				case SDL_SCANCODE_R:
					chip8_key = 0xD;
					break;
				case SDL_SCANCODE_A:
					chip8_key = 0x7;
					break;
				case SDL_SCANCODE_S:
					chip8_key = 0x8;
					break;
				case SDL_SCANCODE_D:
					chip8_key = 0x9;
					break;
				case SDL_SCANCODE_F:
					chip8_key = 0xE;
					break;
				case SDL_SCANCODE_Z:
					chip8_key = 0xA;
					break;
				case SDL_SCANCODE_X:
					chip8_key = 0x0;
					break;
				case SDL_SCANCODE_C:
					chip8_key = 0xB;
					break;
				case SDL_SCANCODE_V:
					chip8_key = 0xF;
					break;
			}

			if (chip8_key != -1 && waiting_for_key) {
				if (m_event.type == SDL_EVENT_KEY_DOWN) {
					keys_down[chip8_key] = true;
				}
				else if (m_event.type == SDL_EVENT_KEY_UP) {
					if (keys_down[chip8_key]) {
						keys[chip8_key] = true;  // Register the press only when released
						keys_down[chip8_key] = false;
					}
				}
			}
			else if (chip8_key != -1 && !waiting_for_key) {
				bool is_down = (m_event.type == SDL_EVENT_KEY_DOWN);
				keys[chip8_key] = is_down;
			}

		}
	}
}

void Display::render(uint8_t* buffer)
{
	SDL_GetWindowSize(m_window, &m_window_width, &m_window_height);
	// Determine the scale factor and the destination offset to keep aspect ratio
	float scale = std::min((float)m_window_width / CHIP8_WIDTH, (float)m_window_height / CHIP8_HEIGHT);
	int render_width = (int)(CHIP8_WIDTH * scale);
	int render_height = (int)(CHIP8_HEIGHT * scale);
	int offset_x = (m_window_width - render_width) / 2;
	int offset_y = (m_window_height - render_height) / 2;

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
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}
