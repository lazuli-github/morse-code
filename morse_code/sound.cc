#include <iostream>
#include <cmath>

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "sound.hh"

void DotSound::stop()
{
	SDL_PauseAudioDevice(device, 1);
}

void DotSound::play()
{
	SDL_PauseAudioDevice(device, 0);
	SDL_Delay(duration);
	stop();
}

void DotSound::callback(float* stream, int num_samples)
{
	float time{ 0.0f };
	for (int i{ 0 }; i < num_samples; i++, wave_counter++) {
		time = static_cast<float>(wave_counter) / static_cast<float>(obtained_spec.freq);
		float sample = static_cast<float>(volume * std::sin(2.0f * M_PI * wave_freq * time));
		stream[i] = sample;
	}
}

DotSound::DotSound()
{
	desired_spec.freq = 96000;
	desired_spec.format = AUDIO_F32SYS;
	desired_spec.channels = 1;
	desired_spec.samples = 4096;
	desired_spec.userdata = this;
	desired_spec.callback = [](void* userdata, Uint8* stream, int len)
				{
					((DotSound*)userdata)->callback((float*) stream, len / sizeof(float));
				};

	device = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (!device) {
		std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

DotSound::~DotSound()
{
	SDL_CloseAudioDevice(device);
}

void DashSound::play()
{
	SDL_PauseAudioDevice(device, 0);
	SDL_Delay(duration);
	stop();
}

void play_morse_code(std::string& morse_code)
{
	DotSound dot_sound{};
	DashSound dash_sound{};
	int delay_factor{ 1 };

	auto iter{ morse_code.begin() };
	while (iter != morse_code.end()) {
		char c = *iter;

		switch (c) {
		case '.':
			dot_sound.play();
			delay_factor = 1;
			iter++;
			break;
		case '-':
			dash_sound.play();
			delay_factor = 1;
			iter++;
			break;
		case ' ':
			if (*(iter + 1) == '/') { // Skips one word.
				delay_factor = 7;
				iter += 3;
			} else if (*(iter + 1) == '.' || *(iter + 1) == '-') { // Skips one letter.
				delay_factor = 3;
				iter += 1;
			} else {
				std::cerr << "ERROR: invalid morse code" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			break;
		default:
			std::cerr << "ERROR: invalid morse code" << std::endl;
			std::exit(EXIT_FAILURE);
		}
		SDL_Delay(delay_factor * DOT_DURATION);
	}
}
