
#include "gdterm.h"
#include "godot_cpp/classes/display_server.hpp"
#include "godot_cpp/classes/global_constants.hpp"
#include "godot_cpp/classes/input_event_mouse_button.hpp"
#include "godot_cpp/classes/input_event_mouse_motion.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/viewport.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "key_converter.h"
#include <assert.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/time.hpp>

using namespace godot;

static const int TARGET_SCROLL  = 1;
static const int TARGET_SCREEN  = 2;

static const int STATE_SCREEN_BEGIN   = 1;
static const int STATE_SCREEN_ROW     = 2;
static const int STATE_SCREEN_RESIZE  = 3;
static const int STATE_SCREEN_DONE    = 4;

static const int SELECT_MODE_CHAR   = 1;
static const int SELECT_MODE_WORD   = 2;
static const int SELECT_MODE_LINE   = 3;

extern PtyProxy * create_proxy(TermRenderer * renderer);

void GDTerm::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_font"), &GDTerm::get_font);
	ClassDB::bind_method(D_METHOD("set_font", "font"), &GDTerm::set_font);
	ClassDB::bind_method(D_METHOD("get_dim_font"), &GDTerm::get_dim_font);
	ClassDB::bind_method(D_METHOD("set_dim_font", "dim_font"), &GDTerm::set_dim_font);
	ClassDB::bind_method(D_METHOD("get_bold_font"), &GDTerm::get_bold_font);
	ClassDB::bind_method(D_METHOD("set_bold_font", "bold_font"), &GDTerm::set_bold_font);
	ClassDB::bind_method(D_METHOD("get_font_size"), &GDTerm::get_font_size);
	ClassDB::bind_method(D_METHOD("set_font_size", "font_size"), &GDTerm::set_font_size);
	ClassDB::bind_method(D_METHOD("get_black"), &GDTerm::get_black);
	ClassDB::bind_method(D_METHOD("set_black", "black"), &GDTerm::set_black);
	ClassDB::bind_method(D_METHOD("get_red"), &GDTerm::get_red);
	ClassDB::bind_method(D_METHOD("set_red", "red"), &GDTerm::set_red);
	ClassDB::bind_method(D_METHOD("get_green"), &GDTerm::get_green);
	ClassDB::bind_method(D_METHOD("set_green", "green"), &GDTerm::set_green);
	ClassDB::bind_method(D_METHOD("get_yellow"), &GDTerm::get_yellow);
	ClassDB::bind_method(D_METHOD("set_yellow", "yellow"), &GDTerm::set_yellow);
	ClassDB::bind_method(D_METHOD("get_blue"), &GDTerm::get_blue);
	ClassDB::bind_method(D_METHOD("set_blue", "blue"), &GDTerm::set_blue);
	ClassDB::bind_method(D_METHOD("get_magenta"), &GDTerm::get_magenta);
	ClassDB::bind_method(D_METHOD("set_magenta", "magenta"), &GDTerm::set_magenta);
	ClassDB::bind_method(D_METHOD("get_cyan"), &GDTerm::get_cyan);
	ClassDB::bind_method(D_METHOD("set_cyan", "cyan"), &GDTerm::set_cyan);
	ClassDB::bind_method(D_METHOD("get_white"), &GDTerm::get_white);
	ClassDB::bind_method(D_METHOD("set_white", "white"), &GDTerm::set_white);
	ClassDB::bind_method(D_METHOD("get_foreground"), &GDTerm::get_foreground);
	ClassDB::bind_method(D_METHOD("set_foreground", "foreground"), &GDTerm::set_foreground);
	ClassDB::bind_method(D_METHOD("get_background"), &GDTerm::get_background);
	ClassDB::bind_method(D_METHOD("set_background", "background"), &GDTerm::set_background);
	ClassDB::bind_method(D_METHOD("get_vt_handler_log_path"), &GDTerm::get_vt_handler_log_path);
	ClassDB::bind_method(D_METHOD("set_vt_handler_log_path", "vt_handler_log_path"), &GDTerm::set_vt_handler_log_path);
	ClassDB::bind_method(D_METHOD("clear"), &GDTerm::clear);
	ClassDB::bind_method(D_METHOD("is_active"), &GDTerm::is_active);
	ClassDB::bind_method(D_METHOD("start"), &GDTerm::start);
	ClassDB::bind_method(D_METHOD("stop"), &GDTerm::stop);
	ClassDB::bind_method(D_METHOD("get_num_scrollback_lines"), &GDTerm::get_num_scrollback_lines);
	ClassDB::bind_method(D_METHOD("get_num_screen_lines"), &GDTerm::get_num_screen_lines);
	ClassDB::bind_method(D_METHOD("get_scroll_pos"), &GDTerm::get_scroll_pos);
	ClassDB::bind_method(D_METHOD("set_scroll_pos", "pos"), &GDTerm::set_scroll_pos);
	ClassDB::bind_method(D_METHOD("get_selected_text"), &GDTerm::get_selected_text);
	ClassDB::bind_method(D_METHOD("send_input", "text"), &GDTerm::send_input);
	ClassDB::bind_method(D_METHOD("_on_cursor_timeout"), &GDTerm::_on_cursor_timeout);
	ClassDB::bind_method(D_METHOD("_on_blink_timeout"), &GDTerm::_on_blink_timeout);
	ClassDB::bind_method(D_METHOD("_on_bell_request"), &GDTerm::_on_bell_request);
	ClassDB::bind_method(D_METHOD("_on_focus_entered"), &GDTerm::_on_focus_entered);
	ClassDB::bind_method(D_METHOD("_on_focus_exited"), &GDTerm::_on_focus_exited);
	ClassDB::bind_method(D_METHOD("_on_inactive"), &GDTerm::_on_inactive);
	ClassDB::bind_method(D_METHOD("_on_resized"), &GDTerm::_on_resized);
	ClassDB::bind_method(D_METHOD("_notify_scrollback"), &GDTerm::_notify_scrollback);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_font", "get_font");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "dim_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_dim_font", "get_dim_font");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "bold_font", PROPERTY_HINT_RESOURCE_TYPE, "Font"), "set_bold_font", "get_bold_font");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "font_size", PROPERTY_HINT_RANGE, "8,24,2"), "set_font_size", "get_font_size");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "black"), "set_black", "get_black");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "red"), "set_red", "get_red");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "green"), "set_green", "get_green");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "yellow"), "set_yellow", "get_yellow");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "blue"), "set_blue", "get_blue");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "magenta"), "set_magenta", "get_magenta");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "cyan"), "set_cyan", "get_cyan");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "white"), "set_white", "get_white");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "foreground"), "set_foreground", "get_foreground");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "background"), "set_background", "get_background");

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "vt_handler_log_path"), "set_vt_handler_log_path", "get_vt_handler_log_path");

	ADD_SIGNAL(MethodInfo("bell_request"));
	ADD_SIGNAL(MethodInfo("inactive"));
	ADD_SIGNAL(MethodInfo("scrollback_changed"));
}

