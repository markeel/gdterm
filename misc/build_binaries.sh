#
# This script is designed to run on Ubuntu latest
# as it would appear in a GitHub workflow
#
# The working directory should be the root of the gdterm
# tree structure, as it would be from the GitHub workflow
#
# The naming for the container images is designed to match
# the godotengine/build-containers project so that those
# container definitions can be re-used as is.
#
basedir=`pwd`
img_version="4.3-f40"
uid=`id -u`
gid=`id -g`
docker build --file build-containers/Dockerfile.base --tag godot-fedora:${img_version} .
docker build --file build-containers/Dockerfile.linux --tag godot-linux:${img_version} --build-arg img_version=${img_version} . 
docker build --file build-containers/Dockerfile.windows --tag godot-windows:${img_version} --build-arg img_version=${img_version} . 

#docker build --file misc/Dockerfile.gdterm --tag gdterm-linux:${img_version} --build-arg gdterm_version="godot-linux:${img_version}" --build-arg uid="${uid}" --build-arg gid="${gid}" . 
#docker run -it --rm -v ${basedir}/addons/gdterm/bin:/root/addons/gdterm/bin -w /root/ gdterm-linux:${img_version} bash scripts/build_linux.sh x86_64
#docker run -it --rm -v ${basedir}/addons/gdterm/bin:/root/addons/gdterm/bin -w /root/ gdterm-linux:${img_version} bash scripts/build_linux.sh arm64
 
docker build --file misc/Dockerfile.gdterm --tag gdterm-windows:${img_version} --build-arg gdterm_version="godot-windows:${img_version}" --build-arg uid=${uid} --build-arg gid=${gid} . 
docker run -it --rm -v ${basedir}/addons/gdterm/bin:/root/addons/gdterm/bin -w /root/ gdterm-windows:${img_version} bash scripts/build_windows.sh x86_64
#docker run -it --rm -v ${basedir}/addons/gdterm/bin:/root/addons/gdterm/bin -w /root/ gdterm-windows:${img_version} bash scripts/build_windows.sh arm64  

