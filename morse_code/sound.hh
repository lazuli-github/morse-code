constexpr int DOT_DURATION{ 64 }; // milliseconds

class DotSound
{
protected:
	const int duration{ DOT_DURATION }; // milliseconds
	SDL_AudioDeviceID device{};

	void stop();

private:
	SDL_AudioSpec desired_spec{}, obtained_spec{};
	unsigned wave_counter{ 0 };
	float wave_freq{ 256.0f }, volume{ 0.1f };

	void callback(float* stream, int num_samples);

public:
	DotSound();
	DotSound::~DotSound();

	void play();
};

class DashSound : public DotSound
{
protected:
	const int duration{ 3 * DOT_DURATION }; // milliseconds

public:
	void play();
};

void play_morse_code(std::string& morse_code);
