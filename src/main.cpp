#include <iostream>
#include "Chip8.h"
#include "Display.h"

int main(int argc, char *argv[])
{
	static constexpr int FPS = 60;
	static constexpr int FPS_DELAY = 1000 / FPS;
	int frame_start, frame_time;

	Chip8 chip8;
	Display display;

	// Init SDL
	if(!display.init()) {
		return 1;
	}

	while (display.is_running) {
		frame_start = SDL_GetTicks();

		display.poll_events(chip8.keys);
		chip8.update();
		display.render(chip8.m_pixels);

		frame_time = SDL_GetTicks() - frame_time;
		if (FPS_DELAY > frame_time) {
			SDL_Delay(FPS_DELAY - frame_time);
		}
	}

	return 0;
}