GDTerm::GDTerm() {
	font_size = 14;
	black = godot::Color("#000000");
	red = godot::Color("#BB0000");
	green = godot::Color("#00BB00");
	yellow = godot::Color("#BBBB00");
	blue = godot::Color("#0000BB");
	magenta = godot::Color("#BB00BB");
	cyan = godot::Color("#00BBBB");
	white = godot::Color("#BBBBBB");
	foreground = godot::Color("#111111");
	background = godot::Color("#EEEEEE");
	set_clip_contents(true);
	set_focus_mode(Control::FocusMode::FOCUS_ALL);
	set_vt_handler_log_path("");

	// Proxy
	_proxy = nullptr;
	_active = false;

	// Renderable
	_screen_lines.resize(24); 
	_scrollback_pos = 0;
	
	// Pending
	_pending_dirty = false;
	_pending_screen_lines.resize(24);
	_pending_state = STATE_SCREEN_DONE;
	_pending_screen_row = 0;
	_pending_cursor_displayed = true;
	_pending_target = TARGET_SCREEN;
	_drawing_active = false;

	// Cursor
	_cursor_timer = nullptr;
	_cursor_showing = false;
	_cursor_blink_count = 0;
	_cursor_displayed = true;
	_cursor_row = 0;
	_cursor_col = 0;

	// Blink
	_blink_timer = nullptr;
	_blink_showing = true;

	// Size
	_min_size = Vector2(100, 100);
	_rows = 24;
	_cols = 80;

	// Selection
	_selecting = false;
	_selection_active = false;
	_select_start_row = 0;
	_select_start_col = 0;
	_select_end_row = 0;
	_select_end_col = 0;
	_select_mode = SELECT_MODE_CHAR;

	// Logging
	_vt_handler_input_log = nullptr;
}

GDTerm::~GDTerm() {
	stop();
}

int
GDTerm::get_num_scrollback_lines() const {
	return _scrollback.size();
}

int
GDTerm::get_num_screen_lines() const {
	return _rows;
}

int
GDTerm::get_scroll_pos() const {
	return _scrollback_pos;
}

void
GDTerm::set_scroll_pos(int pos) {
	if ((pos >= 0) && (pos <= _scrollback.size())) {
		_scrollback_pos = pos;
		queue_redraw();
	}
}

void
GDTerm::_ready() {

	_cursor_timer = memnew(godot::Timer);
	_cursor_timer->set_wait_time(0.8);
	_cursor_timer->set_one_shot(false);
	_cursor_timer->connect("timeout", Callable(this, "_on_cursor_timeout"));
	add_child(_cursor_timer);

	_blink_timer = memnew(godot::Timer);
	_blink_timer->set_wait_time(0.6);
	_blink_timer->set_one_shot(false);
	_blink_timer->connect("timeout", Callable(this, "_on_blink_timeout"));
	add_child(_blink_timer);

	connect("focus_entered", Callable(this, "_on_focus_entered"));
	connect("focus_exited", Callable(this, "_on_focus_exited"));
	connect("resized", Callable(this, "_on_resized"));
}

void
GDTerm::set_black(Color c) {
	if (black != c) {
		black = c;
	}
}

Color
GDTerm::get_black() const {
	return black;
}

void
GDTerm::set_red(Color c) {
	if (red != c) {
		red = c;
	}
}

Color
GDTerm::get_red() const {
	return red;
}

void
GDTerm::set_green(Color c) {
	if (green != c) {
		green = c;
	}
}

Color
GDTerm::get_green() const {
	return green;
}

void
GDTerm::set_yellow(Color c) {
	if (yellow != c) {
		yellow = c;
	}
}

Color
GDTerm::get_yellow() const {
	return yellow;
}

void
GDTerm::set_blue(Color c) {
	if (blue != c) {
		blue = c;
	}
}

Color
GDTerm::get_blue() const {
	return blue;
}

void
GDTerm::set_magenta(Color c) {
	if (magenta != c) {
		magenta = c;
	}
}

Color
GDTerm::get_magenta() const {
	return magenta;
}

