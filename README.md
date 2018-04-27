# MADP-TreeVis

Developed as part of a Final Year Computer Science project at the University of Liverpool, MADP-TreeVis aims to provide an easy, intuitive and interactive way to visualise policies for the [MADP Toolbox](https://github.com/MADPToolbox/MADP). It does this with two primary features, the 'Full Tree Viewer' which allows the policies to be viewed as rooted trees in their entirety and the 'Policy Visualiser View' that acts as a way to visualise a path through a policy by providing the observations to the agents. It provides a built in interface in order to plan directly from the program in addition to other features include saving the policies to a svg file, reading in a policy outputted from a previous plan made by the toolbox and customisation of the visualisations.

It has been developed using the Qt framework and is written in C++.

## Using MADP-TreeVis

There are two options, either compile the program and its dependencies yourself, or use the prebuilt version (only tested on Ubuntu 16.04 and 18.04).

### Compiling sources
#### Dependencies

* Qt (>= 5.9.2) - used for the GUI (install pointers below) 
* MADP (note: also the version of boost that ships with MADP)

Everything else *should* be included with your Linux distro or via the install with MADP.

#### Compile MADP-TreeVis

For both options you will need to change the boost src directory in the MADP-TreeVis.pro file to the one included with MADP.

##### Ubuntu 16.04

In order to compile the program yourself you may need to compile Qt first. If your system has a version of Qt >=5.9.2 then you can just use that, if not you must compile it yourself. There is an install_qt script that will download and compile Qt version 5.9.2 in the repository, just run with `./install_qt.sh /path/to/install/to` (for example ~/Qt_5.9.2). Note: it will also install some dependencies for needed for Qt (you can look at these in the script).

1. Grab a copy of the repo and cd into the root directory
2. Set PATH to point to the place where you installed Qt, in the example given above just run `export PATH=~/Qt_5.9.2/bin:$PATH`
3. run `qmake`
4. run `make -j4`

This should compile the program to ./bin/MADP-TreeVis

##### Ubuntu 18.04

You can just install the Qt sources from apt in order to build as they are a recent enough version -

sudo apt install qt5-default libqt5svg*

Then simply run

1. `qmake`
2. `make -j4`

This should compile the program to ./bin/MADP-TreeVis

### Use Pre Built Version

Note: this has only been tested on Ubuntu 16.04 and 18.04. Things may not work on other distributions!

I have provided as a convenience a compiled a version and bundled the neccessary files with it (i.e. the Qt libs to dynamically link to and their dependencies) and any other dependencies *except* MADP which is presumed to be installed on your system.

To use it simply

1. Download the archive from [the release page](https://github.com/roberthand9/MADP-TreeVis/releases).
2. Extract the archive
3. Run ./start.sh (needed to set LD_LIBRARY_PATH)
