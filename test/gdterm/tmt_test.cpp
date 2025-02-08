#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
extern "C" {
#include "libtmt/tmt.h"
#include "libtmt/u8mbtowc.h"
#include "libtmt/wide_lookup.h"
}
#include <stdio.h>

int _attempt_count = 0;
int _failure_count = 0;

void test_lookup_for_wc(const char * file, int line, tmt_wchar_t wc, bool expected) {
	_attempt_count += 1;
	bool had = is_wc_unicode_full_width(wc, false);
	if (had != expected) {
		_failure_count += 1;
		const char * had_str = had ? "true" : "false";
		const char * expected_str = expected ? "true" : "false";
		fprintf(stderr, "%s:%d lookup for %x failed: had %s, expected %s\n", file, line, wc, had_str, expected_str);
	}
}

void
test_wide_lookup() {
	test_lookup_for_wc(__FILE__, __LINE__, 0x00000032, false);  // Space is not wide
	test_lookup_for_wc(__FILE__, __LINE__, 0x000000A3, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x000010FF, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x00001100, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x00001132, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000115F, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x00001160, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FE20, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FE35, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FE41, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FE42, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FE43, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FFE6, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0000FFE7, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x00016000, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x00016FE0, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0001AFFC, false);
	test_lookup_for_wc(__FILE__, __LINE__, 0x000323AF, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x000323B0, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0003FFFD, true);
	test_lookup_for_wc(__FILE__, __LINE__, 0x0003FFFE, false);
}

class TmtResult {
	public:
		TmtResult(const char * file, int line) {
			_file = file;
			_line = line;
		}
	
