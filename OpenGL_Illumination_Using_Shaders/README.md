# Computer Graphics

The framework provided is compatible with all operating systems.
The recommended C++ IDEs are the following:

* MS Visual Studio 2015 Community edition (Windows)
* MS Visual Studio 2017 Community edition (Windows)
* XCode (Mac)
* Terminal (Mac)
* Visual Studio Code (Any platform)
* Qt Open Source (Any platform)

Detailed installation information can be seen below.

## MS Visual Studio 2015 Community edition (Windows)

MS Visual Studio 2015 can be downloaded from [here](https://my.visualstudio.com/Downloads?q=visual%20studio%202015&wt.mc_id=o~msft~vscom~older-downloads). Make sure you select all packages
related to C++.

After the installation you just need to open the file *visual.sln* located in the *visual* folder.

## MS Visual Studio 2017 Community edition (Windows)

MS Visual Studio 2017 can be downloaded from [here](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=15#).
Make sure you select all packages related to C++.

In addition you need to install *CMake* which can be downloaded from [here](https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-win64-x64.msi).

Once you have all required software go to this file folder and run this on a windows console:

```console
mkdir build
cd build
cmake -G "Visual Studio 15" ../.
```

## XCode (Mac)

You need XCode installed in your system and also Homebrew to install the missing libraries and
*cmake*. To install Homebrew open a terminal and run this command:

```console
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Continue by installing *cmake* and the missing libraries using this command:

```console
brew install cmake
brew install sdl2
```

Finally create the XCode project by going to this file folder and run these commands:

```console
mkdir build && cd build
cmake -G "Xcode" ..
```

The XCode project will be located into the *build* directory.

If this process leads to cmake errors, run this command an try again:

```console
sudo xcode-select --reset
```

## Terminal (Mac)

If using XCode doesnt work, just compile from the terminal, it is less confortable but easier and works everywhere.
First install the tools required:

```console
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install cmake
brew install sdl2
```

We provide a Makefile already created in the "terminal" folder, but if that one doesnt work, you can create one like this:

Go to the folder where you unzipped the code and create a new folder for your project:

```console
mkdir myproject
cd myproject
```

And now once there create a Makefile for Unix using the cmake in the folder above:

```console
cmake -G "Unix Makefiles" ..
```

This will create the Makefile necessary to compile the project, from now on, when you modify the code and want to test it, just invoke "make" to compile, and call the executable (./ComputerGraphics) to execute it:

```console
make 
./ComputerGraphics
```

You can add a shortcut in your text editor to execute this command automatically so you do not have to go to the console every time.


## Visual Studio Code (Any platform)

Remember that **Visual Studio Code is not the same as Visual Studio 2017**, that means the project supplied in the zip **DOESNT WORK WITH VISUAL STUDIO CODE**.

Visual Studio Code was created for developing websites, not C++ application, but it can be used doing some tricks.

Visual Studio Code can be downloaded for each platform in [here](https://code.visualstudio.com/download).

### Requirements on Windows

Windows do not need any further requirement.

### Requirements on Mac

**It is not recommended since there is a bug in one the the needed extensions, that prevents
starting the debugger, use XCode instead.**

Once the ide is installed, on **Mac** it is necessary to first open the application and and press
**Cmd+Shift+P**, then type *"shell command"* and select *"Shell Command: Install 'code' command in
PATH"*. Then it is also necessary to install the following pakcages:

Homebrew to install the missing libraries and
*cmake*. To install Homebrew open a terminal and run this command:

```console
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install cmake
brew install sdl2
```

### Requirements on Linux

If you are using **linux** you have to install the following packages:

```console
sudo apt-get install -y libsdl2-dev libglew-dev freeglut3-dev
```

### Configuring VSCode

After the VSCode and the requirements for each platform are installed, the following extensions
are needed to work with C++ code:

```console
code --install-extension ms-vscode.cpptools
code --install-extension twxs.cmake
code --install-extension vector-of-bool.cmake-tools
code --install-extension austin.code-gnu-global
```

Finally open the folder containing this file.

## Qt Open Source (Any platform)

Qt Open Source can be downloaded from [here](https://www.qt.io/download-qt-installer). Once it is
installed you have to install the same packages that you seen in the previous section. After that
you have to create a new cmake project.


#Execution

## Access controls
After compiling the program a gray screen will appear. Then, depending on which key is pressed (from 1 to 5), one algorithm or another will be used to draw a mesh made of triangles that symbolizes the head of a human person.
The keys to use the different algorithms are as follows:


##### - Key number 1:
Accesses the function that draws all the triangles in the mesh from a function that uses Bresenham's algorithm.

##### - Key number 2:
Allows access to the function that draws the triangles by interpolating three colors.

#### - Key number 3:
Allows access to the function that uses the Z-Buffer to draw the interpolated triangles avoiding occlusions.

#### - Key number 4:
Allows access to the function that draws all the triangles from the Z-Buffer but we color them from the reading of a texture.

### - Key number 5:
The framebuffer is back to black.


The keys to change the view are:

### - F key:
Change the fov. Decrease the fov, so that the figure looks smaller.

### - G key:
Change the fov. Increase the fov, so that the figure looks bigger.

### - A key:
Change the center position. Move the figure to the right.

### - S key:
Change the center position. Move the figure down.

### - D key:
Change the center position. Move the figure to the left.

### - W key:
Change the center position. Move the figure up.