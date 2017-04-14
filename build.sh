#!/bin/sh

set -x

buildtype="debug"

for ((i=0; i<=$#; i++))
do
	if  [ $1 == "debug" -o $1 == "release" ]; then
		buildtype=$1
	elif [ $1 == "install" ]; then
		BUILD_INSTALL=install
	elif [ $1 == "-r" ]; then
		shift
		BUILD_SUB_DIR=$1
	fi
	
	shift
done

SOURCE_DIR=`pwd`/$BUILD_SUB_DIR
BUILD_DIR=`pwd`/build
BUILD_TYPE=${BUILD_TYPE:-$buildtype}

#echo $BUILD_TYPE
#echo $BUILD_SUB_DIR

#INSTALL_DIR=${INSTALL_DIR:-./${BUILD_TYPE}-install}
INSTALL_DIR=${INSTALL_DIR:-./}
BUILD_NO_EXAMPLES=${BUILD_NO_EXAMPLES:-0}
#BUILD_DIR=/mnt/hgfs/opensource/lspf/build/

mkdir -p $BUILD_DIR/$BUILD_TYPE/$BUILD_SUB_DIR \
  && cd $BUILD_DIR/$BUILD_TYPE/$BUILD_SUB_DIR \
  && cmake \
		   -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
		   -DCMAKE_LOCAL_PATH=$BUILD_DIR \
           -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
           -DCMAKE_BUILD_NO_EXAMPLES=$BUILD_NO_EXAMPLES \
           $SOURCE_DIR \
  && make $BUILD_INSTALL

# Use the following command to run all the unit tests
# at the dir $BUILD_DIR/$BUILD_TYPE :
# CTEST_OUTPUT_ON_FAILURE=TRUE make test

# cd $SOURCE_DIR && doxygen

