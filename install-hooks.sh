#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ln -s $SCRIPT_DIR/.hooks $SCRIPT_DIR/.git/hooks
