#!/bin/sh
echo "Initializing submodules"
git submodule init 
echo "Updating submodules"
git submodule update
echo "Switching submodules to master"
git submodule foreach 'git checkout master || :'
echo "Making sure submodules are up to date"
git submodule foreach git pull
echo "Installing submodule hooks"
install_hooks="$PWD/install-hooks.sh"
git submodule foreach '$install_hooks || :'
echo "Installing hooks"
$install_hooks
