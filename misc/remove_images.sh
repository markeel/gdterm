# 
# This script will clear out the images and build
# cache so that the build containers can be built as
# they would on a fresh VM.
#

basedir=`pwd`
img_version="4.3-f41"
uid=`id -u`
gid=`id -g`

docker image rm gdterm-windows:${img_version} 
docker image rm gdterm-linux:${img_version} 
docker image rm godot-windows:${img_version} 
docker image rm godot-linux:${img_version} 
docker image rm godot-fedora:${img_version} 

docker prune
docker buildx prune
