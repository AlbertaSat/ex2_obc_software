# EX2_OBC_SOFTWARE  [![CircleCI](https://circleci.com/gh/AlbertaSat/ex2_obc_software.svg?style=svg)](https://circleci.com/gh/AlbertaSat/ex2_obc_software)
Welcome 👋 - this is the central repository for the Ex-Alta 2 OBC, dubbed 'Athena'. 

The software in this repository reflects the designs found in the Ex-Alta 2 Critical Design Review document, and separate folders contain software submodules for the Equipment Handlers (EH), Hardware Interface (HIL) and On-board services.

### File Structure
* ex2_hal/
	* Software submodules for connected hardware devices. These submodules are responsible for implementing the hardware Equiment Handlers, and Hardware Interface Layer that will be used in the higher layers.
* ex2_services/
	* Software defining the CSP listeners and service handlers that expose the satellite's functionality to the ground user.
* ex2_system/
	* Collection of background tasks that do work during the satellite's operation
* libcsp/
	* Submodule for the for the CSP network software.
* main/
	* Main entry point, and LEOP sequence. All background tasks and third-part systems (i.e. Reliance Edge, FreeRTOS Sceduler, and CSP node) are initialized here.
* source/
	* HalCoGEN generated source files including hardware drivers, and configurations

## Getting Started
1. Set up an SSH key with GitHub. [Instructions](https://docs.github.com/en/github/authenticating-to-github/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account)

2. Clone this repository and run the auto configure script:
```./configre_repo.sh```
Note: Just `cd` into the submodule and change branches from there if you need some branch other than `master`
Note: You must have git version 2.23 or greater in order for the git switch command to work. Check your version using ```git --version```

3. [Download](https://software-dl.ti.com/ccs/esd/documents/ccs_downloads.html) Code Composer Studio v10 

4. Create a new empty workspace folder on your computer (wherever you want, but name it differently than the repository name, so not 'ex2_obc_software')

5. Open Code Composer Studio to your workspace file

6. Import this code as a CCS Project:
	Select `File > Import`
	Select `Code Composer Studio` as your import wizard
	Choose `CCS Projects`
	Then browse the this repository, and select the discovered project
	Select `Finish`

7. You  can now build and debug, and flash this project to a Texas Instruments development board!
