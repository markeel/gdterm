export SCONS="scons verbose=yes warnings=no progress=no"
export OPTIONS="production=yes"

export PATH=${GODOT_SDK_LINUX_X86_64}/bin:${BASE_PATH}
$SCONS platform=linux arch=x86_64 $OPTIONS

export PATH=${GODOT_SDK_LINUX_X86_32}/bin:${BASE_PATH}
$SCONS platform=linux arch=x86_32 $OPTIONS

export PATH=${GODOT_SDK_LINUX_ARM64}/bin:${BASE_PATH}
$SCONS platform=linux arch=arm64 $OPTIONS

export PATH=${GODOT_SDK_LINUX_ARM32}/bin:${BASE_PATH}
$SCONS platform=linux arch=arm32 $OPTIONS
