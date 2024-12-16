#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

env.Tool('compilation_db')
env.CompilationDatabase('compile_commands.json')

if env["platform"] == "macos":
    name = "libgdterm.{}.{}.framework/libgdterm.{}.{}".format(env["platform"], env["target"], env["platform"], env["target"])
else:
    name = "libgdterm{}{}".format(env["suffix"], env["SHLIBSUFFIX"])


ptylib = SConscript("src/gdterm/pty/SConscript", variant_dir="build/gdterm/pty", duplicate=0, exports="env")
library = SConscript("src/gdterm/SConscript", variant_dir="build/gdterm", duplicate=0, exports=["env", "name", "ptylib"])
tests = SConscript("test/gdterm/SConscript", variant_dir="build/test", duplicate=0, exports=["env", "ptylib"])

env.Alias('tests', tests)
addon_lib = "addons/gdterm/bin/{}".format(name)
Command(addon_lib, library, Copy('$TARGET', '$SOURCE'))
Default(addon_lib, "compile_commands.json")

