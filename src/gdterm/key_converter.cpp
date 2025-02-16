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
			strcpy_s(buffer, buf_len, "\x1b");
			break;
		case godot::Key::KEY_TAB:
			strcpy_s(buffer, buf_len, "\t");
			break;
		case godot::Key::KEY_BACKSPACE:
			if (ctrl) {
				strcpy_s(buffer, buf_len, TMT_KEY_BACKSPACE);
			} else {
				strcpy_s(buffer, buf_len, "\x7f");
			}
			break;
		case godot::Key::KEY_DELETE:
			strcpy_s(buffer, buf_len, "\x1b[3~");
			break;
		case godot::Key::KEY_KP_ENTER:
			strcpy_s(buffer, buf_len, "\r");
			break;
		case godot::Key::KEY_ENTER:
			strcpy_s(buffer, buf_len, "\r");
			break;
		case godot::Key::KEY_INSERT:
			strcpy_s(buffer, buf_len, TMT_KEY_INSERT);
			break;
		case godot::Key::KEY_HOME:
			strcpy_s(buffer, buf_len, TMT_KEY_HOME);
			break;
		case godot::Key::KEY_END:
			strcpy_s(buffer, buf_len, TMT_KEY_END);
			break;
		case godot::Key::KEY_LEFT:
			strcpy_s(buffer, buf_len, TMT_KEY_LEFT);
			break;
		case godot::Key::KEY_UP:
			strcpy_s(buffer, buf_len, TMT_KEY_UP);
			break;
		case godot::Key::KEY_RIGHT:
			strcpy_s(buffer, buf_len, TMT_KEY_RIGHT);
			break;
		case godot::Key::KEY_DOWN:
			strcpy_s(buffer, buf_len, TMT_KEY_DOWN);
			break;
		case godot::Key::KEY_PAGEUP:
			strcpy_s(buffer, buf_len, TMT_KEY_PAGE_UP);
			break;
		case godot::Key::KEY_PAGEDOWN:
			strcpy_s(buffer, buf_len, TMT_KEY_PAGE_DOWN);
			break;
		case godot::Key::KEY_F1:
			strcpy_s(buffer, buf_len, TMT_KEY_F1);
			break;
		case godot::Key::KEY_F2:
			strcpy_s(buffer, buf_len, TMT_KEY_F2);
			break;
		case godot::Key::KEY_F3:
			strcpy_s(buffer, buf_len, TMT_KEY_F3);
			break;
		case godot::Key::KEY_F4:
			strcpy_s(buffer, buf_len, TMT_KEY_F4);
			break;
		case godot::Key::KEY_F5:
			strcpy_s(buffer, buf_len, TMT_KEY_F5);
			break;
		case godot::Key::KEY_F6:
			strcpy_s(buffer, buf_len, TMT_KEY_F6);
			break;
		case godot::Key::KEY_F7:
			strcpy_s(buffer, buf_len, TMT_KEY_F7);
			break;
		case godot::Key::KEY_F8:
			strcpy_s(buffer, buf_len, TMT_KEY_F8);
			break;
		case godot::Key::KEY_F9:
			strcpy_s(buffer, buf_len, TMT_KEY_F9);
			break;
		case godot::Key::KEY_F10:
			strcpy_s(buffer, buf_len, TMT_KEY_F10);
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

