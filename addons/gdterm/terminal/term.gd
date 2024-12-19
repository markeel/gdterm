@tool
extends HBoxContainer

var _restart = false
var _id = 0
var _gd_term_changing = false

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
	$scrollbar.value = $GDTerm.get_scroll_pos()
	$scrollbar.page = $GDTerm.get_num_screen_lines()
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
	if text.length() > 0:
		$GDTerm.send_input(text)

func _do_restart():
	if $GDTerm.is_active():
		_restart = true
		$GDTerm.stop()
	else:
		$GDTerm.start()

func _on_visibility_changed() -> void:
	if visible:
		if $GDTerm.is_inside_tree():
			$GDTerm.start()

func _on_gd_term_tree_entered() -> void:
	if visible:
		$GDTerm.start()