void
GDTerm::set_cyan(Color c) {
	if (cyan != c) {
		cyan = c;
	}
}

Color
GDTerm::get_cyan() const {
	return cyan;
}

void
GDTerm::set_white(Color c) {
	if (white != c) {
		white = c;
	}
}

Color
GDTerm::get_white() const {
	return white;
}

void
GDTerm::set_foreground(Color c) {
	if (foreground != c) {
		foreground = c;
	}
}

Color
GDTerm::get_foreground() const {
	return foreground;
}

void
GDTerm::set_background(Color c) {
	if (background != c) {
		background = c;
	}
}

String
GDTerm::get_vt_handler_log_path() const {
	return vt_handler_log_path;
}

void
GDTerm::set_vt_handler_log_path(String log_path) {
	if (vt_handler_log_path != log_path) {
		vt_handler_log_path = log_path;
		if (_vt_handler_input_log != nullptr) {
			_vt_handler_input_log->close();
			delete _vt_handler_input_log;
		}
		if (vt_handler_log_path.length() != 0) {
			_vt_handler_input_log = new std::fstream();
			String os_path = ProjectSettings::get_singleton()->globalize_path(vt_handler_log_path);
			_vt_handler_input_log->open(os_path.utf8(), std::ios::out);
		}
	}
}

Color
GDTerm::get_background() const {
	return background;
}

void 
GDTerm::set_font(const Ref<Font> & p_font) {
	if (font != p_font) {
		font = p_font;
		_do_resize();
	}
}

Ref<Font>
GDTerm::get_font() const {
	return font;
}

void 
GDTerm::set_dim_font(const Ref<Font> & p_dim_font) {
	if (dim_font != p_dim_font) {
		dim_font = p_dim_font;
	}
}

Ref<Font>
GDTerm::get_dim_font() const {
	return dim_font;
}

void 
GDTerm::set_bold_font(const Ref<Font> & p_bold_font) {
	if (bold_font != p_bold_font) {
		bold_font = p_bold_font;
	}
}

Ref<Font>
GDTerm::get_bold_font() const {
	return bold_font;
}

void 
GDTerm::set_font_size(int p_font_size) {
	if (font_size != p_font_size) {
		font_size = p_font_size;
		_do_resize();
	}
}

int
GDTerm::get_font_size() const {
	return font_size;
}

void
GDTerm::clear() {
	_screen_lines.resize(_rows);
	_scrollback.clear();
	_scrollback_pos = 0;
	_cursor_showing = false;
	_cursor_displayed = true;
	_cursor_row = 0;
	_cursor_col = 0;
	_blink_showing = true;
	_selecting = false;
	_selection_active = false;
	_select_mode = SELECT_MODE_CHAR;
	_clear_pending();
}

void
GDTerm::start() {

	// Make sure it inside the tree before starting
	if (!is_inside_tree()) {
		UtilityFunctions::printerr("started terminal prior to being inside tree");
		return;
	}

	// Make sure it isn't already active and set active atomically
	{
		const std::lock_guard<std::mutex> lock(_pending_mutex);
		if (_active) {
			return;
		}
		_active = true;
	}

	// Delete the proxy if it already existed and create a new one
	if (_proxy != nullptr) {
		delete _proxy;
		_proxy = nullptr;
	}

	// Clear the screen and scrollback ready for a new proxy
	clear();

	// Create a new proxy
	_proxy = create_proxy(this);
	_proxy->resize_screen(_rows, _cols);
}

void
GDTerm::stop() {
	if (_proxy != nullptr) {
		delete _proxy;
		_proxy = nullptr;
	}

	if (_vt_handler_input_log != nullptr) {
		_vt_handler_input_log->close();
		delete _vt_handler_input_log;
		_vt_handler_input_log = nullptr;
	}
}

bool
GDTerm::is_active() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	return _active;
}