/*
const char * 
lookup_term_string(godot::Key key) {
	const char * return_val = "";
	int code = key & godot::KeyModifierMask::KEY_CODE_MASK; 
	if (code == godot::Key::KEY_TAB) {
		printf("Tab key\n");
	} else {
		printf("NOT Tab key\n");
	}
	int modifiers = key & godot::KeyModifierMask::KEY_MODIFIER_MASK; 
	if ((code >= godot::Key::KEY_AT) && (code <= godot::Key::KEY_UNDERSCORE)) {
		return_val = lower_case[code-godot::Key::KEY_AT];
		if (modifiers & godot::KeyModifierMask::KEY_MASK_CTRL) {
			return_val = control_code[code-godot::Key::KEY_AT];
		}
		if ((code >= godot::Key::KEY_A) && (code <= godot::Key::KEY_Z)) {
			if (modifiers & godot::KeyModifierMask::KEY_MASK_SHIFT) {
				return_val = upper_case[code-godot::Key::KEY_A];
			}
		}
	} else if ((code >= godot::Key::KEY_KP_0) && (code <= godot::Key::KEY_KP_9)) {
		return_val = numbers[code-godot::Key::KEY_KP_0];
	} else if ((code >= godot::Key::KEY_SPACE) && (code <= godot::Key::KEY_SLASH)) {
		return_val = symbols1[code-godot::Key::KEY_SPACE];
	} else if ((code >= godot::Key::KEY_0) && (code <= godot::Key::KEY_9)) {
		return_val = numbers[code-godot::Key::KEY_0];
	} else if ((code >= godot::Key::KEY_COLON) && (code <= godot::Key::KEY_QUESTION)) {
		return_val = symbols2[code-godot::Key::KEY_COLON];
	} else if ((code >= godot::Key::KEY_BRACKETLEFT) && (code <= godot::Key::KEY_QUOTELEFT)) {
		return_val = symbols3[code-godot::Key::KEY_BRACKETLEFT];
	} else if ((code >= godot::Key::KEY_BRACELEFT) && (code <= godot::Key::KEY_ASCIITILDE)) {
		return_val = symbols4[code-godot::Key::KEY_BRACELEFT];
	} else {
		switch (code) {
			case godot::Key::KEY_ESCAPE:
				return "\x1b\x1b";
			case godot::Key::KEY_TAB:
				return "\t";
			case godot::Key::KEY_BACKSPACE:
				return TMT_KEY_BACKSPACE;
			case godot::Key::KEY_ENTER:
			case godot::Key::KEY_KP_ENTER:
				return "\r";
			case godot::Key::KEY_INSERT:
				return TMT_KEY_INSERT;
			case godot::Key::KEY_DELETE:
				return "\x1f";
			case godot::Key::KEY_HOME:
				return TMT_KEY_HOME;
			case godot::Key::KEY_END:
				return TMT_KEY_END;
			case godot::Key::KEY_LEFT:
				return TMT_KEY_LEFT;
			case godot::Key::KEY_UP:
				return TMT_KEY_UP;
			case godot::Key::KEY_RIGHT:
				return TMT_KEY_RIGHT;
			case godot::Key::KEY_DOWN:
				return TMT_KEY_DOWN;
			case godot::Key::KEY_PAGEUP:
				return TMT_KEY_PAGE_UP;
			case godot::Key::KEY_PAGEDOWN:
				return TMT_KEY_PAGE_DOWN;
			case godot::Key::KEY_F1:
				return TMT_KEY_F1;
			case godot::Key::KEY_F2:
				return TMT_KEY_F2;
			case godot::Key::KEY_F3:
				return TMT_KEY_F3;
			case godot::Key::KEY_F4:
				return TMT_KEY_F4;
			case godot::Key::KEY_F5:
				return TMT_KEY_F5;
			case godot::Key::KEY_F6:
				return TMT_KEY_F6;
			case godot::Key::KEY_F7:
				return TMT_KEY_F7;
			case godot::Key::KEY_F8:
				return TMT_KEY_F8;
			case godot::Key::KEY_F9:
				return TMT_KEY_F9;
			case godot::Key::KEY_F10:
				return TMT_KEY_F10;
			case godot::Key::KEY_KP_MULTIPLY:
				return "*";
			case godot::Key::KEY_KP_DIVIDE:
				return "/";
			case godot::Key::KEY_KP_SUBTRACT:
				return "-";
			case godot::Key::KEY_KP_PERIOD:
				return ".";
		}
	}
	
	return return_val;
}
*/
