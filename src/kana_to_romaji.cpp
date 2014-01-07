#include <iostream>
#include "romaji.h"

int main(int argc, char **argv) {

	if(argc != 2) {
		std::cerr << "Usage: kana_to_romaji <kana>" << std::endl;
		return 1;
	}

	const char *kana = argv[1];
	std::string romaji;
	japanese::utf8_kana_to_romaji(kana, romaji);
	std::cout << romaji << std::endl;

	return 0;
}

