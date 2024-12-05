#ifndef PTY_PROXY_H
#define PTY_PROXY_H

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

class PtyProxy {
public:
	PtyProxy();
	virtual ~PtyProxy();

	void set_renderer(TermRenderer * renderer) {
		_renderer = renderer;
	}

	virtual int send_string(const char * data) = 0;
	virtual int available_to_send() = 0;
	virtual void resize_screen(int nrows, int ncols) = 0;

	// Callback methods
	void _screen_begin();
	void _screen_set_row(int idx);
	void _screen_add_glyph(char * buffer, int numchars);
	void _screen_add_tag(LineTag tag);
	void _screen_remove_tag(LineTag tag);
	void _screen_done();
	void _scroll_begin(int size);
	void _scroll_done();
	void _update_cursor(int row, int col);
	void _play_bell();
	void _show_cursor(bool flag);

protected:
	TermRenderer * _renderer;

	struct TMT *   _tmt;
	int _num_rows;
	int _num_cols;

	void _handle_from_pty(unsigned char * buffer, int len);

};

#endif