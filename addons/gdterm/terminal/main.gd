@tool
extends MarginContainer

func theme_changed():
	$term_container.apply_themes()

func set_initial_cmds(cmds):
	$term_container.set_initial_cmds(cmds)

func set_alt_meta(setting):
	$term_container.set_alt_meta(setting)

func _on_theme_changed():
	theme_changed()
