# GDTerm

GDTerm the Godot In-Editor Terminal

## Description

This project was created to address the needs of developers who are working on their game in the Godot editor 
and also need to perform actions at the command line (like start and stop servers, monitor logs, etc.).  In other words this 
is for those of us who like to keep the Godot Editor in full-screen and not shuffle their windows when needing to do command 
line tasks.

It provides the following features:

* Multiple terminals in main screen area (the same place as the 2D/3D/Script/Assets go).
* Pseudo-terminal interface to default shell
* Emulates an ANSI terminal (16 colors)
* Independent scrollback in each terminal
* Copy and paste in each terminal
* Works fine with typical command line tools: vi, top, tail

The following are its current limitations
* Only provided for Linux and Windows (Mac in the future)

## Getting Started

### Dependencies

* Developed against Godot 4.3-stable
* Godot supported Linux or Windows distribution
* Environment suitable for compiling an extension (if compiling from source)
  * See: https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/gdextension_cpp_example.html

### Installing

Source is only on GitHub at the moment, once it matures a bit, a version for the Asset Lib will be created
that will make it easier to install.

To use the pre-compiled binaries:

* git clone http://github.com/markeel/gdterm
* copy addons directory to the Godot project you need this extension

To compile from source instead of using the pre-compiled library:

* git clone http://github.com/markeel/gdterm
* git submodule update --init --recursive
* cd gdterm
* scons
* copy addons directory to the Godot project you need this extension

### Executing program

* Within your Godot project
* From menubar: Project->Project Settings...
* Click Plugins tab
* Select Enabled checkbox next to "GDTerm"

### Using the terminal

A default instance of a terminal will be available when selecting the "Terminal" button

#### Copy and Paste

The mouse can be used to select text:
* single-click and drag to highlight characters
* double-click and drag to highlight words
* triple-click and drag to highlight lines

The context menu (Right-Click) includes a copy and paste which goes to the system clipboard.

#### New and Close

A new terminal can be created by adding one above, below, left, or right of the window the
context menu is in.  

A terminal can be closed from the context menu as well.

#### Restart

A restart will clear the window and start a new terminal session.  This is also the way to
get a terminal session going again if the shell being used by this terminal has been exited.

## Help

This is a brand new extension, so if you run into problems, create an issue, for general 
questions you can use the Discussions tab.

## Authors

markeel

## Version History

* 0.95
    * Support for Windows

* 0.9
    * Initial Release

## License

This project is licensed under the MIT License - see the LICENSE.md file for details

## Acknowledgments

### Godot

You wouldn't be here if you weren't already using the Godot Game Engine: See [Godot Engine](https://godotengine.org/)

### ANSI Terminal code logic 

The ANSI code interpretation is built using libtmt, but slightly extended to support
scrollback.  See [libtmt](https://github.com/deadpixi/libtmt)

### Fonts

The fonts used are Source Code Pro from [Google Fonts](https://fonts.google.com/specimen/Source+Code+Pro), With license as follows:

Copyright 2010, 2012 Adobe Systems Incorporated (http://www.adobe.com/), with Reserved Font Name 'Source'. All Rights Reserved. Source is a trademark of Adobe Systems Incorporated in the United States and/or other countries.

This Font Software is licensed under the SIL Open Font License, Version 1.1 . This license is available with a FAQ at: https://openfontlicense.org

SIL OPEN FONT LICENSE Version 1.1 - 26 February 2007 

### Sound

The bell sound:

```
Copyright: Dr. Richard Boulanger et al
URL: http://www.archive.org/details/Berklee44v12
License: CC-BY Attribution 3.0 Unported
```
