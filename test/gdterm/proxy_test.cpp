#include "pty_proxy.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>

extern PtyProxy * create_proxy(TermRenderer * renderer);

class TestRenderer : public TermRenderer {
	public:
		TestRenderer() {}

		virtual void update_cursor(int row, int col) {
			fprintf(stdout, "update_cursor(%d, %d)\n", row, col);
		}

		virtual void screen_begin() {
			fprintf(stdout, "screen_begin()\n");
		}

		virtual void screen_set_row(int row) {
			fprintf(stdout, "screen_set_row(%d)\n", row);
		}

		virtual void screen_add_tag(LineTag tag) {
			fprintf(stdout, "screen_add_tag(%d)\n", tag);
		}

		virtual void screen_remove_tag(LineTag tag) {
			fprintf(stdout, "screen_remove_tag(%d)\n", tag);
		}

		virtual void screen_add_glyph(const char * c, int len) {
			char * buffer = (char *)malloc(len+1);
			memcpy(buffer, c, len);
			buffer[len] = 0;
			fprintf(stdout, "screen_add_glyph(%s)\n", buffer);
			free(buffer);
		}

		virtual void screen_done() {
			fprintf(stdout, "screen_done\n");
		}

		virtual void scroll_begin(int size) {
			fprintf(stdout, "scroll_begin(%d)\n", size);
		}

		virtual void scroll_done() {
			fprintf(stdout, "scroll_done()\n");
		}

		virtual void play_bell() {
			fprintf(stdout, "play_bell()\n");
		}

		virtual void show_cursor(bool flag) {
			fprintf(stdout, "show_cursor(%b)\n", flag);
		}

		virtual void resize_complete() {
			fprintf(stdout, "resize_complete()\n");
		}

		virtual void exited() {
			fprintf(stdout, "exited()\n");
		}
};

int 
main(int argc, char * argv[]) {
	TestRenderer renderer;
	PtyProxy * proxy = create_proxy(&renderer);
	proxy->set_renderer(&renderer);
	proxy->send_string("dir\r");
	std::this_thread::sleep_for(std::chrono::seconds(5));
	delete proxy;
}
