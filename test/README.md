# EX2_OBC_SOFTWARE Testing Directory
This is the Testing directory for Ex-alta 2 OBC software. 

The software in this directory uses an external library package called cgreen to operate. In order to run the code follow the steps below.

### Setting up Cgreen
One can find the Cgreen repository at https://github.com/cgreen-devs/cgreen.

1. Simply git clone the cgreen repository to wherever you want the directory to go, note that it is reccomended to keep the cgreen library outside of the repository being tested (ie. not in the ex2_obc_software repository)

2. Note that having CMake and a compiler (gcc reccomended as per Cgreen documents) is a prerequisite for this: 
	Once cloned, navigate to the cgreen root directory (a makefile should be there), and then run "make" in the terminal. A "build" subdirectory should be created.

3. Cgreen is now ready for use. Navigate to the testing directory (where this README should be) and open the makefile in an editor.
	Where LCGREEN is defined (under the User specific manually set variables section) set the value to be the root file path to the cgreen root directory (ie. /home/usr/path/to/cgreen  --make sure to not put a '/' after cgreen)

The Makefile is now ready to use Cgreen. Currently the tests have been organized in the makefile into seperate subsections in the "Test Dependant Files" section of the makefile. This includes the path to the test.c file, actual.c file, and any libraries specific to that test.
