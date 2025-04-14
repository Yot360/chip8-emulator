#include "Display.h"

bool Display::init()
{
	// Init SDL
	SDL_Init(SDL_INIT_VIDEO);

	SDL_CreateWindowAndRenderer("Chip-8 Emulator", 800, 400, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

	m_screen_surface = SDL_GetWindowSurface(m_window);

	std::cout << "window created" << std::endl;

	return true;
}

void Display::poll_events(bool* keys)
{
	while (SDL_PollEvent(&m_event)) {
		if (m_event.type == SDL_EVENT_QUIT) {
			is_running = false;
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
				SDL_RenderRect(m_renderer, &pixel_rect);
			}
		}
	}

	SDL_RenderPresent(m_renderer);
}