void
GDTerm::_draw_term_line(Vector2 & pos, const GDTermLine & line, int cursor_row, int actual_row) {
	float font_height = font->get_height(font_size);
	float font_ascent = font->get_ascent(font_size);
	float font_underline = font->get_underline_position(font_size);
	float font_descent = font->get_descent(font_size);
	Color fg_color = foreground;
	Color bg_color = background;
	bool reverse = false;
	bool invisible = false;
	bool blinking = false;
	bool underline = false;

	Ref<Font> cur_font = font;
	Vector2 seg_pos = pos;

	Color cur_fg = fg_color;
	Color cur_bg = bg_color;
	int col = 0;
	for (int j=0; j<line.dirs.size(); j++) {
		int kind = line.dirs[j].kind;
		cur_fg = fg_color;
		cur_bg = bg_color;
		if (reverse) {
			std::swap(cur_fg, cur_bg);
		}
		if (_selection_active && _is_in_selection(actual_row, col)) {
			std::swap(cur_fg, cur_bg);
		}
		bool outline_cursor = false;
		if (_cursor_displayed && _active) {
			if (_is_cursor_pos(cursor_row, col)) {
				if (!has_focus()) {
					outline_cursor = true;
				} else {
					if (_cursor_showing) {
						std::swap(cur_fg, cur_bg);
					}
				}
			}
		}
		if (blinking) {
			if (!_blink_showing) {
				cur_fg = cur_bg;
			}
		}
		if (invisible) {
			cur_fg = cur_bg;
		}
		if (kind == DIRECTIVE_WRITE_GLYPH) {
			// TODO - Check that LC_CTYPE is UTF8
			String glyph = String::utf8(line.dirs[j].data.text.c_str(), line.dirs[j].data.text.length());
			Rect2 rect = Rect2(seg_pos.x, seg_pos.y-font_ascent, _font_space_size.x, font_height);
			draw_rect(rect, cur_bg, true);
			if (outline_cursor) {
				draw_rect(rect, cur_fg, false);
			}
			draw_string(cur_font, seg_pos, glyph, godot::HORIZONTAL_ALIGNMENT_LEFT, -1, font_size, cur_fg);
			if (underline) {
				draw_line(Vector2(seg_pos.x, seg_pos.y + font_underline), Vector2(seg_pos.x + _font_space_size.x, seg_pos.y + font_underline), cur_fg);
			}
			seg_pos.x += _font_space_size.x;
			col += 1;
		} else if (kind == DIRECTIVE_SET_STATE) {
			LineTag tag = line.dirs[j].data.tag;
			switch (tag) {
				case BOLD:
					cur_font = bold_font;
					break;
				case DIM:
					cur_font = dim_font;
					break;
				case REVERSE:
					reverse = true;
					break;
				case BLINK:
					blinking = true;
					break;
				case INVISIBLE:
					invisible = true;
					break;
				case UNDERLINE:
					underline = true;
					break;
				case FG_COLOR_BLACK:
					fg_color = black;
					break;
				case FG_COLOR_RED:
					fg_color = red;
					break;
				case FG_COLOR_GREEN:
					fg_color = green;
					break;
				case FG_COLOR_YELLOW:
					fg_color = yellow;
					break;
				case FG_COLOR_BLUE:
					fg_color = blue;
					break;
				case FG_COLOR_MAGENTA:
					fg_color = magenta;
					break;
				case FG_COLOR_CYAN:
					fg_color = cyan;
					break;
				case FG_COLOR_WHITE:
					fg_color = white;
					break;
				case BG_COLOR_BLACK:
					bg_color = black;
					break;
				case BG_COLOR_RED:
					bg_color = red;
					break;
				case BG_COLOR_GREEN:
					bg_color = green;
					break;
				case BG_COLOR_YELLOW:
					bg_color = yellow;
					break;
				case BG_COLOR_BLUE:
					bg_color = blue;
					break;
				case BG_COLOR_MAGENTA:
					bg_color = magenta;
					break;
				case BG_COLOR_CYAN:
					bg_color = cyan;
					break;
				case BG_COLOR_WHITE:
					bg_color = white;
					break;
				default:
					break;
			}
		} else if (kind == DIRECTIVE_CLEAR_STATE) {
			LineTag tag = line.dirs[j].data.tag;
			switch (tag) {
				case BOLD:
				case DIM:
					cur_font = font;
					break;
				case REVERSE:
					reverse = false;
					break;
				case INVISIBLE:
					invisible = false;
					break;
				case BLINK:
					blinking = false;
					break;
				case UNDERLINE:
					underline = false;
					break;
				case FG_COLOR_BLACK:
				case FG_COLOR_RED:
				case FG_COLOR_GREEN:
				case FG_COLOR_YELLOW:
				case FG_COLOR_BLUE:
				case FG_COLOR_MAGENTA:
				case FG_COLOR_CYAN:
				case FG_COLOR_WHITE:
					fg_color = foreground;
					break;
				case BG_COLOR_BLACK:
				case BG_COLOR_RED:
				case BG_COLOR_GREEN:
				case BG_COLOR_YELLOW:
				case BG_COLOR_BLUE:
				case BG_COLOR_MAGENTA:
				case BG_COLOR_CYAN:
				case BG_COLOR_WHITE:
					bg_color = background;
					break;
				default:
					break;
			}

		}
	}
	Rect2 rect = Rect2(seg_pos.x, seg_pos.y-font_ascent, _font_space_size.x, font_height);
	draw_rect(rect, cur_bg, true);
	pos.y += font_height;
}

void
GDTerm::_process(double p_delta) {
	if (_is_screen_dirty()) {
		queue_redraw();
	}
	if (_send_input_buffer.length() > 0) {
		if (_proxy != nullptr) {
			int max_send = _proxy->available_to_send();
			_send_input_chunk(max_send);
		}
	}
}

void
GDTerm::_draw() {
	// Don't draw unless there is a font
	if (font.is_null() || dim_font.is_null() || bold_font.is_null()) {
		return;
	}

	// Get any pending updates that came in on other threads
	// into the active set of rendering directives
	_make_pending_active();

	draw_rect(get_rect(), background, true);
	Vector2 font_space_size = font->get_string_size(" ", godot::HORIZONTAL_ALIGNMENT_LEFT, -1, font_size);
	float font_height = font->get_height(font_size);
	float font_ascent = font->get_ascent(font_size);
	float xpos = 0.0;
	float ypos = font_ascent;
	Vector2 pos(xpos, ypos);
	int line_idx = 0;
	while (line_idx < _rows) {
		if ((_scrollback_pos + line_idx) < _scrollback.size()) {
			GDTermLine &line = _scrollback[_scrollback_pos + line_idx];
			_draw_term_line(pos, line, -1, _scrollback_pos+line_idx);
		} else {
			int screen_line = line_idx - (_scrollback.size() - _scrollback_pos);
			GDTermLine &line = _screen_lines[screen_line];
			_draw_term_line(pos, line, screen_line, _scrollback.size() + screen_line);
		}
		line_idx += 1;
	}
	Rect2 rect = Rect2(pos.x, pos.y-font_ascent, (_cols+1)*font_space_size.x, font_height);
	draw_rect(rect, background, true);
}

static const uint64_t CLICK_DELTA_TIME = 500;

