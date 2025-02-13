
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>

int 
main(int argc, char * argv[]) {

	if (argc != 2) {
		std::cerr << "usage: gen_unicode_mark <path to UnicodeData.txt>" << std::endl;
		exit(1);
	}

	std::ifstream fs = std::ifstream(argv[1]);
	std::string line;
	std::cout << "struct unicode_mark_types[] = {" << std::endl;
	while (std::getline(fs, line)) {
		int pos = 0;
		int lpos = 0;
		int col_num = 0;
		std::string code;
		std::string name;
		std::string combining_class;
		while ((pos = line.find(';', pos)) != std::string::npos) {
			switch (col_num) {
				case 0:
					code = line.substr(lpos, pos-lpos);
					break;
				case 1:
					name = line.substr(lpos, pos-lpos);
					break;
				case 2:
					combining_class = line.substr(lpos, pos-lpos);
					break;
				default:
					break;
			}
			col_num += 1;
			if (col_num > 2) {
				break;
			}
			lpos = pos+1;
			pos += 1;
		}
		if (combining_class == "Cf") {
			std::cout << "    { 0x" << code << ", TMT_FORMAT }, // " << name << std::endl;
		}
		if (combining_class == "Mn") {
			std::cout << "    { 0x" << code << ", TMT_MARK }, // " << name << std::endl;
		}
		if (combining_class == "Mc") {
			std::cout << "    { 0x" << code << ", TMT_MARK_FULLWIDTH }, //" << name << std::endl;
		}
		if (combining_class == "Me") {
			std::cout << "    { 0x" << code << ", TMT_MARK }, //" << name << std::endl;
		}
	}
	std::cout << "};" << std::endl;
}
