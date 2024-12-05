
#include "poller.h"
#include "pty_proxy.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <pty.h>
#include <sys/ioctl.h>

extern "C" {
	#include <libtmt/tmt.h>

	int fill_chars(char * buf, TMTCHAR * chars, int start, int end) {
		char * pos = buf;
		int total = 0;
		for (int i=start; i<end; i++) {
			mbstate_t state;
			memset(&state, 0, sizeof(mbstate_t));
            int len = wcrtomb(pos, chars[i].c, &state);
		    if (len > 0) {
			   pos += len;
			   total += len;
		   }
		}
		return total;
	}

	LineTag get_fg_color_tag(tmt_color_t color) {
		switch (color) {
			case TMT_COLOR_BLACK:
				return LineTag::FG_COLOR_BLACK;
			case TMT_COLOR_RED:
				return LineTag::FG_COLOR_RED;
			case TMT_COLOR_GREEN:
				return LineTag::FG_COLOR_GREEN;
			case TMT_COLOR_YELLOW:
				return LineTag::FG_COLOR_YELLOW;
			case TMT_COLOR_BLUE:
				return LineTag::FG_COLOR_BLUE;
			case TMT_COLOR_MAGENTA:
				return LineTag::FG_COLOR_MAGENTA;
			case TMT_COLOR_CYAN:
				return LineTag::FG_COLOR_CYAN;
			case TMT_COLOR_WHITE:
				return LineTag::FG_COLOR_WHITE;
			case TMT_COLOR_DEFAULT:
			case TMT_COLOR_MAX:
			  break;
		}
		return LineTag::UNDEFINED;
	}

	LineTag get_bg_color_tag(tmt_color_t color) {
		switch (color) {
			case TMT_COLOR_BLACK:
				return LineTag::BG_COLOR_BLACK;
			case TMT_COLOR_RED:
				return LineTag::BG_COLOR_RED;
			case TMT_COLOR_GREEN:
				return LineTag::BG_COLOR_GREEN;
			case TMT_COLOR_YELLOW:
				return LineTag::BG_COLOR_YELLOW;
			case TMT_COLOR_BLUE:
				return LineTag::BG_COLOR_BLUE;
			case TMT_COLOR_MAGENTA:
				return LineTag::BG_COLOR_MAGENTA;
			case TMT_COLOR_CYAN:
				return LineTag::BG_COLOR_CYAN;
			case TMT_COLOR_WHITE:
				return LineTag::BG_COLOR_WHITE;
			case TMT_COLOR_DEFAULT:
			case TMT_COLOR_MAX:
			  break;
		}
		return LineTag::UNDEFINED;
	}

	int add_glyph_chars(int char_start_idx, int cidx, TMTLINE * line, PtyProxy * proxy) {
		int maxchar = MB_CUR_MAX * (cidx - char_start_idx);
		char buffer[maxchar];
		int numchars = fill_chars(buffer, line->chars, char_start_idx, cidx);
		if (numchars > 0) {
			proxy->screen_add_glyph(buffer, numchars);
		}
		return cidx;
	}

	void handle_screen(PtyProxy * proxy, TMTSCREEN * screen) {
		bool bold = false;
		bool dim = false;
		bool underline = false;
		bool blink = false;
		bool reverse = false;
		bool invisible = false;
		for (int lidx=0; lidx<screen->nline; lidx++) {
			tmt_color_t fg = TMT_COLOR_DEFAULT;
			tmt_color_t bg = TMT_COLOR_DEFAULT;
			TMTLINE * line = screen->lines[lidx];
			if (line->dirty) {
				proxy->screen_set_row(lidx);
				int char_start_idx = 0;
				for (int cidx=0; cidx<screen->ncol; cidx++) {
					char_start_idx = add_glyph_chars(char_start_idx, cidx, line, proxy);
					if (line->chars[cidx].a.bold != bold) {
						bold = line->chars[cidx].a.bold;
						if (bold) { proxy->screen_add_tag(LineTag::BOLD); } else { proxy->screen_remove_tag(LineTag::BOLD); }
					}
					if (line->chars[cidx].a.dim != dim) {
						dim = line->chars[cidx].a.dim;
						if (dim) { proxy->screen_add_tag(LineTag::DIM); } else { proxy->screen_remove_tag(LineTag::DIM); }
					}
					if (line->chars[cidx].a.underline != underline) {
						underline = line->chars[cidx].a.underline;
						if (underline) { proxy->screen_add_tag(LineTag::UNDERLINE); } else { proxy->screen_remove_tag(LineTag::UNDERLINE); }
					}
					if (line->chars[cidx].a.blink != blink) {
						blink = line->chars[cidx].a.blink;
						if (blink) { proxy->screen_add_tag(LineTag::BLINK); } else { proxy->screen_remove_tag(LineTag::BLINK); }
					}
					if (line->chars[cidx].a.reverse != reverse) {
						reverse = line->chars[cidx].a.reverse;
						if (reverse) { proxy->screen_add_tag(LineTag::REVERSE); } else { proxy->screen_remove_tag(LineTag::REVERSE); }
					}
					if (line->chars[cidx].a.invisible != invisible) {
						invisible = line->chars[cidx].a.invisible;
						if (invisible) { proxy->screen_add_tag(LineTag::INVISIBLE); } else { proxy->screen_remove_tag(LineTag::INVISIBLE); }
					}
					if (line->chars[cidx].a.fg != fg) {
						if (fg >= TMT_COLOR_BLACK) {
							proxy->screen_remove_tag(get_fg_color_tag(fg));
						}
						fg = line->chars[cidx].a.fg;
						if (fg >= TMT_COLOR_BLACK) {
							proxy->screen_add_tag(get_fg_color_tag(fg));
						}
					}
					if (line->chars[cidx].a.bg != bg) {
						if (bg >= TMT_COLOR_BLACK) {
							proxy->screen_remove_tag(get_bg_color_tag(bg));
						}
						bg = line->chars[cidx].a.bg;
						if (bg >= TMT_COLOR_BLACK) {
							proxy->screen_add_tag(get_bg_color_tag(bg));
						}
					}
				}
				char_start_idx = add_glyph_chars(char_start_idx, screen->ncol, line, proxy);
			}
		}
	}

	void tmt_callback(tmt_msg_t m, struct TMT * v, const void *r, void *p) {
		PtyProxy * proxy = (PtyProxy *)p;
		switch (m) {
			case TMT_MSG_MOVED:
			{
				TMTPOINT * point = (TMTPOINT *)r;
				proxy->update_cursor(point->r, point->c);
				break;
			}
			case TMT_MSG_UPDATE:
			{
				proxy->screen_begin();
				TMTSCREEN * screen = (TMTSCREEN *)r;
				handle_screen(proxy, screen);
				tmt_clean(v);
				proxy->screen_done();
				break;
			}
			case TMT_MSG_SCROLL:
			{
				TMTSCREEN * screen = (TMTSCREEN *)r;
				int scroll_size = 0;
				for (int i=0; i<screen->nline; i++) {
					if (!screen->lines[i]->dirty) {
						break;
					}
					scroll_size += 1;
				}
				proxy->scroll_begin(scroll_size);
				handle_screen(proxy, screen);
				tmt_clean_scroll(v);
				proxy->scroll_done();
				break;
			}
			case TMT_MSG_ANSWER:
			{
				const char * msg = (const char *)r;
				proxy->send_string(msg);
				break;
			}
			case TMT_MSG_BELL:
			{
				proxy->play_bell();
				break;
			}
			case TMT_MSG_CURSOR:
			{
				const char * condition = (const char *)r;
				if (strcmp(condition, "t") == 0) {
					proxy->show_cursor(true);
				} else {
					proxy->show_cursor(false);
				}
				break;
			}
        }
	}
}