void
GDTerm::_gui_input(const Ref<InputEvent> & p_event) {
	Ref<InputEventKey> ke = p_event;
	if (ke != NULL) {
		if (ke->is_pressed()) {
			if (!_active) {
				emit_signal("bell_request");
				return;
			}
			assert(sizeof(wchar_t) == sizeof(int64_t));
			wchar_t unicode = (wchar_t)ke->get_unicode();
			Key code = ke->get_keycode_with_modifiers();
			if (_is_shift_control_tab(code)) {
				Control * prev = find_prev_valid_focus();
				prev->grab_focus();
			} else if (_is_control_tab(code)) {
				Control * next = find_next_valid_focus();
				next->grab_focus();
			} else {
				char buffer[100];
				fill_term_string(buffer, 100, unicode, code);
				if (_proxy != nullptr) {
					_proxy->send_string(buffer);
				} else {
					emit_signal("bell_request");
				}
				if (_is_control_c(code)) {
					_send_input_buffer = "";
				}
			}
			Control::accept_event();
		}
	}

	Ref<InputEventMouseButton> mbe = p_event;
	if (mbe != nullptr) {
		if (mbe->get_button_index() == 1) {
			if (mbe->is_pressed()) {
				Vector2 mpos = mbe->get_position();
				int mouse_row = mpos.y / _font_space_size.y;
				int mouse_col = mpos.x / _font_space_size.x;
				if (!_selecting) {
					if (_selection_active && !_is_in_selection(mouse_row+_scrollback_pos, mouse_col)) {
						_selection_active = false;
						_selecting = false;
						_select_mode = SELECT_MODE_CHAR;
						_select_tick = godot::Time::get_singleton()->get_ticks_msec();
					} else {
						uint64_t now = godot::Time::get_singleton()->get_ticks_msec();
						uint64_t delta = now - _select_tick;
						if (delta < (_select_mode * CLICK_DELTA_TIME)) {
							_select_mode += 1;
							if (_select_mode > SELECT_MODE_LINE) {
								_select_mode = SELECT_MODE_CHAR;
								_selection_active = false;
							} else {
								_selection_active = true;
							}
						} else {
							_select_mode = SELECT_MODE_CHAR;
							_selection_active = false;
							_select_tick = godot::Time::get_singleton()->get_ticks_msec();
						}
					}
				}
				_selecting = true;
				_select_start_row = _scrollback_pos + mouse_row;
				_update_select_for_start_col(_select_start_row, mouse_col);
				_select_end_row = _scrollback_pos + mouse_row;
				_update_select_for_end_col(_select_end_row, mouse_col);
				queue_redraw();
			} else {
				if (_selecting) {
					_selecting = false;
					uint64_t now = godot::Time::get_singleton()->get_ticks_msec();
					uint64_t delta = now - _select_tick;
					if (_select_mode == SELECT_MODE_CHAR) {
						if (delta < CLICK_DELTA_TIME) {
							_selection_active = false;
						}
					}
					queue_redraw();
				}
				if (_selection_active) {
					godot::String text = get_selected_text();
					if (text.length() > 0) {
						godot::DisplayServer::get_singleton()->clipboard_set_primary(text);
					}
				}
			}
		}
	}

	Ref<InputEventMouseMotion> mme = p_event;
	if (mme != nullptr) {
		if (_selecting) {
			Vector2 mpos = mme->get_position();
			int mouse_row = mpos.y / _font_space_size.y;
			int mouse_col = mpos.x / _font_space_size.x;
			_select_end_row = _scrollback_pos + mouse_row;
			_update_select_for_end_col(_select_end_row, mouse_col);
			uint64_t now = godot::Time::get_singleton()->get_ticks_msec();
			uint64_t delta = now - _select_tick;
			if (delta >= CLICK_DELTA_TIME) {
				_selection_active = true;
			}
			queue_redraw();
		}
	}

}


bool
GDTerm::_is_screen_dirty() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_dirty || _pending_cursor_dirty) {
		return true;
	}

	return false;
}

void 
GDTerm::update_cursor(int row, int col) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_pending_cursor_dirty = true;
	_pending_cursor_row = row;
	_pending_cursor_col = col;
}

void 
GDTerm::screen_begin() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_state != STATE_SCREEN_DONE) {
		return;
	}
	_pending_target = TARGET_SCREEN;
	_pending_state = STATE_SCREEN_BEGIN;
}

void 
GDTerm::screen_set_row(int row) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if ((_pending_state != STATE_SCREEN_BEGIN) && (_pending_state != STATE_SCREEN_ROW)) {
		return;
	}
	if (row > _pending_screen_lines.size()) {
		fprintf(stderr, "Invalid row %d: maximum value %zu\n", row, _pending_screen_lines.size());
		return;
	}
	_pending_screen_row = row;
	if (_pending_target == TARGET_SCREEN) {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_screen_lines.size())) {
			_pending_screen_lines[row].dirs.clear();
		}
	} else {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_scrollback.size())) {
			_pending_scrollback[row].dirs.clear();
		}
	}
	_pending_state = STATE_SCREEN_ROW;
}

void 
GDTerm::screen_add_tag(LineTag tag) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if ((_pending_state != STATE_SCREEN_ROW)) {
		return;
	}
	GDTermLineDirective d;
	d.kind = DIRECTIVE_SET_STATE;
	d.data.tag = tag;
	if (_pending_target == TARGET_SCREEN) {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_screen_lines.size())) {
			_pending_screen_lines[_pending_screen_row].dirs.push_back(d);
		}
	} else {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_scrollback.size())) {
			_pending_scrollback[_pending_screen_row].dirs.push_back(d);
		}
	}
}

