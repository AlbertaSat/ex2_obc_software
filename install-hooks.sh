#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sudo ln -sf $SCRIPT_DIR/.git-hooks/hooks $SCRIPT_DIR/.git/hooks
