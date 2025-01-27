@tool
extends HBoxContainer

var _restart = false
var _id = 0
var _gd_term_changing = false
var _cmds = null
var _initialized = false

signal bell
signal new_above
signal new_below
signal new_left
signal new_right
signal close

func set_id(p_id : int):
	_id = p_id

func _ready():
	$scrollbar.min_value = 0
	$scrollbar.page = $GDTerm.get_num_screen_lines()
	$scrollbar.max_value = $GDTerm.get_num_scrollback_lines() + $GDTerm.get_num_screen_lines()
	$scrollbar.value = $GDTerm.get_num_scrollback_lines()
	apply_theme()

func apply_cmds(cmds):
	_cmds = cmds

func apply_theme():
	if has_theme_color("background", "GDTerm"):     $GDTerm.background     = get_theme_color("background", "GDTerm")
	if has_theme_color("foreground", "GDTerm"):     $GDTerm.foreground     = get_theme_color("foreground", "GDTerm")
	if has_theme_color("black", "GDTerm"):          $GDTerm.black          = get_theme_color("black", "GDTerm")
	if has_theme_color("red", "GDTerm"):            $GDTerm.red            = get_theme_color("red", "GDTerm")
	if has_theme_color("green", "GDTerm"):          $GDTerm.green          = get_theme_color("green", "GDTerm")
	if has_theme_color("blue", "GDTerm"):           $GDTerm.blue           = get_theme_color("blue", "GDTerm")
	if has_theme_color("cyan", "GDTerm"):           $GDTerm.cyan           = get_theme_color("cyan", "GDTerm")
	if has_theme_color("yellow", "GDTerm"):         $GDTerm.yellow         = get_theme_color("yellow", "GDTerm")
	if has_theme_color("magenta", "GDTerm"):        $GDTerm.magenta        = get_theme_color("magenta", "GDTerm")
	if has_theme_color("white", "GDTerm"):          $GDTerm.white          = get_theme_color("white", "GDTerm")
	if has_theme_color("bright_black", "GDTerm"):   $GDTerm.bright_black   = get_theme_color("bright_black", "GDTerm")
	if has_theme_color("bright_red", "GDTerm"):     $GDTerm.bright_red     = get_theme_color("bright_red", "GDTerm")
	if has_theme_color("bright_green", "GDTerm"):   $GDTerm.bright_green   = get_theme_color("bright_green", "GDTerm")
	if has_theme_color("bright_blue", "GDTerm"):    $GDTerm.bright_blue    = get_theme_color("bright_blue", "GDTerm")
	if has_theme_color("bright_cyan", "GDTerm"):    $GDTerm.bright_cyan    = get_theme_color("bright_cyan", "GDTerm")
	if has_theme_color("bright_yellow", "GDTerm"):  $GDTerm.bright_yellow  = get_theme_color("bright_yellow", "GDTerm")
	if has_theme_color("bright_magenta", "GDTerm"): $GDTerm.bright_magenta = get_theme_color("bright_magenta", "GDTerm")
	if has_theme_color("bright_white", "GDTerm"):   $GDTerm.bright_white   = get_theme_color("bright_white", "GDTerm")

func _gui_input(e : InputEvent):
	var me = e as InputEventMouseButton
	if me != null:
		if me.is_pressed():
			if me.button_index == MOUSE_BUTTON_RIGHT:
				var pos = me.global_position
				$menu.popup(Rect2i(pos.x, pos.y, 0, 0))
			elif me.button_index == MOUSE_BUTTON_WHEEL_UP:
				if $scrollbar.value > 0: 
					$scrollbar.value -= 1
			elif me.button_index == MOUSE_BUTTON_WHEEL_DOWN:
				if $scrollbar.value < $GDTerm.get_num_scrollback_lines(): 
					$scrollbar.value += 1

func _on_gd_term_scrollback_changed() -> void:
	_gd_term_changing = true
	$scrollbar.max_value = $GDTerm.get_num_scrollback_lines() + $GDTerm.get_num_screen_lines()
	$scrollbar.page = $GDTerm.get_num_screen_lines()
	$scrollbar.value = $GDTerm.get_scroll_pos()
	_gd_term_changing = false

func _on_scrollbar_value_changed(value: float) -> void:
	if not _gd_term_changing:
		var row := roundi(value)
		if row != $GDTerm.get_scroll_pos():
			$GDTerm.set_scroll_pos(row)

func _on_gd_term_bell_request() -> void:
	bell.emit()

func _on_gd_term_inactive() -> void:
	if _restart:
		$GDTerm.start()
		_send_initial_cmds()
		_restart = false

func _on_menu_id_pressed(id: int) -> void:
	match id:
		0: _do_copy()
		1: _do_paste()
		2: _do_restart()
		3: new_above.emit()
		4: new_below.emit()
		5: new_left.emit()
		6: new_right.emit()
		8: close.emit()

func _do_copy():
	var text = $GDTerm.get_selected_text()
	DisplayServer.clipboard_set(text)

func _do_paste():
	var text = DisplayServer.clipboard_get()
	text = text.replace("\r\n", "\r")
	if text.length() > 0:
		$GDTerm.send_input(text)

func _send_initial_cmds():
	if $GDTerm.is_active() and !_initialized:
		if _cmds != null and not _cmds.is_empty():
			$GDTerm.send_input(_cmds.replace("\r\n", "\r") + "\r")
			_initialized = true

func _do_restart():
	if $GDTerm.is_active():
		_restart = true
		_initialized = false
		$GDTerm.stop()
	else:
		_initialized = false
		$GDTerm.start()
		_send_initial_cmds()

func _on_visibility_changed() -> void:
	if visible:
		if $GDTerm.is_inside_tree():
			$GDTerm.start()
			_send_initial_cmds()

func _on_gd_term_tree_entered() -> void:
	if visible:
		$GDTerm.start()
		_send_initial_cmds()

func _on_gd_term_paste_request() -> void:
	_do_paste()

func _on_gd_term_copy_request() -> void:
	_do_copy()