void 
GDTerm::screen_remove_tag(LineTag tag) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if ((_pending_state != STATE_SCREEN_ROW)) {
		return;
	}
	GDTermLineDirective d;
	d.kind = DIRECTIVE_CLEAR_STATE;
	d.data.tag = tag;
	if (_pending_target == TARGET_SCREEN) {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_screen_lines.size())) {
			_pending_screen_lines[_pending_screen_row].dirs.push_back(d);
		}
	} else {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_scrollback.size())) {
			_pending_scrollback[_pending_screen_row].dirs.push_back(d);
		}
	}
}

void 
GDTerm::screen_add_glyph(const char * c, int len) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_state != STATE_SCREEN_ROW) {
		return;
	}
	GDTermLineDirective d;
	d.kind = DIRECTIVE_WRITE_GLYPH;
	d.data.text = std::string(c, len);
	if (_pending_target == TARGET_SCREEN) {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_screen_lines.size())) {
			_pending_screen_lines[_pending_screen_row].dirs.push_back(d);
		}
	} else {
		if ((_pending_screen_row >= 0) && (_pending_screen_row < _pending_scrollback.size())) {
			_pending_scrollback[_pending_screen_row].dirs.push_back(d);
		}
	}
}

void 
GDTerm::screen_done() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_state == STATE_SCREEN_RESIZE) {
		return;
	}
	_pending_dirty = true;
	_pending_state = STATE_SCREEN_DONE;
}

bool
GDTerm::_clear_drawing() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_drawing_active = false;
	return _pending_dirty || _pending_cursor_dirty;
}

void 
GDTerm::scroll_begin(int size) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_state != STATE_SCREEN_DONE) {
		return;
	}
	_pending_scrollback.clear();
	_pending_scrollback.resize(size);
	_pending_target = TARGET_SCROLL;
	_pending_state = STATE_SCREEN_BEGIN;
}

void 
GDTerm::scroll_done() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	if (_pending_state == STATE_SCREEN_RESIZE) {
		return;
	}
	_pending_saved_scrollback.insert(_pending_saved_scrollback.end(), _pending_scrollback.begin(), _pending_scrollback.end());
	_pending_dirty = true;
	_pending_state = STATE_SCREEN_DONE;
}

void
GDTerm::exited() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_active = false;
	call_deferred("_on_inactive");
}

void
GDTerm::resize_complete() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_pending_state = STATE_SCREEN_DONE;
}

bool
GDTerm::_blink_on_line(GDTermLine &line) const {
	for (int j=0; j<line.dirs.size(); j++) {
		if ((line.dirs[j].kind == DIRECTIVE_SET_STATE) && (line.dirs[j].data.tag == LineTag::BLINK)) {
			return true;
		}
	}
	return false;
}

const GDTermLine &
GDTerm::_get_term_line(int row) const {
	if (row < _scrollback.size()) {
		return _scrollback[row];
	}
	return _screen_lines[row-_scrollback.size()];
}

void
GDTerm::_calc_select_word_col(const GDTermLine & line, int & start_col, int & end_col, int col) const {
	int cur_col=0;
	bool in_word=false;
	bool found_word=false;
	for (int i=0; i<line.dirs.size(); i++) {
		if (line.dirs[i].kind == DIRECTIVE_WRITE_GLYPH) {
			wchar_t buf[2];
			size_t num_converted;
#ifdef USE_WCRTOMB_S
			mbstowcs_s(&num_converted, buf, line.dirs[i].data.text.c_str(), 1);
#else
			num_converted = mbstowcs(buf, line.dirs[i].data.text.c_str(), 1);
#endif
			if (isspace(buf[0])) {
				if (found_word) {
					return;
				}
				start_col = cur_col;
				end_col = cur_col;
				in_word = false;
			} else {
				if (in_word) {
					end_col = cur_col;
				} else {
					start_col = cur_col;
					end_col = cur_col;
					in_word = true;
				}
			}
			if (cur_col == col) {
				if (in_word) {
					found_word = true;
				} else {
					return;
				}
			}
			cur_col += 1;
		}
	}
}

void
GDTerm::_update_select_for_start_col(int row, int col) {
	if (_select_mode == SELECT_MODE_WORD) {
		int start_col;
		int end_col;
		const GDTermLine & line = _get_term_line(row);
		_calc_select_word_col(line, start_col, end_col, col);
		_select_start_col = start_col;
		return;
	} else if (_select_mode == SELECT_MODE_LINE) {
		_select_start_col = 0;
		return;
	}
	_select_start_col = col;
}

void
GDTerm::_update_select_for_end_col(int row, int col) {
	if (_select_mode == SELECT_MODE_WORD) {
		int start_col;
		int end_col;
		int new_start_col;
		int new_end_col;
		const GDTermLine & line = _get_term_line(row);
		const GDTermLine & start_line = _get_term_line(_select_start_row);
		if ((row > _select_start_row) || ((row == _select_start_row) && (col >= _select_start_col))) {
			_calc_select_word_col(start_line, new_start_col, new_end_col, _select_start_col);
			_select_start_col = new_start_col;
			_calc_select_word_col(line, start_col, end_col, col);
			_select_end_col = end_col;
		} else {
			_calc_select_word_col(start_line, new_start_col, new_end_col, _select_start_col);
			_select_start_col = new_end_col;
			_calc_select_word_col(line, start_col, end_col, col);
			_select_end_col = start_col;
		}
		return;
	} else if (_select_mode == SELECT_MODE_LINE) {
		const GDTermLine & line = _get_term_line(row);
		if (row >= _select_start_row) {
			_select_end_col = _calc_line_size(line)-1;
			_select_start_col = 0;
		} else {
			const GDTermLine & start_line = _get_term_line(_select_start_row);
			_select_start_col = _calc_line_size(start_line)-1;
			_select_end_col = 0;
		}
		return;
	}
	_select_end_col = col;
}

