@tool
extends PanelContainer

var _id : int = 0

func apply_themes():
	_apply_child_themes(self)

func _apply_child_themes(parent):
	for child in parent.get_children():
		if child is VSplitContainer:
			_apply_child_themes(child)
		elif child is HSplitContainer:
			_apply_child_themes(child)
		elif child is AudioStreamPlayer:
			pass
		else:
			child.apply_theme()

func _ready():
	$term.set_id(_next_id())
	$term.bell.connect(_on_bell)
	$term.new_above.connect(_on_new_above.bind($term))
	$term.new_below.connect(_on_new_below.bind($term))
	$term.new_left.connect(_on_new_left.bind($term))
	$term.new_right.connect(_on_new_right.bind($term))
	$term.close.connect(_on_close.bind($term))

func _next_id() -> int:
	_id += 1
	return _id
	
func _on_bell():
	$player.play()

func _on_new_above(node):
	var size = node.get_size()
	var vsplitter = VSplitContainer.new()
	var parent = node.get_parent()
	var child_pos = node.get_index()
	var new_node = preload("res://addons/gdterm/terminal/term.tscn").instantiate()
	new_node.set_id(_next_id())
	new_node.new_above.connect(_on_new_above.bind(new_node))
	new_node.new_below.connect(_on_new_below.bind(new_node))
	new_node.new_left.connect(_on_new_left.bind(new_node))
	new_node.new_right.connect(_on_new_right.bind(new_node))
	new_node.close.connect(_on_close.bind(new_node))
	parent.remove_child(node)
	vsplitter.add_child(new_node)
	vsplitter.add_child(node)
	vsplitter.split_offset = size.y / 2.0
	vsplitter.set_h_size_flags(SIZE_FILL)
	vsplitter.set_v_size_flags(SIZE_FILL)	
	parent.add_child(vsplitter)
	parent.move_child(vsplitter, child_pos)

func _on_new_below(node):
	var size = node.get_size()
	var vsplitter = VSplitContainer.new()
	var parent = node.get_parent()
	var child_pos = node.get_index()
	var new_node = preload("res://addons/gdterm/terminal/term.tscn").instantiate()
	new_node.set_id(_next_id())
	new_node.new_above.connect(_on_new_above.bind(new_node))
	new_node.new_below.connect(_on_new_below.bind(new_node))
	new_node.new_left.connect(_on_new_left.bind(new_node))
	new_node.new_right.connect(_on_new_right.bind(new_node))
	new_node.close.connect(_on_close.bind(new_node))
	parent.remove_child(node)
	vsplitter.add_child(node)
	vsplitter.add_child(new_node)
	vsplitter.split_offset = size.y / 2.0
	vsplitter.set_h_size_flags(SIZE_FILL)
	vsplitter.set_v_size_flags(SIZE_FILL)
	parent.add_child(vsplitter)
	parent.move_child(vsplitter, child_pos)

func _on_new_left(node):
	var size = node.get_size()
	var hsplitter = HSplitContainer.new()
	var parent = node.get_parent()
	var child_pos = node.get_index()
	var new_node = preload("res://addons/gdterm/terminal/term.tscn").instantiate()
	new_node.set_id(_next_id())
	new_node.new_above.connect(_on_new_above.bind(new_node))
	new_node.new_below.connect(_on_new_below.bind(new_node))
	new_node.new_left.connect(_on_new_left.bind(new_node))
	new_node.new_right.connect(_on_new_right.bind(new_node))
	new_node.close.connect(_on_close.bind(new_node))
	parent.remove_child(node)
	hsplitter.add_child(new_node)
	hsplitter.add_child(node)
	hsplitter.split_offset = size.x / 2.0
	hsplitter.set_h_size_flags(SIZE_FILL)
	hsplitter.set_v_size_flags(SIZE_FILL)
	parent.add_child(hsplitter)
	parent.move_child(hsplitter, child_pos)

func _on_new_right(node):
	var size = node.get_size()
	var hsplitter = HSplitContainer.new()
	var parent = node.get_parent()
	var child_pos = node.get_index()
	var new_node = preload("res://addons/gdterm/terminal/term.tscn").instantiate()
	new_node.set_id(_next_id())
	new_node.new_above.connect(_on_new_above.bind(new_node))
	new_node.new_below.connect(_on_new_below.bind(new_node))
	new_node.new_left.connect(_on_new_left.bind(new_node))
	new_node.new_right.connect(_on_new_right.bind(new_node))
	new_node.close.connect(_on_close.bind(new_node))
	parent.remove_child(node)
	hsplitter.add_child(node)
	hsplitter.add_child(new_node)
	hsplitter.split_offset = size.x / 2.0
	hsplitter.set_h_size_flags(SIZE_FILL)
	hsplitter.set_v_size_flags(SIZE_FILL)
	parent.add_child(hsplitter)
	parent.move_child(hsplitter, child_pos)

func _on_close(node):
	var parent = node.get_parent()
	if parent != self:
		var grandparent = parent.get_parent()
		var parent_idx = parent.get_index()
		var child1 = parent.get_child(0)
		var child2 = parent.get_child(1)
		if child1 == node:
			parent.remove_child(child2)
			grandparent.remove_child(parent)
			grandparent.add_child(child2)
			grandparent.move_child(child2, parent_idx)
		elif child2 == node:
			parent.remove_child(child1)
			grandparent.remove_child(parent)
			grandparent.add_child(child1)
			grandparent.move_child(child1, parent_idx)
		parent.queue_free()
