#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <algorithm>

class Display
{
private:
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	SDL_Surface* m_screen_surface = nullptr;
	SDL_Event m_event;

	int window_width, window_height;
	static constexpr int CHIP8_WIDTH = 64;
	static constexpr int CHIP8_HEIGHT = 32;

public:
	Display() {};
	virtual ~Display() {};

	bool is_running = true;

	bool init();
	void poll_events(bool* keys);
	void render(uint8_t* buffer);
	void loop();
};

#endif /* DISPLAY_H */
