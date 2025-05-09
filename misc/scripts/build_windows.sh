export SCONS="scons verbose=yes warnings=no progress=no"
export OPTIONS="use_mingw=yes"

$SCONS platform=windows arch=$1 $OPTIONS target=template_debug || exit 1
$SCONS platform=windows arch=$1 $OPTIONS target=template_release || exit 1

chown $OUTUID:$OUTGID addons/gdterm/bin/libgdterm.windows.template_debug.$1.dll
chown $OUTUID:$OUTGID addons/gdterm/bin/libgdterm.windows.template_release.$1.dll
