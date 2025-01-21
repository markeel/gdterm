@tool
extends PanelContainer

var command = ""

func _enter_tree() -> void:
	$term_container/term/GDTerm.command = command

func theme_changed():
	$term_container.apply_themes()

func _on_theme_changed():
	theme_changed()
