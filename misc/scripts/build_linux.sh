export SCONS="scons verbose=yes"
export OPTIONS=""

echo "********** $1 *********************"
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

echo "********** $PATH *********************"
$SCONS platform=linux arch=$1 $OPTIONS
