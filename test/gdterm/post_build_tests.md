
# Notes for post GitHub build verification

After merging all the source changes that go into the C++ binary builds, the
"Build Binaries" Action is run manually which builds the binaries for all the
different target types and loads them into the addons/gdterm/bin directory.

A final step is to verify everything ran smoothly by bringing down the result
into target systems and running a basic checkout of the plugin

# Install 

Steps to set up a new test project

Get the latest source and compiled binaries
- cd \<github project dir\>
- git pull

Create a new Godot project
- Start Godot Editor
- Create... a new project

Copy addons folder into new project
- copy \<github project dir\>/addons/gdterm to \<New Godot Project\>

Enable the plugin
- From within Editor for \<New Godot Project\>
- Select Project -> Project Settings...
- Click on Plugins tab
- Click on checkbox for gdterm plugin

# Test

## Basic usage

### Linux

- ls
- vi x.dat
- Paste a sample of unicode characters
  - Copyright ©
  - Up arrow: Ʌ
  - Right pointing triangle: ▷ 
  - 🕹️Alarm clock: 
