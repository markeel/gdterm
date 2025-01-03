@tool
extends EditorPlugin

const MainPanel = preload("res://addons/gdterm/terminal/main.tscn")
const InactivePanel = preload("res://addons/gdterm/terminal/inactive.tscn")
const BottomPanel = preload("res://addons/gdterm/terminal/main.tscn")

var main_panel_instance = null
var bottom_panel_instance = null

var current_theme = null
var current_layout = null
var active_theme = null

const THEME_EDITOR : int = 0
const THEME_DARK   : int = 1
const THEME_LIGHT  : int = 2

var theme_property_info = {
	"name": "gdterm/theme",
	"type": TYPE_INT,
	"hint": PROPERTY_HINT_ENUM,
	"hint_string": "editor,dark,light"
}

const LAYOUT_MAIN   : int = 0
const LAYOUT_BOTTOM : int = 1
const LAYOUT_BOTH   : int = 2

var layout_property_info = {
	"name": "gdterm/layout",
	"type": TYPE_INT,
	"hint": PROPERTY_HINT_ENUM,
	"hint_string": "main,bottom,both"
}

func _on_settings_changed():
	var settings = EditorInterface.get_editor_settings()
	
	var theme = THEME_EDITOR
	if settings.has_setting("gdterm/theme"):
		theme = settings.get_setting("gdterm/theme")
	var layout = LAYOUT_MAIN
	if settings.has_setting("gdterm/layout"):
		layout = settings.get_setting("gdterm/layout")
	_apply_theme(theme)
	_apply_layout(layout)

func _apply_theme(theme):
	if current_theme != theme:
		if theme == THEME_EDITOR:
			var editor_theme = Theme.new()
			var settings = EditorInterface.get_editor_settings()
			var background = settings.get_setting("text_editor/theme/highlighting/background_color")
			var foreground = settings.get_setting("text_editor/theme/highlighting/text_color")
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "background", "GDTerm", background)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "foreground", "GDTerm", foreground)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "black", "GDTerm", Color.BLACK)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "white", "GDTerm", Color.WHITE)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "red", "GDTerm", Color.RED)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "green", "GDTerm", Color.GREEN)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "blue", "GDTerm", Color.CORNFLOWER_BLUE)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "yellow", "GDTerm", Color.YELLOW)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "cyan", "GDTerm", Color.CYAN)
			editor_theme.set_theme_item(Theme.DATA_TYPE_COLOR, "magenta", "GDTerm", Color.MAGENTA)
			active_theme = editor_theme
		elif theme == THEME_LIGHT:
			active_theme = preload("res://addons/gdterm/themes/light.tres")
		elif theme == THEME_DARK:
			active_theme = preload("res://addons/gdterm/themes/dark.tres")
		if main_panel_instance != null:
			main_panel_instance.set_theme(active_theme)
			main_panel_instance.theme_changed()
		if bottom_panel_instance != null:
			bottom_panel_instance.set_theme(active_theme)
			bottom_panel_instance.theme_changed()
		current_theme = theme
	
func _apply_layout(layout):
	if current_layout != layout:
		if layout == LAYOUT_MAIN or layout == LAYOUT_BOTH:
			var show_main = false
			if current_layout == LAYOUT_BOTTOM:
				if main_panel_instance != null:
					show_main = main_panel_instance.visible
					EditorInterface.get_editor_main_screen().remove_child(main_panel_instance)
					main_panel_instance.queue_free()
					main_panel_instance = null
				else:
					printerr("Must restart editor using Project->Reload Current Project for Terminal to be displayed in the main editor window")
					return
			if main_panel_instance == null:
				main_panel_instance = MainPanel.instantiate()
				if active_theme != null:
					main_panel_instance.set_theme(active_theme)
				main_panel_instance.visible = show_main
				EditorInterface.get_editor_main_screen().add_child(main_panel_instance)
		if layout == LAYOUT_BOTTOM or layout == LAYOUT_BOTH:
			if bottom_panel_instance == null:
				bottom_panel_instance = BottomPanel.instantiate()
				if active_theme != null:
					bottom_panel_instance.set_theme(active_theme)
					bottom_panel_instance.theme_changed()
				add_control_to_bottom_panel(bottom_panel_instance, "Terminal")
		if layout == LAYOUT_MAIN:
			if bottom_panel_instance != null:
				remove_control_from_bottom_panel(bottom_panel_instance)
				bottom_panel_instance.queue_free()
				bottom_panel_instance = null
		if layout == LAYOUT_BOTTOM:
			if main_panel_instance != null:
				EditorInterface.get_editor_main_screen().remove_child(main_panel_instance)
				main_panel_instance.queue_free()
				main_panel_instance = InactivePanel.instantiate()
				EditorInterface.get_editor_main_screen().add_child(main_panel_instance)
		current_layout = layout

func _enter_tree() -> void:
	var settings = EditorInterface.get_editor_settings()
	
	var theme = THEME_EDITOR
	if settings.has_setting("gdterm/theme"):
		theme = settings.get_setting("gdterm/theme")
	else:
		settings.set_setting("gdterm/theme", theme)
		theme = settings.get_setting("gdterm/theme")
	_apply_theme(theme)
	
	var layout = LAYOUT_MAIN
	if settings.has_setting("gdterm/layout"):
		layout = settings.get_setting("gdterm/layout")
	else:
		settings.set_setting("gdterm/layout", layout)
		layout = settings.get_setting("gdterm/layout")
	_apply_layout(layout)

	# Make sure shows as enum
	settings.add_property_info(theme_property_info)
	settings.add_property_info(layout_property_info)
	settings.settings_changed.connect(_on_settings_changed)
	
	# Hide the main panel. Very much required.
	_make_visible(false)

func _exit_tree() -> void:
	if main_panel_instance:
		main_panel_instance.queue_free()
	if bottom_panel_instance:
		bottom_panel_instance.queue_free()

func _has_main_screen():
	var settings = EditorInterface.get_editor_settings()
	if settings.has_setting("gdterm/layout"):
		var layout = settings.get_setting("gdterm/layout")
		if layout == LAYOUT_MAIN or layout == LAYOUT_BOTH:
			return true
		else:
			return false
	else:
		settings.set_setting("gdterm/layout", 0)
		return true

func _make_visible(visible):
	if main_panel_instance:
		main_panel_instance.visible = visible

func _get_plugin_name():
	return "Terminal"

func _get_plugin_icon():
	return EditorInterface.get_editor_theme().get_icon("Node", "EditorIcons")
