@tool
extends PanelContainer

func theme_changed():
	$term_container.apply_themes()

func _on_theme_changed():
	theme_changed()