		void add_screen(const char * mode, TMTSCREEN * screen) {
			for (int lidx=0; lidx<screen->nline; lidx++) {
				TMTLINE * line = screen->lines[lidx];
				if (line->dirty) {
					std::stringstream s;
					s << mode << "-line(" << lidx << "):";
					std::string prev;
					int rep=0;
					for (int cidx=0; cidx<screen->ncol; cidx++) {
						std::stringstream cur;
					    wchar_t wc = line->chars[cidx].c;	
						if (wc < 0x20) {
							cur << "'" << std::hex << wc << "'";
						} else  if (wc == ':') {
							cur << "'" << std::hex << wc << "'";
						} else {
							int max_len = 1+4*(1+line->chars[cidx].num_marks);
							char buffer[max_len];
							int len = wc_to_utf8(buffer, max_len, wc);
							std::stringstream hex_buf;
							hex_buf << std::hex << wc;
							char * buf_pos = buffer+len;
							for (int mark_pos=0; mark_pos<line->chars[cidx].num_marks; mark_pos++) {
								size_t mark_len = wc_to_utf8(buf_pos, max_len-len, line->chars[cidx].marks[mark_pos]);
								hex_buf << " " << std::hex << line->chars[cidx].marks[mark_pos];
								buf_pos += mark_len;
								len += mark_len;
							}
							buffer[len] = '\0';
							if (wc > 0x7f) {
								cur << "'" << buffer << "'(" << hex_buf.str() << ")";
							} else {
								cur << "'" << buffer << "'";
							}
						}
						int num_attrs = 0;
						if (line->chars[cidx].char_type == TMT_FULLWIDTH) {
							cur << "[full";
							num_attrs += 1;
						}
						if (line->chars[cidx].char_type == TMT_IGNORED) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "ignore";
							num_attrs += 1;
						}
						if (line->chars[cidx].char_type == TMT_FORMATTER) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "format";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.bold) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "bold";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.dim) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "dim";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.underline) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "underline";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.blink) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "blink";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.reverse) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "reverse";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.invisible) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							cur << "invisible";
							num_attrs += 1;
						}
						if (line->chars[cidx].a.fg.code >= TMT_COLOR_BLACK) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							switch (line->chars[cidx].a.fg.code) {
								case TMT_COLOR_BLACK: 
									cur << "fg=black";
									break;
								case TMT_COLOR_RED: 
									cur << "fg=red";
									break;
								case TMT_COLOR_GREEN: 
									cur << "fg=green";
									break;
								case TMT_COLOR_YELLOW: 
									cur << "fg=yellow";
									break;
								case TMT_COLOR_BLUE: 
									cur << "fg=blue";
									break;
								case TMT_COLOR_MAGENTA: 
									cur << "fg=magenta";
									break;
								case TMT_COLOR_CYAN: 
									cur << "fg=cyan";
									break;
								case TMT_COLOR_WHITE: 
									cur << "fg=white";
									break;
								case TMT_COLOR_BRIGHT_BLACK: 
									cur << "fg=bright-black";
									break;
								case TMT_COLOR_BRIGHT_RED: 
									cur << "fg=bright-red";
									break;
								case TMT_COLOR_BRIGHT_GREEN: 
									cur << "fg=bright-green";
									break;
								case TMT_COLOR_BRIGHT_YELLOW: 
									cur << "fg=bright-yellow";
									break;
								case TMT_COLOR_BRIGHT_BLUE: 
									cur << "fg=bright-blue";
									break;
								case TMT_COLOR_BRIGHT_MAGENTA: 
									cur << "fg=bright-magenta";
									break;
								case TMT_COLOR_BRIGHT_CYAN: 
									cur << "fg=bright-cyan";
									break;
								case TMT_COLOR_BRIGHT_WHITE: 
									cur << "fg=bright-white";
									break;
								case TMT_COLOR_RGB: 
									cur << "fg=";
									cur << std::hex << line->chars[cidx].a.fg.red << ":";
									cur << std::hex << line->chars[cidx].a.fg.green << ":";
									cur << std::hex << line->chars[cidx].a.fg.blue;
									break;
								default:
									cur << "unk";
									break;
							}
						}
						if (line->chars[cidx].a.bg.code >= TMT_COLOR_BLACK) {
							if (num_attrs == 0) { cur << "["; } else { cur << ","; }
							switch (line->chars[cidx].a.bg.code) {
								case TMT_COLOR_BLACK: 
									cur << "bg=black";
									break;
								case TMT_COLOR_RED: 
									cur << "bg=red";
									break;
								case TMT_COLOR_GREEN: 
									cur << "bg=green";
									break;
								case TMT_COLOR_YELLOW: 
									cur << "bg=yellow";
									break;
								case TMT_COLOR_BLUE: 
									cur << "bg=blue";
									break;
								case TMT_COLOR_MAGENTA: 
									cur << "bg=magenta";
									break;
								case TMT_COLOR_CYAN: 
									cur << "bg=cyan";
									break;
								case TMT_COLOR_WHITE: 
									cur << "bg=white";
									break;
								case TMT_COLOR_BRIGHT_BLACK: 
									cur << "bg=bright-black";
									break;
								case TMT_COLOR_BRIGHT_RED: 
									cur << "bg=bright-red";
									break;
								case TMT_COLOR_BRIGHT_GREEN: 
									cur << "bg=bright-green";
									break;
								case TMT_COLOR_BRIGHT_YELLOW: 
									cur << "bg=bright-yellow";
									break;
								case TMT_COLOR_BRIGHT_BLUE: 
									cur << "bg=bright-blue";
									break;
								case TMT_COLOR_BRIGHT_MAGENTA: 
									cur << "bg=bright-magenta";
									break;
								case TMT_COLOR_BRIGHT_CYAN: 
									cur << "bg=bright-cyan";
									break;
								case TMT_COLOR_BRIGHT_WHITE: 
									cur << "bg=bright-white";
									break;
								case TMT_COLOR_RGB: 
									cur << "bg=";
									cur << std::hex << line->chars[cidx].a.fg.red << ":";
									cur << std::hex << line->chars[cidx].a.fg.green << ":";
									cur << std::hex << line->chars[cidx].a.fg.blue;
									break;
								default:
									cur << "unk";
									break;
							}
							num_attrs += 1;
						}
						if (num_attrs > 0) {
							cur << "]";
						}
						std::string cs = cur.str();
						if (cs == prev) {
							rep += 1;
						} else {
							if (prev != "") {
								s << prev;
								if (rep > 0) {
									s << "/" << std::dec << (rep+1);
								}
								rep = 0;
							}
							prev = cs;
						}
					}
					if (rep > 0) {
						s << prev << "/" << std::dec << (rep+1);
					} else {
						s << prev;
					}
					results.push_back(s.str());
				}
			}
		}

		void add_moved(int row, int col) {
			std::stringstream s;
			s << "move(" << row << "/" << col << ")";
			results.push_back(s.str());

		}

		void add_answer(const char * msg) {
			std::stringstream s;
			s << "answer(" << msg << ")";
			results.push_back(s.str());
		}

		void add_bell() {
			std::stringstream s;
			s << "bell()";
			results.push_back(s.str());
		}

		void add_cursor(const char * condition) {
			std::stringstream s;
			s << "cursor(" << condition << ")";
			results.push_back(s.str());
		}

		void clear() {
			results.clear();
		}

		bool check_result_at(int rpos, const std::string & exp) {
			if (results[rpos] == exp) {
				return true;
			}
			fprintf(stderr, "%s:%d result mismatch on result (%d):\n", _file, _line, rpos);
			fprintf(stderr, "   had: %s\n", results[rpos].c_str());
			fprintf(stderr, "wanted: %s\n", exp.c_str());
			_failure_count += 1;
			return false;
		}

		void check_result(const char * expected) {
			std::string e = std::string(expected);
			int exp_pos = 0;
			int res_pos = 0;
			while (true) {
				if (res_pos >= results.size()) {
					fprintf(stderr, "%s:%d comparison failed: expected more results\n", _file, _line);
					_failure_count += 1;
					break;
				}
				int comma_pos = e.find(",", exp_pos);
				if (comma_pos == std::string::npos) {
					std::string exp = e.substr(exp_pos);
					if (!check_result_at(res_pos, exp))
						return;
					res_pos += 1;
					break;
				} 
				std::string exp = e.substr(exp_pos, comma_pos-exp_pos);
				if (!check_result_at(res_pos, exp))
					return;
				exp_pos = comma_pos+1;
				res_pos += 1;
			}
			if (res_pos < results.size()) {
				fprintf(stderr, "%s:%d comparison failed: had unexpected results\n", _file, _line);
				_failure_count += 1;
				for (int i=res_pos; i<results.size(); i++) {
					fprintf(stderr, "--> %s\n", results[i].c_str());
				}

			}

		}
	
	private:
		const char * _file;
		int _line;
		std::vector<std::string> results;
};

