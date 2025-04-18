#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <algorithm>

class Display
{
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Surface* m_screen_surface;
	SDL_Event m_event;

	int window_width, window_height;
	static constexpr int CHIP8_WIDTH = 64;
	static constexpr int CHIP8_HEIGHT = 32;

	uint32_t m_last_timer_update;

public:
	Display();
	virtual ~Display();

	bool is_running;

	bool init();
	void update_timers(uint8_t& delay_timer, uint8_t& sound_timer);
	void poll_events(bool* keys, bool* keys_down, bool waiting_for_key);
	void render(uint8_t* buffer);
	void loop();
};

#endif /* DISPLAY_H */
