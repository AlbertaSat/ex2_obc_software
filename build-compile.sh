#!/bin/bash
set -e

# You can pass the Code Composer Studio install path as an environment variable
BUILD_TOOL_ROOT=${BUILD_TOOL_ROOT:-"$HOME/ti/ccs1040"}

# Builds the ex2_obc_software project using the specified configuration (the
# active configuration if omitted) and workspace.
usage() {
    echo "Usage: $0 [-c <configuration> ] [-w <workspace>]"
    exit 1
}

# Default workspace is from environment variable or ..
workspace=${BUILD_WORKSPACE:="../workspace"}
configuration=""
while getopts ":c:w:" o; do
    case "${o}" in
        c)
            configuration="-ccs.configuration ${OPTARG}"
            ;;
        w)
            workspace=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done

echo "Building $configuration from workspace $workspace"
${BUILD_TOOL_ROOT}/ccs/eclipse/eclipse -noSplash -data "$workspace" -application com.ti.ccstudio.apps.buildProject -ccs.projects ex2_obc_software "$configuration" -ccs.listErrors

exit 0


