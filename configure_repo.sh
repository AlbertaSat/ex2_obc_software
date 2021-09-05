#!/bin/sh
echo "Switching submodules to master"
git submodule foreach 'git switch master || :'
echo "Making sure submodules are up to date"
git submodule foreach git pull
echo "Installing submodule hooks"
git submodule foreach './install-hooks.sh || :'
echo "Installing hooks"
./install-hooks.sh
