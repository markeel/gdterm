#include "key_converter.h"

extern "C" {
#include <libtmt/tmt.h>
#include <libtmt/u8mbtowc.h>
}
#include <string.h>
#include <cassert>

static const char * control_code[] = {
	"\x00",
	"\x01",
	"\x02",
	"\x03",
	"\x04",
	"\x05",
	"\x06",
	"\x07",
	"\x08",
	"\x09",
	"\x0a",
	"\x0b",
	"\x0c",
	"\x0d",
	"\x0e",
	"\x0f",
	"\x10",
	"\x11",
	"\x12",
	"\x13",
	"\x14",
	"\x15",
	"\x16",
	"\x17",
	"\x18",
	"\x19"
};

static const char * lower_case[] = {
	"@",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z"
};

static const char * upper_case[] = {
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z"
};

const char * numbers[] = {
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
};

const char * symbols1[] = {
	" ",
	"!",
	"\"",
	"#",
	"$",
	"%",
	"&",
	"'",
	"(",
	")",
	"*",
	"+",
	",",
	"-",
	".",
	"/",
};

const char * symbols2[] = {
	":",
	";",
	"<",
	"=",
	">",
	"?",
};

const char * symbols3[] = {
	"[",
	"\\",
	"]",
	"_",
	"`",
};

const char * symbols4[] = {
	"{",
	"|",
	"}",
	"~",
};

