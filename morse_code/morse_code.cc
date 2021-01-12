#include <iostream>
#include <map>
#include <sstream>
#include <cctype>
#include <regex>
#include <limits>
#include <cmath>

#define SDL_MAIN_HANDLED
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "morse_code.hh"
#include "sound.hh"

/*
 * Removes leading and trailing white spaces and control characters from str.
 */
std::string trim(const std::string &str)
{
	const std::string whitespace{ " \t\f\v\n\r" };
	const std::string::size_type str_start{ str.find_first_not_of(whitespace) };

	if (str_start == std::string::npos)
		return "";

	const std::string::size_type str_end{ str.find_last_not_of(whitespace) };
	const std::string::size_type str_len{ str_end - str_start + 1 };

	return str.substr(str_start, str_len);
}

/*
 * REQUIREMENT 1: str must not have any leading or trailing white spaces and
 * control characters.
 */
std::string convert_to_morse_code(const std::string &str)
{
	std::string morse_code{};

	if (str == "")
		return "";

	for (char c : str) {
		if (c == ' ')
			morse_code += "/ ";
		if ((c < '0' && c < '9') && ((c < 'A' && c > 'Z') || (c < 'a' && c > 'z')))
			throw "invalid ascii sequence";
		/* std::toupper is undefined if the argument's value is neither representable
		 * as unsigned char nor equal to EOF. To use this function safely with plain
		 * chars (or signed chars), the argument should first be converted to
		 * unsigned char. */
		c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
		morse_code += ascii_to_morse_map[c];
		morse_code += " ";
	}

	return morse_code;
}

/*
 * REQUIREMENT 1: morse_code must not have any leading or trailing white spaces
 * and control characters.
 */
std::string convert_to_ascii(const std::string &morse_code)
{
	char c{};
	std::string morse_char{}, str{};
	constexpr auto max_size{ std::numeric_limits<std::string::size_type>::max() };

	if (morse_code == "")
		return "";

	std::string::size_type char_start{ 0 };
	std::string::size_type char_end{ morse_code.find(' ') };
	while (char_start < morse_code.length()) {
		if (char_end == std::string::npos)
			char_end = morse_code.length();
		std::string::size_type morse_char_len{ char_end - char_start };
		morse_char = morse_code.substr(char_start, morse_char_len);
		auto search{ morse_to_ascii_map.find(morse_char) };
		if (search != morse_to_ascii_map.end())
			str += search->second;
		else
			throw "invalid morse code";

		/* This prevents an integer overflow, because char_end may be equal to the
		 * maxium of the type (i.e the length of the string may be the maximum) */
		if (char_end > max_size - 1)
			char_start = max_size;
		else
			char_start = char_end + 1;
		/* '/' between two morse code characters means the end of the current word
		 * and the beginning of a new word. */
		if (char_start < morse_code.length() && morse_code[char_start] == '/') {
			if (morse_code[char_start + 1] != ' ')
				throw "invalid morse code";
			else if (morse_code[char_start + 2] != '.' && morse_code[char_start + 2] != '-')
				throw "invalid morse code";
			str += ' ';
			if (char_end > max_size - 2)
				char_start = max_size;
			else
				char_start += 2;
		}
		/* If char_start is greater than the length of morse_code, a match is never
		 * found (as defined by the standard). And the loop is going to stop
		 * afterwards, so no error will occur. Likewise, if char_start is equal to
		 * the length, no match will be found because the trailling whitespaces have
		 * already been removed beforehand. */
		char_end = morse_code.find(' ', char_start);
	}

	return str;
}

char *get_opt(int argc, char **argv, const std::string &option)
{
	char **begin = argv, **end = argv + argc;
	char **itr = std::find(begin, end, option);

	if (itr != end && ++itr != end)
		return *itr;
	return nullptr;
}

bool cmd_opt_exists(int argc, char **argv, const std::string &option)
{
	char **begin = argv, **end = argv + argc;

	return std::find(begin, end, option) != end;
}

int main(int argc, char **argv)
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	std::string ascii{};
	std::string morse_code{};

	if (argc < 2) {
		std::cerr << "ERROR: no arguments provided." << std::endl;
		return EXIT_FAILURE;
	}

	if (cmd_opt_exists(argc, argv, "-m")) {
		morse_code = get_opt(argc, argv, "-m");
		morse_code = trim(morse_code);
		ascii = convert_to_ascii(morse_code);
		try {
			std::cout << ascii << std::endl;
		} catch (char *msg) {
			std::cerr << "ERROR: " << msg << std::endl;
			return EXIT_FAILURE;
		}
		if (cmd_opt_exists(argc, argv, "-p"))
			play_morse_code(morse_code);
	} else if (cmd_opt_exists(argc, argv, "-a")) {
		ascii = get_opt(argc, argv, "-a");
		ascii = trim(ascii);
		try {
			morse_code = convert_to_morse_code(ascii);
		} catch (char *msg) {
			std::cerr << "ERROR: " << msg << std::endl;
			return EXIT_FAILURE;
		}
		std::cout << morse_code << std::endl;
		if (cmd_opt_exists(argc, argv, "-p"))
			play_morse_code(morse_code);
	} else if (cmd_opt_exists(argc, argv, "-p")) {
		ascii = get_opt(argc, argv, "-p");
		play_morse_code(ascii);
	}
	else {
		std::cout << "ERROR: invalid arguments" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