extern "C" {
	void test_write_string_cb(tmt_msg_t m, struct TMT * v, const void * r, void * p) {
		struct TmtResult * result = (struct TmtResult *)p;
		
		switch (m) {
           case TMT_MSG_MOVED:
		   {
			   TMTPOINT * point = (TMTPOINT *)r;
			   result->add_moved(point->r, point->c);
			   break;
		   }
           case TMT_MSG_UPDATE:
		   {
			   TMTSCREEN * screen = (TMTSCREEN *)r;
			   result->add_screen("screen", screen);
			   tmt_clean(v);
			   break;
		   }
           case TMT_MSG_ANSWER:
		   {
		       const char * msg = (const char *)r;
			   result->add_answer(msg);
			   break;
		   }
           case TMT_MSG_BELL:
		   {
			   result->add_bell();
			   break;
		   }
           case TMT_MSG_CURSOR:
		   {
			   const char * condition = (const char *)r;
			   result->add_cursor(condition);
			   break;
		   }
           case TMT_MSG_SCROLL:
		   {
			   TMTSCREEN * screen = (TMTSCREEN *)r;
			   result->add_screen("scroll", screen);
			   tmt_clean_scroll(v);
			   break;
		   }
        }
	}
}

void
test_write_string(const char * file, int line, const char * setup, const char * input, const char * expected) {
	_attempt_count += 1;
	TmtResult result(file, line);
	TMT * tmt = tmt_open(24, 80, test_write_string_cb, (void *)&result, NULL);
	if (setup != NULL) {
		tmt_write(tmt, setup, strlen(setup));
		result.clear();
	}
	tmt_write(tmt, input, strlen(input));
	result.check_result(expected);
	tmt_close(tmt);
}