void
fill_term_string(char * buffer, int buf_len, wchar_t unicode, godot::Key key) {
	assert(buf_len > sizeof(wchar_t));
	buffer[0] = '\0';
	const char * return_val = "";
	int code = key & godot::KeyModifierMask::KEY_CODE_MASK; 
	int ctrl = key & godot::KeyModifierMask::KEY_MASK_CTRL;
	switch (code) {
		case godot::Key::KEY_NONE:
		case godot::Key::KEY_SHIFT:
		case godot::Key::KEY_CTRL:
		case godot::Key::KEY_META:
		case godot::Key::KEY_ALT:
		case godot::Key::KEY_CAPSLOCK:
		case godot::Key::KEY_NUMLOCK:
		case godot::Key::KEY_SCROLLLOCK:
		case godot::Key::KEY_PAUSE:
		case godot::Key::KEY_PRINT:
		case godot::Key::KEY_SYSREQ:
		case godot::Key::KEY_CLEAR:
		case godot::Key::KEY_SPECIAL:
		case godot::Key::KEY_MENU:
		case godot::Key::KEY_HYPER:
		case godot::Key::KEY_HELP:
		case godot::Key::KEY_BACK:
		case godot::Key::KEY_FORWARD:
		case godot::Key::KEY_STOP:
		case godot::Key::KEY_REFRESH:
		case godot::Key::KEY_VOLUMEDOWN:
		case godot::Key::KEY_VOLUMEMUTE:
		case godot::Key::KEY_VOLUMEUP:
		case godot::Key::KEY_MEDIAPLAY:
		case godot::Key::KEY_MEDIASTOP:
		case godot::Key::KEY_MEDIAPREVIOUS:
		case godot::Key::KEY_MEDIANEXT:
		case godot::Key::KEY_MEDIARECORD:
		case godot::Key::KEY_HOMEPAGE:
		case godot::Key::KEY_FAVORITES:
		case godot::Key::KEY_SEARCH:
		case godot::Key::KEY_STANDBY:
		case godot::Key::KEY_OPENURL:
		case godot::Key::KEY_LAUNCHMAIL:
		case godot::Key::KEY_LAUNCHMEDIA:
		case godot::Key::KEY_LAUNCH0:
		case godot::Key::KEY_LAUNCH1:
		case godot::Key::KEY_LAUNCH2:
		case godot::Key::KEY_LAUNCH3:
		case godot::Key::KEY_LAUNCH4:
		case godot::Key::KEY_LAUNCH5:
		case godot::Key::KEY_LAUNCH6:
		case godot::Key::KEY_LAUNCH7:
		case godot::Key::KEY_LAUNCH8:
		case godot::Key::KEY_LAUNCH9:
		case godot::Key::KEY_LAUNCHA:
		case godot::Key::KEY_LAUNCHB:
		case godot::Key::KEY_LAUNCHC:
		case godot::Key::KEY_LAUNCHD:
		case godot::Key::KEY_LAUNCHE:
		case godot::Key::KEY_LAUNCHF:
		case godot::Key::KEY_GLOBE:
		case godot::Key::KEY_KEYBOARD:
		case godot::Key::KEY_JIS_EISU:
		case godot::Key::KEY_JIS_KANA:
		case godot::Key::KEY_UNKNOWN:
		case godot::Key::KEY_F11:
		case godot::Key::KEY_F12:
		case godot::Key::KEY_F13:
		case godot::Key::KEY_F14:
		case godot::Key::KEY_F15:
		case godot::Key::KEY_F16:
		case godot::Key::KEY_F17:
		case godot::Key::KEY_F18:
		case godot::Key::KEY_F19:
		case godot::Key::KEY_F20:
		case godot::Key::KEY_F21:
		case godot::Key::KEY_F22:
		case godot::Key::KEY_F23:
		case godot::Key::KEY_F24:
		case godot::Key::KEY_F25:
		case godot::Key::KEY_F26:
		case godot::Key::KEY_F27:
		case godot::Key::KEY_F28:
		case godot::Key::KEY_F29:
		case godot::Key::KEY_F30:
		case godot::Key::KEY_F31:
		case godot::Key::KEY_F32:
		case godot::Key::KEY_F33:
		case godot::Key::KEY_F34:
		case godot::Key::KEY_F35:
			break;
		case godot::Key::KEY_ESCAPE:
			strcpy(buffer, "\x1b");
			break;
		case godot::Key::KEY_TAB:
			strcpy(buffer, "\t");
			break;
		case godot::Key::KEY_BACKSPACE:
			if (ctrl) {
				strcpy(buffer, TMT_KEY_BACKSPACE);
			} else {
				strcpy(buffer, "\x7f");
			}
			break;
		case godot::Key::KEY_DELETE:
			strcpy(buffer, "\x1b[3~");
			break;
		case godot::Key::KEY_KP_ENTER:
			strcpy(buffer, "\r");
			break;
		case godot::Key::KEY_ENTER:
			strcpy(buffer, "\r");
			break;
		case godot::Key::KEY_INSERT:
			strcpy(buffer, TMT_KEY_INSERT);
			break;
		case godot::Key::KEY_HOME:
			strcpy(buffer, TMT_KEY_HOME);
			break;
		case godot::Key::KEY_END:
			strcpy(buffer, TMT_KEY_END);
			break;
		case godot::Key::KEY_LEFT:
			strcpy(buffer, TMT_KEY_LEFT);
			break;
		case godot::Key::KEY_UP:
			strcpy(buffer, TMT_KEY_UP);
			break;
		case godot::Key::KEY_RIGHT:
			strcpy(buffer, TMT_KEY_RIGHT);
			break;
		case godot::Key::KEY_DOWN:
			strcpy(buffer, TMT_KEY_DOWN);
			break;
		case godot::Key::KEY_PAGEUP:
			strcpy(buffer, TMT_KEY_PAGE_UP);
			break;
		case godot::Key::KEY_PAGEDOWN:
			strcpy(buffer, TMT_KEY_PAGE_DOWN);
			break;
		case godot::Key::KEY_F1:
			strcpy(buffer, TMT_KEY_F1);
			break;
		case godot::Key::KEY_F2:
			strcpy(buffer, TMT_KEY_F2);
			break;
		case godot::Key::KEY_F3:
			strcpy(buffer, TMT_KEY_F3);
			break;
		case godot::Key::KEY_F4:
			strcpy(buffer, TMT_KEY_F4);
			break;
		case godot::Key::KEY_F5:
			strcpy(buffer, TMT_KEY_F5);
			break;
		case godot::Key::KEY_F6:
			strcpy(buffer, TMT_KEY_F6);
			break;
		case godot::Key::KEY_F7:
			strcpy(buffer, TMT_KEY_F7);
			break;
		case godot::Key::KEY_F8:
			strcpy(buffer, TMT_KEY_F8);
			break;
		case godot::Key::KEY_F9:
			strcpy(buffer, TMT_KEY_F9);
			break;
		case godot::Key::KEY_F10:
			strcpy(buffer, TMT_KEY_F10);
			break;
		default:
		{
			if (ctrl && (code >= godot::Key::KEY_AT) && (code <= godot::Key::KEY_UNDERSCORE)) {
				buffer[0] = code - godot::Key::KEY_AT;
				buffer[1] = '\0';
			} else {
				int len = wc_to_utf8(buffer, buf_len, unicode);
				if (len < buf_len) {
					buffer[len] = '\0';
				} else {
					buffer[buf_len-1] = '\0';
				}
			}
		}
	}
}

