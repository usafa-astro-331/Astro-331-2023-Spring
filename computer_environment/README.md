# Getting started

You will need to install several programs. This folder contains programs and batch scripts to setup the correct computing environment. These tools are setup for Windows. Equivalent tools exist for Mac or Linux, but you'll have to install your own. 

If you are using the Astro 331 lab computers, git is already installed and the course repository is already cloned. 



## Install git

If you are just getting started, copy `install_git.bat` to your local machine from https://github.com/usafa-astro-331/Astro-331-2023-Spring/blob/main/computer_environment/install_git.bat

Verify file contents using a text editor. You should do this before running any script. This script runs two commands using Windows' built-in command `winget` to install `git` and `tortoisegit`.  Complete setup of both programs. 

- **git**: version control software

- **TortoiseGit**: windows integration for git



## Clone the course repository

Copy the path to the course repository: https://github.com/usafa-astro-331/Astro-331-2023-Spring

Go to the location on your computer where you want to place the folder for all labs, for example, My Documents. Do not create a new course folder yet. 

Right click -> Git clone

Paste the course URL and click OK. 



## Update the course

This step is not necessary if you just cloned the course repository. 

**This step should be performed before every lab to ensure you're using updated material.** 

- Right click the course folder (or any location within the course folder). 
- Git Sync -> Pull



## Install editors

Go to the `computer_environment` folder. 

- Verify and run `install_editors.bat` to install editors

  - **typedown**: a markdown editor/viewer that you will use to view lab instructions
  
  
    - **notepad++**: a useful text editor
    - **Thonny**: IDE used for communication lab
  
  - **Arduino IDE**
  
- Verify and run `update_arduino.bat` which updates the Arduino IDE/environment using the `arduino-cli` tool

  - updates Arduino environment

  - installs drivers for newer SAMD-based Arduino boards such as Arduino MKR Zero




# Before each lab

- Pull latest changes from course repository
  - Right click the course folder (or any location within the course folder). 
  - Git Sync -> Pull
- Update Arduino environment using `install_libraries.bat` found in that lab's folder
  - each lab requires different drivers--`install_libraries.bat` is different in each folder