PtyProxy::PtyProxy(TermRenderer * renderer) {
	_renderer = renderer;
	_num_rows = 24;
	_num_cols = 80;
	_tmt = tmt_open(_num_rows, _num_cols, tmt_callback, this, NULL);
	_init_pty();
}

PtyProxy::~PtyProxy() {
	tmt_close(_tmt);
	if (_poller != nullptr) {
		delete _poller;
		_poller = nullptr;
	}
}

/**
 * This is invoked on the poller thread, so it is safe to manipulate the TMT data
 * structure and the file descriptor for the pseudo terminal 
 */
void
PtyProxy::apply_size() {
	struct winsize size;
	size.ws_col = _num_cols;
	size.ws_row = _num_rows;
	size.ws_xpixel = 0;
	size.ws_ypixel = 0;
	ioctl(_pty_fd, TIOCSWINSZ, &size);
	tmt_resize(_tmt, _num_rows, _num_cols);
	_renderer->resize_complete();
}

void
PtyProxy::handle(unsigned char * data, int data_len) {
	tmt_write(_tmt, (const char *)data, data_len);
}

void
PtyProxy::exited() {
	if (_renderer != nullptr) {
		_renderer->exited();
	}
}

void
PtyProxy::_init_pty() {
	struct winsize wsize;

	wsize.ws_col = _num_cols;
	wsize.ws_row = _num_rows;
	wsize.ws_xpixel = 0;
	wsize.ws_ypixel = 0;

	pid_t pid = forkpty(&_pty_fd, nullptr, nullptr, &wsize); 
	if (!pid) {
		static char termstr[] = "TERM=ansi";
		putenv(termstr);
		execl(std::getenv("SHELL"), std::getenv("SHELL"), "-l", "-i", nullptr);
	}
	fcntl(_pty_fd, F_SETFL, fcntl(_pty_fd, F_GETFL) | O_NONBLOCK);

	_poller = new Poller(_pty_fd, pid, this);
	_poller->start();
}

int
PtyProxy::send_string(const char * data) {
	return _poller->write_cmd(Poller::CMD_SEND, (const unsigned char *)data, strlen(data));
}

int
PtyProxy::available_to_send() {
	if (_poller != nullptr) {
		return _poller->available_to_send();
	}
	return 0;
}

void
PtyProxy::resize_screen(int nrows, int ncols) {
	_num_rows = nrows;
	_num_cols = ncols;
	if (_poller != nullptr) {
		_poller->write_cmd(Poller::CMD_RESIZE, nullptr, 0);
	}
}

