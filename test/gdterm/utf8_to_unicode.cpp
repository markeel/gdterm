#include <sstream>
#include <cstring>
#include <iomanip>

extern "C" {
#include "libtmt/tmt.h"
#include "libtmt/u8mbtowc.h"
}

int
main(int argc, char * argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: utf8_to_unicode <utf8 string>\n");
		exit(1);
	}
	size_t len = strlen(argv[1]);
	size_t consumed = 0;
	const char * pos = argv[1];
	while (consumed < len) {
		tmt_wchar_t result;
		struct utf8_state state;
		memset(&state,0,sizeof(state));
		int size = utf8_to_wc(&result, pos, (len-consumed), &state);
		if (size < 0) {
			fprintf(stderr, "decoding error at byte starting at: %zu", consumed);
			exit(1);
		}
		char buf[5];
		memcpy(buf, pos, size);
		buf[size] = '\0';
		std::stringstream s;
		s << "U+" << std::setw(6) << std::hex << result;
		printf("%s: %s\n", s.str().c_str(), buf);
		consumed += size;
		pos += size;
	}
}
