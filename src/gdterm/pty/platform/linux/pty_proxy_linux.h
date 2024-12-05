#ifndef PTY_PROXY_H
#define PTY_PROXY_H

#include "poller.h"

enum LineTag {
	UNDEFINED,
	BOLD,
	DIM,
	UNDERLINE,
	BLINK,
	REVERSE,
	INVISIBLE,
	FG_COLOR_BLACK,
	FG_COLOR_RED,
	FG_COLOR_GREEN,
	FG_COLOR_YELLOW,
	FG_COLOR_BLUE,
	FG_COLOR_MAGENTA,
	FG_COLOR_CYAN,
	FG_COLOR_WHITE,
	BG_COLOR_BLACK,
	BG_COLOR_RED,
	BG_COLOR_GREEN,
	BG_COLOR_YELLOW,
	BG_COLOR_BLUE,
	BG_COLOR_MAGENTA,
	BG_COLOR_CYAN,
	BG_COLOR_WHITE,
};

class TermRenderer {
	public:
		virtual void update_cursor(int row, int col) = 0;
		virtual void screen_begin() = 0;
		virtual void screen_set_row(int row) = 0;
		virtual void screen_add_tag(LineTag tag) = 0;
		virtual void screen_remove_tag(LineTag tag) = 0;
		virtual void screen_add_glyph(const char * c, int len) = 0;
		virtual void screen_done() = 0;
		virtual void scroll_begin(int size) = 0;
		virtual void scroll_done() = 0;
		virtual void play_bell() = 0;
		virtual void show_cursor(bool flag) = 0;
		virtual void resize_complete() = 0;
		virtual void exited() = 0;
};

class PtyProxy : public PollHandler {
public:
	PtyProxy(TermRenderer * renderer);
	virtual ~PtyProxy();

	int send_string(const char * data);
	int available_to_send();
	void resize_screen(int nrows, int ncols);

	virtual void handle(unsigned char * data, int data_len) override;
	virtual void apply_size() override;
	virtual void exited() override;

	void update_cursor(int row, int col) {
		_renderer->update_cursor(row, col);
	}

	void screen_begin() {
		_renderer->screen_begin();
	}

	void screen_done() {
		_renderer->screen_done();
	}

	void screen_set_row(int row) {
		_renderer->screen_set_row(row);
	}

	void screen_add_tag(LineTag tag) {
		_renderer->screen_add_tag(tag);
	}

	void screen_remove_tag(LineTag tag) {
		_renderer->screen_remove_tag(tag);
	}

	void screen_add_glyph(char * buf, int len) {
		_renderer->screen_add_glyph(buf, len);
	}

	void play_bell() {
		_renderer->play_bell();
	}

	void show_cursor(bool flag) {
		_renderer->show_cursor(flag);
	}

	void resize_complete() {
		_renderer->resize_complete();
	}

	void scroll_begin(int size) {
		_renderer->scroll_begin(size);
	}

	void scroll_done() {
		_renderer->scroll_done();
	}

private:
	TermRenderer * _renderer;
	Poller * _poller;
	int _pty_fd;

	struct TMT * _tmt;
	int _num_rows;
	int _num_cols;

	void _init_pty();
};

#endif
