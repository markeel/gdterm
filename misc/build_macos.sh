pip install scons==4.8.1

scons platform=macos target=template_debug || exit 1
scons platform=macos target=template_release || exit 1

