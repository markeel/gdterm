#ifndef KEY_CONVERTER_H
#define KEY_CONVERTER_H

#include "godot_cpp/classes/global_constants.hpp"

//const char * lookup_term_string(godot::Key key);
void fill_term_string(char * buffer, int buf_len, wchar_t unicode, godot::Key key);

#endif
