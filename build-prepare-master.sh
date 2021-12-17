#!/bin/bash
set -e -o pipefail

BUILD_ROOT=${BUILD_ROOT:-"$HOME/AlbertaSat/builds"}
BUILD_TOOL_ROOT=${BUILD_TOOL_ROOT:-"$HOME/ti/ccs1040"}
BUILD_WORK_SPACE=${BUILD_WORK_SPACE:-"../workspace"}

BUILD_REPOSITORY=${BUILD_REPOSITORY:-"ex2_obc_software"}

now=`date +"%Y-%m-%d.%H-%M-%S"`

echo "Preparing repository ${BUILD_REPOSITORY} at time: $now"

build_dir="${BUILD_ROOT}/build-$now"
mkdir -p $build_dir
cd $build_dir

git clone "git@github.com:AlbertaSat/${BUILD_REPOSITORY}.git"

cd ${BUILD_REPOSITORY}
echo "Configuring repository in $PWD"
./configure_repo.sh

echo "Importing Project"
$TOOL_ROOT/ccs/eclipse/eclipse -noSplash -data "$BUILD_WORK_SPACE" -application com.ti.ccstudio.apps.importProject -ccs.location "."

echo "Ready to build"
exit 0



