@tool
extends MarginContainer

func theme_changed():
	$term_container.apply_themes()

func _on_theme_changed():
	theme_changed()
