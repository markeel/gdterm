export SCONS="scons"
export OPTIONS=""

if [ "$1" == "x86_64" ]
then export PATH=${GODOT_SDK_LINUX_X86_64}/bin:${BASE_PATH}
fi

if [ "$1" == "x86_32" ]
then export PATH=${GODOT_SDK_LINUX_X86_32}/bin:${BASE_PATH}
fi

if [ "$1" == "arm64" ]
then export PATH=${GODOT_SDK_LINUX_ARM64}/bin:${BASE_PATH}
fi

if [ "$1" == "arm32" ]
then export PATH=${GODOT_SDK_LINUX_ARM32}/bin:${BASE_PATH}
fi

$SCONS platform=linux arch=$1 $OPTIONS target=template_debug
$SCONS platform=linux arch=$1 $OPTIONS target=template_release

chown $OUTUID:$OUTGID addons/gdterm/bin/libgdterm.linux.template_debug.$1.so
chown $OUTUID:$OUTGID addons/gdterm/bin/libgdterm.linux.template_release.$1.so