void
test_tmt_write() {
	test_write_string(__FILE__,__LINE__, "\r", "À\r\n",
			"screen-line(0):'À'' '/79,"
			"move(1/0)");
	test_write_string(__FILE__,__LINE__, "\r", "؀21\r\n",
			"screen-line(0):'؀2'(600 32)'1'' '/78,"
			"move(1/0)");
	test_write_string(__FILE__,__LINE__, "\r", "🏌🏼‍♂️\r\n",
			"screen-line(0):'🏌'(1f3cc)'🏼'(1f3fc)[full]' '[ignore]'‍♂️'(200d 2642 fe0f)' '/76,"
			"move(1/0)");
	test_write_string(__FILE__, __LINE__, NULL, "abc😃efg\r\n", 
			"screen-line(0):' '/80,"
			"screen-line(1):' '/80,"
			"screen-line(2):' '/80,"
			"screen-line(3):' '/80,"
			"screen-line(4):' '/80,"
			"screen-line(5):' '/80,"
			"screen-line(6):' '/80,"
			"screen-line(7):' '/80,"
			"screen-line(8):' '/80,"
			"screen-line(9):' '/80,"
			"screen-line(10):' '/80,"
			"screen-line(11):' '/80,"
			"screen-line(12):' '/80,"
			"screen-line(13):' '/80,"
			"screen-line(14):' '/80,"
			"screen-line(15):' '/80,"
			"screen-line(16):' '/80,"
			"screen-line(17):' '/80,"
			"screen-line(18):' '/80,"
			"screen-line(19):' '/80,"
			"screen-line(20):' '/80,"
			"screen-line(21):' '/80,"
			"screen-line(22):' '/80,"
			"screen-line(23):' '/80,"
			"move(0/0),"
			"screen-line(0):'a''b''c''😃'(1f603)[full]' '[ignore]'e''f''g'' '/72,"
			"move(1/0)"
			);
	//                                      1 2 3 4 5 6 7 8 910111213
	test_write_string(__FILE__, __LINE__, 
			"a\r\nb\r\nc\r\nd\r\ne\r\nf\r\ng\r\nh\r\ni\r\nj\r\nk\r\nl\r\nm\r\nn\r\no\r\np\r\nq\r\nr\r\ns\r\nt\r\nu\r\nv\r\nw\r\n", 
		    "                                                                               😃\r\n", 
			"scroll-line(0):'a'' '/79,"
			"scroll-line(0):'b'' '/79,"
			"screen-line(0):'c'' '/79,"
			"screen-line(1):'d'' '/79,"
			"screen-line(2):'e'' '/79,"
			"screen-line(3):'f'' '/79,"
			"screen-line(4):'g'' '/79,"
			"screen-line(5):'h'' '/79,"
			"screen-line(6):'i'' '/79,"
			"screen-line(7):'j'' '/79,"
			"screen-line(8):'k'' '/79,"
			"screen-line(9):'l'' '/79,"
			"screen-line(10):'m'' '/79,"
			"screen-line(11):'n'' '/79,"
			"screen-line(12):'o'' '/79,"
			"screen-line(13):'p'' '/79,"
			"screen-line(14):'q'' '/79,"
			"screen-line(15):'r'' '/79,"
			"screen-line(16):'s'' '/79,"
			"screen-line(17):'t'' '/79,"
			"screen-line(18):'u'' '/79,"
			"screen-line(19):'v'' '/79,"
			"screen-line(20):'w'' '/79,"
			"screen-line(21):' '/80,"
			"screen-line(22):'😃'(1f603)[full]' '[ignore]' '/78,"
			"screen-line(23):' '/80"
			);
	test_write_string(__FILE__, __LINE__, 
			"a\r\nb\r\nc\r\nd\r\ne\r\nf\r\ng\r\nh\r\ni\r\nj\r\nk\r\nl\r\nm\r\nn\r\no\r\np\r\nq\r\nr\r\ns\r\nt\r\nu\r\nv\r\nw\r\n"
			"x😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃😃",
		    "😃\r\n", 
			"scroll-line(0):'a'' '/79,"
			"scroll-line(0):'b'' '/79,"
			"screen-line(0):'c'' '/79,"
			"screen-line(1):'d'' '/79,"
			"screen-line(2):'e'' '/79,"
			"screen-line(3):'f'' '/79,"
			"screen-line(4):'g'' '/79,"
			"screen-line(5):'h'' '/79,"
			"screen-line(6):'i'' '/79,"
			"screen-line(7):'j'' '/79,"
			"screen-line(8):'k'' '/79,"
			"screen-line(9):'l'' '/79,"
			"screen-line(10):'m'' '/79,"
			"screen-line(11):'n'' '/79,"
			"screen-line(12):'o'' '/79,"
			"screen-line(13):'p'' '/79,"
			"screen-line(14):'q'' '/79,"
			"screen-line(15):'r'' '/79,"
			"screen-line(16):'s'' '/79,"
			"screen-line(17):'t'' '/79,"
			"screen-line(18):'u'' '/79,"
			"screen-line(19):'v'' '/79,"
			"screen-line(20):'w'' '/79,"
			"screen-line(21):'x''😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]'😃'(1f603)[full]' '[ignore]' ',"
			"screen-line(22):'😃'(1f603)[full]' '[ignore]' '/78,"
			"screen-line(23):' '/80,"
			"move(23/0)"
			);
}
 
void
test_wc_convert(const char * file, int line, tmt_wchar_t wc, unsigned char * list, size_t len) {
	unsigned char buffer[5];
	int actual = wc_to_utf8((char *)buffer, 5, wc);
	if (actual != len) {
		fprintf(stderr, "%s:%d comparison failed: mismatched len, had=%d, expected=%zu\n", file, line, actual, len);
		_failure_count += 1;
		return;
	}

	for (int i=0; i<len; i++) {
		if (buffer[i] != list[i]) {
			fprintf(stderr, "%s:%d comparison failed: had[%d]=%hhx, expected[%d]=%hhx\n", file, line, i, buffer[i], i, list[i]);
			_failure_count += 1;
			return;
		}
	}
}

static unsigned char ucnvt_200d_to_utf8[] = { 0xE2, 0x80, 0x8d };

void
test_wc_to_utf8() {
	test_wc_convert(__FILE__, __LINE__, 0x200d, ucnvt_200d_to_utf8, sizeof(ucnvt_200d_to_utf8)); 
}

int
main(int argc, char * argv[]) {
	test_wc_to_utf8();
	test_wide_lookup();
	test_tmt_write();
	printf("Tests attempted: %d\n", _attempt_count);
	printf("Tests failed: %d\n", _failure_count);
	if (_failure_count > 0) {
		exit(1);
	}
}