int
GDTerm::_calc_line_size(const GDTermLine & line) const {
	int size = 0;
	for (int i=0; i<line.dirs.size(); i++) {
		if (line.dirs[i].kind == DIRECTIVE_WRITE_GLYPH) {
			size += 1;
		}
	}
	return size;
}

void
GDTerm::_clear_pending() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_pending_state = STATE_SCREEN_DONE;
	_pending_dirty = false;
	_pending_cursor_dirty = false;
}

#define SEND_INPUT_BUFFER_SIZE  1024 

void
GDTerm::send_input(godot::String text) {
	_send_input_buffer += text;
	if (_proxy != nullptr) {
		int max_send = _proxy->available_to_send();
		_send_input_chunk(max_send);
	}
}

godot::String
GDTerm::get_selected_text() const {
	std::string selection;
	if (_selection_active) {
		int start_row = _select_start_row;
		int start_col = _select_start_col;
		int end_row = _select_end_row;
		int end_col = _select_end_col;
		if (end_row < start_row) {
			std::swap(start_row, end_row);
			std::swap(start_col, end_col);
		} else if (start_row == end_row) {
			if (end_col <= start_col) {
				std::swap(start_col, end_col);
			}
		}
		for (int row=start_row; row<=end_row; row++) {
			int row_start_col = (row > start_row) ? 0 : start_col;
			int row_end_col = (row < end_row) ? _cols : end_col;
			int cur_col = 0;
			if (row > start_row) { selection += "\n"; }
			const GDTermLine & line = (row < _scrollback.size()) ? _scrollback[row] : _screen_lines[row-_scrollback.size()];
			for (int d=0; d<line.dirs.size(); d++) {
				const GDTermLineDirective & dir = line.dirs[d];
				if (dir.kind == DIRECTIVE_WRITE_GLYPH) {
					if ((cur_col >= row_start_col) && (cur_col <= row_end_col)) {
						selection += dir.data.text;
					}
					cur_col += 1;
				}
			}
		}
	}
	godot::String s = godot::String::utf8(selection.c_str());
	return s;
}

void
GDTerm::_make_pending_active() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_drawing_active = true;
	if (_pending_state != STATE_SCREEN_DONE) {
		return;
	}
	if (_pending_dirty) {
		_screen_lines = _pending_screen_lines;
		if (_pending_saved_scrollback.size() > 0) {
			bool at_end = _scrollback_pos == _scrollback.size();
			_scrollback.insert(_scrollback.end(), _pending_saved_scrollback.begin(), _pending_saved_scrollback.end());
			if (at_end) {
				_scrollback_pos = _scrollback.size();
			}
			_pending_saved_scrollback.clear();
			call_deferred("_notify_scrollback");
		}
		_pending_dirty = false;
	}
	if (_pending_cursor_dirty) {
		_cursor_row = _pending_cursor_row;
		_cursor_col = _pending_cursor_col;
		_cursor_displayed = _pending_cursor_displayed;
		_pending_cursor_dirty = false;
		_restart_cursor();
	}
	bool had_blink = false;
	int line_idx = 0;
	while (line_idx < _rows) {
		if ((_scrollback_pos + line_idx) < _scrollback.size()) {
			GDTermLine &line = _scrollback[_scrollback_pos + line_idx];
			if (_blink_on_line(line)) {
				had_blink = true;
			}
		} else {
			int screen_line = line_idx - (_scrollback.size() - _scrollback_pos);
			GDTermLine &line = _screen_lines[screen_line];
			if (_blink_on_line(line)) {
				had_blink = true;
			}
		}
		line_idx += 1;
	}
	if (had_blink) {
		if (!_blink_displayed) {
			_blink_displayed = true;
			_blink_timer->start();
		}
	} else {
		if (_blink_displayed) {
			_blink_displayed = false;
			_blink_timer->stop();
		}
	}
}

void 
GDTerm::play_bell() {
	call_deferred("_on_bell_request");
}

void 
GDTerm::show_cursor(bool flag) {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_pending_cursor_displayed = flag;
	_pending_cursor_dirty = true;
	call_deferred("queue_redraw");
}

void
GDTerm::_restart_cursor() {
	if (_cursor_timer != nullptr) {
		_cursor_timer->stop();
		_cursor_timer->start();
		_cursor_showing = true;
		_cursor_blink_count = 0;
	}
}

bool
GDTerm::_is_cursor_pos(int row, int col) {
	if ((_cursor_row == row) && (_cursor_col == col)) {
		return true;
	}

	return false;
}

bool
GDTerm::_is_in_selection(int row, int col) {
	int start_row = _select_start_row;
	int start_col = _select_start_col;
	int end_row = _select_end_row;
	int end_col = _select_end_col;
	if (end_row < start_row) {
		std::swap(start_row, end_row);
		std::swap(start_col, end_col);
	} else if (start_row == end_row) {
		if (end_col <= start_col) {
			std::swap(start_col, end_col);
		}
	}

	if ((row > start_row) && (row < end_row)) {
		return true;
	}

	if (row == start_row) {
		if (col >= start_col) {
			if (row < end_row) {
				return true;
			}
			if (col <= end_col) {
				return true;
			}
		}
		return false;
	}

	if (row == end_row) {
		if (col <= end_col) {
			return true;
		}
		return false;
	}

	return false;
}

