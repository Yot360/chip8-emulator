#include <filesystem>
#include <iostream>
#include "Chip8.h"
#include "Display.h"

int main(int argc, char *argv[])
{
	// Get Chip-8 ROM
	std::string rom_path = argv[1];
	if (argv[1] == nullptr || !std::filesystem::exists(rom_path)) {
		std::cerr << "Chip-8 ROM not found. Exiting." << std::endl;
		return 1;
	}

	// Framerate related variables
	static constexpr int FPS = 60;
	static constexpr int FPS_DELAY = 1000 / FPS;
	int frame_start, frame_time;

	// Emulator related objects
	Chip8 chip8(rom_path);
	Display display;

	// Init SDL
	if(!display.init()) {
		return 1;
	}

	// Emulator loop
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
