
#include "pty_proxy.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
	#include <libtmt/tmt.h>

	int fill_chars(char * buf, TMTCHAR * chars, int start, int end) {
		char * pos = buf;
		int total = 0;
		for (int i=start; i<end; i++) {
			mbstate_t state;
			size_t len;
			memset(&state, 0, sizeof(mbstate_t));
#ifdef USE_WCRTOMB_S
            wcrtomb_s(&len, pos, end-total, chars[i].c, &state);
#else
            len = wcrtomb(pos, chars[i].c, &state);
#endif
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
		char *buffer = (char *)malloc(maxchar);
		int numchars = fill_chars(buffer, line->chars, char_start_idx, cidx);
		if (numchars > 0) {
			proxy->_screen_add_glyph(buffer, numchars);
		}
		free(buffer); 
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
				proxy->_screen_set_row(lidx);
				int char_start_idx = 0;
				for (int cidx=0; cidx<screen->ncol; cidx++) {
					char_start_idx = add_glyph_chars(char_start_idx, cidx, line, proxy);
					if (line->chars[cidx].a.bold != bold) {
						bold = line->chars[cidx].a.bold;
						if (bold) { proxy->_screen_add_tag(LineTag::BOLD); } else { proxy->_screen_remove_tag(LineTag::BOLD); }
					}
					if (line->chars[cidx].a.dim != dim) {
						dim = line->chars[cidx].a.dim;
						if (dim) { proxy->_screen_add_tag(LineTag::DIM); } else { proxy->_screen_remove_tag(LineTag::DIM); }
					}
					if (line->chars[cidx].a.underline != underline) {
						underline = line->chars[cidx].a.underline;
						if (underline) { proxy->_screen_add_tag(LineTag::UNDERLINE); } else { proxy->_screen_remove_tag(LineTag::UNDERLINE); }
					}
					if (line->chars[cidx].a.blink != blink) {
						blink = line->chars[cidx].a.blink;
						if (blink) { proxy->_screen_add_tag(LineTag::BLINK); } else { proxy->_screen_remove_tag(LineTag::BLINK); }
					}
					if (line->chars[cidx].a.reverse != reverse) {
						reverse = line->chars[cidx].a.reverse;
						if (reverse) { proxy->_screen_add_tag(LineTag::REVERSE); } else { proxy->_screen_remove_tag(LineTag::REVERSE); }
					}
					if (line->chars[cidx].a.invisible != invisible) {
						invisible = line->chars[cidx].a.invisible;
						if (invisible) { proxy->_screen_add_tag(LineTag::INVISIBLE); } else { proxy->_screen_remove_tag(LineTag::INVISIBLE); }
					}
					if (line->chars[cidx].a.fg != fg) {
						if (fg >= TMT_COLOR_BLACK) {
							proxy->_screen_remove_tag(get_fg_color_tag(fg));
						}
						fg = line->chars[cidx].a.fg;
						if (fg >= TMT_COLOR_BLACK) {
							proxy->_screen_add_tag(get_fg_color_tag(fg));
						}
					}
					if (line->chars[cidx].a.bg != bg) {
						if (bg >= TMT_COLOR_BLACK) {
							proxy->_screen_remove_tag(get_bg_color_tag(bg));
						}
						bg = line->chars[cidx].a.bg;
						if (bg >= TMT_COLOR_BLACK) {
							proxy->_screen_add_tag(get_bg_color_tag(bg));
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
				proxy->_update_cursor(point->r, point->c);
				break;
			}
			case TMT_MSG_UPDATE:
			{
				proxy->_screen_begin();
				TMTSCREEN * screen = (TMTSCREEN *)r;
				handle_screen(proxy, screen);
				tmt_clean(v);
				proxy->_screen_done();
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
				proxy->_scroll_begin(scroll_size);
				handle_screen(proxy, screen);
				tmt_clean_scroll(v);
				proxy->_scroll_done();
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
				proxy->_play_bell();
				break;
			}
			case TMT_MSG_CURSOR:
			{
				const char * condition = (const char *)r;
				if (strcmp(condition, "t") == 0) {
					proxy->_show_cursor(true);
				} else {
					proxy->_show_cursor(false);
				}
				break;
			}
        }
	}
}

PtyProxy::PtyProxy() {
	_renderer = nullptr;
	_num_rows = 24;
	_num_cols = 80;
	_tmt = tmt_open(_num_rows, _num_cols, tmt_callback, this, NULL);
}

PtyProxy::~PtyProxy() {
	tmt_close(_tmt);
}

void
PtyProxy::_handle_from_pty(unsigned char * data, int data_len) {
	if (_renderer != nullptr) {
		_renderer->log_vt_handler_input(data, data_len);
	}
	tmt_write(_tmt, (const char *)data, data_len);
}

void
PtyProxy::_handle_pty_exited() {
	if (_renderer != nullptr) {
		_renderer->exited();
	}
}

void
PtyProxy::_apply_resize() {
	tmt_resize(_tmt, _num_rows, _num_cols);
	_renderer->resize_complete();
}

void
PtyProxy::_screen_begin() {
	if (_renderer != nullptr) {
		_renderer->screen_begin();
	}
}

void
PtyProxy::_screen_set_row(int row) {
	if (_renderer != nullptr) {
		_renderer->screen_set_row(row);
	}
}

void 
PtyProxy::_screen_add_glyph(char * buf, int len) {
	if (_renderer != nullptr) {
		_renderer->screen_add_glyph(buf, len);
	}
}

void 
PtyProxy::_screen_add_tag(LineTag tag) {
	if (_renderer != nullptr) {
		_renderer->screen_add_tag(tag);
	}
}

void 
PtyProxy::_screen_remove_tag(LineTag tag) {
	if (_renderer != nullptr) {
		_renderer->screen_remove_tag(tag);
	}
}

void
PtyProxy::_screen_done() {
	if (_renderer != nullptr) {
		_renderer->screen_done();
	}
}

void
PtyProxy::_update_cursor(int row, int col) {
	if (_renderer != nullptr) {
		_renderer->update_cursor(row, col);
	}
}

void 
PtyProxy::_play_bell() {
	if (_renderer != nullptr) {
		_renderer->play_bell();
	}
}

void 
PtyProxy::_show_cursor(bool flag) {
	if (_renderer != nullptr) {
		_renderer->show_cursor(flag);
	}
}

void
PtyProxy::_scroll_begin(int size) {
	if (_renderer != nullptr) {
		_renderer->scroll_begin(size);
	}
}

void
PtyProxy::_scroll_done() {
	if (_renderer != nullptr) {
		_renderer->scroll_done();
	}
}
