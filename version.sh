#!/bin/bash

# get the current branch
branch=`git rev-parse --abbrev-ref HEAD`
# get the most recent tag
tag=`git describe --tags --abbrev=0`

cat <<EOT > main/version.h
#ifndef VERSION_H
#define VERSION_H

static const char *ex2_version = "$branch: $tag";

#endif //VERSION_H
EOT

exit 0
