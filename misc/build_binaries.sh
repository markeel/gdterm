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
docker build --file build-containers/Dockerfile.base --tag godot-fedora:${img_version} .
docker build --file build-containers/Dockerfile.linux --tag gdterm-linux:${img_version} --build-arg img_version=${img_version} . 
docker run -it --rm -v ${basedir}/misc/scripts:/root/scripts:ro -v ${basedir}/addons/bin:/root/addons/bin -v ${basedir}/godot-cpp:/root/godot-cpp:ro -v ${basedir}/src:/root/src:ro -v ${basedir}/SConstruct:/root/SConstruct:ro -w /root/ gdterm-linux:${img_version} bash scripts/build_linux.sh
