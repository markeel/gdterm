#!/usr/bin/env python
import os
import sys

env = SConscript("godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/gdterm", "src/gdterm/thirdparty"])
env.Tool('compilation_db')
env.CompilationDatabase('compile_commands.json')

sources = Glob("src/gdterm/*.cpp")
sources.extend(Glob("src/gdterm/thirdparty/libtmt/*.c"))

if env["platform"] == "macos":
    library = env.SharedLibrary("addons/gdterm/bin/libgdterm.{}.{}.framework/libgdterm.{}.{}".format(
            env["platform"], env["target"], env["platform"], env["target"]
        ),
        source=sources,
    )
elif env["platform"] == "ios":
    if env["ios_simulator"]:
        library = env.StaticLibrary("addons/gdterm/bin/libgdterm.{}.{}.simulator.a".format(env["platform"], env["target"]),
            source=sources,
        )
    else:
        library = env.StaticLibrary("addons/gdterm/bin/libgdterm.{}.{}.a".format(env["platform"], env["target"]),
            source=sources,
        )
else:
    library = env.SharedLibrary("addons/gdterm/bin/libgdterm{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
        source=sources,
    )

ptytest_source=Glob("src/gdterm/thirdparty/libtmt/*.c")
ptytest_source.extend(Glob("src/gdterm/*.cpp"))
ptytest_source.extend(Glob("test/gdterm/ptytest2.cpp"))

ptytest = env.Program(target="test/gdterm/ptytest2", source=ptytest_source)

Default(library, "compile_commands.json", ptytest)