bool
GDTerm::_is_control_tab(Key code) {
	int key = code & godot::KeyModifierMask::KEY_CODE_MASK;
	int ctrl = code & godot::KeyModifierMask::KEY_MASK_CTRL;
	if (ctrl && (key == Key::KEY_TAB)) {
		return true;
	}
	return false;
}

bool
GDTerm::_is_shift_control_tab(Key code) {
	int key = code & godot::KeyModifierMask::KEY_CODE_MASK;
	int ctrl = code & godot::KeyModifierMask::KEY_MASK_CTRL;
	int shift = code & godot::KeyModifierMask::KEY_MASK_SHIFT;
	if (ctrl && shift && (key == Key::KEY_TAB)) {
		return true;
	}
	return false;
}


bool
GDTerm::_is_control_c(Key code) {
	int key = code & godot::KeyModifierMask::KEY_CODE_MASK;
	int ctrl = code & godot::KeyModifierMask::KEY_MASK_CTRL;
	if (ctrl && (key == Key::KEY_C)) {
		return true;
	}
	return false;
}

void 
GDTerm::_do_resize() {
	if (font == nullptr) {
		return;
	}

	_font_space_size = font->get_string_size(" ", godot::HORIZONTAL_ALIGNMENT_LEFT, -1, font_size);

	if (!is_inside_tree()) {
		return;
	}

	Vector2 size = get_size();

	int num_cols = size.x / _font_space_size.x;
	if (num_cols < 2) {
		num_cols = 2;
	}
	int num_rows = size.y / _font_space_size.y;
	if (num_rows < 2) {
		num_rows = 2;
	}

	Vector2 minimum_size = _font_space_size * 2;
	if (_min_size != minimum_size) {
		_min_size = minimum_size;
		emit_signal("minimum_size_changed");
	}

	if ((_rows != num_rows) || (_cols != num_cols)) {
		_rows = num_rows;
		_cols = num_cols;
		_resize_pty();
	}
	queue_redraw();
}

Vector2
GDTerm::_get_minimum_size() const {
	return _min_size;
}

void
GDTerm::_resize_screen_lines() {
	const std::lock_guard<std::mutex> lock(_pending_mutex);
	_pending_state = STATE_SCREEN_RESIZE;
	_screen_lines.resize(_rows); 
	_pending_screen_lines.resize(_rows);
	GDTermLineDirectiveData space;
	space.text = " ";
	GDTermLineDirective space_dir;
	space_dir.kind = DIRECTIVE_WRITE_GLYPH;
	space_dir.data = space;
	for (int i=0; i<_pending_screen_lines.size(); i++) {
		GDTermLine &line = _pending_screen_lines[i];	
		int col = 0;
		auto dir_iter = line.dirs.begin();
		auto dir_end = line.dirs.end();
		for (;dir_iter!=dir_end; ++dir_iter) {
			if (dir_iter->kind == DIRECTIVE_WRITE_GLYPH) {
				col += 1;
				if (col >= _cols) {
					line.dirs.erase(++dir_iter, dir_end);
					break;
				}
			}
		}
		while (col < _cols) {
			line.dirs.push_back(space_dir);
			col += 1;
		}
	}
	_pending_dirty = true;
}

void
GDTerm::_resize_pty() {
	_resize_screen_lines();
	if (_proxy != nullptr) {
		_proxy->resize_screen(_rows, _cols);
	}
}

void
GDTerm::_on_cursor_timeout() {
	_cursor_showing = !_cursor_showing;
	_cursor_blink_count += 1;
	if (_cursor_blink_count >= 20) {
		_cursor_timer->stop();
	}
	queue_redraw();
}

void
GDTerm::_on_blink_timeout() {
	_blink_showing = !_blink_showing;
	queue_redraw();
}

void
GDTerm::_on_inactive() {
	emit_signal("inactive");
	queue_redraw();
}

void
GDTerm::_on_bell_request() {
	emit_signal("bell_request");
}

void
GDTerm::_on_focus_entered() {
	queue_redraw();
}

void
GDTerm::_on_focus_exited() {
	queue_redraw();
}

void
GDTerm::_on_resized() {
	_do_resize();
}

void
GDTerm::_notify_scrollback() {
	emit_signal("scrollback_changed");
}

void
GDTerm::_send_input_chunk(int max_send) {
	PackedByteArray bytes = _send_input_buffer.to_utf8_buffer();
	int amount = bytes.size();
	if (amount == 0) {
		return;
	}

	char buffer[SEND_INPUT_BUFFER_SIZE + 1];
	if (amount > SEND_INPUT_BUFFER_SIZE) {
		amount = SEND_INPUT_BUFFER_SIZE;
	}

	if (amount > max_send) {
		amount = max_send;
	}

	std::memcpy(buffer, bytes.ptr(), amount);
	buffer[amount] = '\0';
	if (_proxy->send_string(buffer) >= 0) {
		_send_input_buffer = _send_input_buffer.substr(amount);
	}
}

void
GDTerm::log_vt_handler_input(unsigned char * data, int data_len) {
	if (_vt_handler_input_log != nullptr) {
		std::string s((char *)data, data_len);
		*_vt_handler_input_log << "VT:'" << s << "':VT" << std::endl;
		_vt_handler_input_log->flush();
	}
}
