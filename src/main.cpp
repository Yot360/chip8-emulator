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

	// Timing constants
	const int TARGET_FPS = 60;             // Display refresh rate
	const int FRAME_DELAY = 1000 / TARGET_FPS;  // ~16.666ms per frame
	int frame_start, frame_time, timer_accumulator = 0;

	// Emulator related objects, init SDL and emulator variables
	Chip8 chip8(rom_path);
	Display display;

	// Emulator loop
	while (display.is_running) {
		frame_start = SDL_GetTicks();

		// Get inputs
		display.poll_events(chip8.keys);

		// Update timers at 60Hz
		timer_accumulator += FRAME_DELAY;
		if (timer_accumulator >= FRAME_DELAY) {
			display.update_timers(chip8.delay_timer, chip8.sound_timer);
			timer_accumulator -= FRAME_DELAY;
		}

		// Execute multiple instructions, 12 per frame
		for (int i = 0; i < 12; i++) {
			chip8.update();
		}

		display.render(chip8.pixels);

		frame_time = SDL_GetTicks() - frame_start;
		if (frame_time > FRAME_DELAY) {
			SDL_Delay(FRAME_DELAY - frame_time);
		}
	}

	return 0;
}
