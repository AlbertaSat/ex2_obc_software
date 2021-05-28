# EX2_OBC_SOFTWARE
Welcome 👋 - this is the central repository for the Ex-alta 2 OBC, dubbed 'Athena'. 

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
1. Clone this repository and all submodules:
```git clone --recursive https://github.com/AlbertaSat/ex2_obc_software.git ``` 
then, assuming you wish to work on the latest version of all submodules, run
```git submodule foreach git pull origin master```
Note: Just `cd` into the submodule and change branches from there if you need some branch other than `master`

2. [Download](https://software-dl.ti.com/ccs/esd/documents/ccs_downloads.html) Code Composer Studio v10 

3. Create a new empty workspace folder on your computer (wherever you want, but name it differently than the repository name, so not 'ex2_obc_software')

4. Open Code Composer Studio to your workspace file

5. Import this code as a CCS Project:
	Select `File > Import`
	Select `Code Composer Studio` as your import wizard
	Choose `CCS Projects`
	Then browse the this repository, and select the discovered project
	Select `Finish`
1. You  can now build and debug, and flash this project to a Texas Instruments development board!
