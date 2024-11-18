
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
extern "C" {
#include <libtmt/tmt.h>
}
#include <thread>
#include <pty.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>

#include "poller.h"
#include "pty_proxy.h"

#include <vector>

class TestRenderer : public TermRenderer {

public:
	virtual void update_cursor(int row, int col) {
		printf("cursor moved to: [%d,%d]\n", row, col);
	}

	virtual void screen_begin() {
		printf("screen_begin()\n");
	}

	virtual void screen_set_row(int row) {
		printf("screen_set_row(%d)\n", row);
	}

	virtual void screen_add_tag(LineTag tag) {
		printf("screen_add_tag(%d)\n", tag);
	}

	virtual void screen_remove_tag(LineTag tag) {
		printf("screen_remove_tag(%d)\n", tag);
	}

	virtual void screen_add_glyph(const char * c, int len) {
		char buffer[len+1];
		memcpy(buffer, c, len);
		buffer[len] = '\0';
		printf("screen_add_glyph('%s', %d)\n", buffer, len);
	}

	virtual void screen_done() {
		printf("screen_done()\n");
	}

	virtual void scroll_begin(int size) {
		printf("scroll_begin(%d)\n", size);
	}

	virtual void scroll_done() {
		printf("scroll_done()\n");
	}

	virtual void play_bell() {
		printf("play_bell()\n");
	}

	virtual void show_cursor(bool flag) {
		printf("show_cursor(%b)\n", flag);
	}

	virtual void resize_complete() {
		printf("resize_complete\n");
	}

	virtual void exited() {
		printf("exited\n");
	}

};

const char * line[] = {
	"A\r",
	"B\r",
	"C\r",
	"D\r",
	"E\r",
	"F\r",
	"G\r",
	"H\r",
	"I\r",
	"J\r",
	"K\r",
	"L\r",
	"M\r",
	"N\r",
	"O\r",
	"P\r",
	"Q\r",
	"R\r",
	"S\r",
	"T\r",
	"U\r",
	"V\r",
	"W\r",
	"X\r",
	"Y\r",
	"Z\r",
};

int
main(int argc, char **argv) {
	TestRenderer renderer;
	PtyProxy proxy(&renderer);

	proxy.send_string("ls\r");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	/*
	const char * vi = "vi test/example.dat\r";
	proxy.send_string(vi);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	proxy.send_string("i");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	for (int i=0; i<26; i++) {
		proxy.send_string(line[i]);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	proxy.send_string("\033");
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	proxy.send_string(":wq\r");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	proxy.send_string("ls\r");
	std::this_thread::sleep_for(std::chrono::milliseconds(600));
	*/
}
