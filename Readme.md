## SmartHeadTracker

An open source head tracker based on computer vision a PS3 Eye.
Really this is just a generic tracker for pink circles, but it is intended for head tracking by placing the marker on a microphone headset.
Pink circles are tracked first by colour and then by fitting an ellipse to it to
get a sub-pixel accurate center. The Halide computer vision language is used to accelerate some code so it the system can run at 60FPS.
Currently the accuracy is more than enough for using this to control a mouse, one of my as yet unreleased research projects can use this system to control a mouse quite accurately and even at high head movement speeds.

The program spits out JSON messages containing the x and y coordinates of the marker over [ZeroMQ](http://zeromq.org/).
The messages also include coordinates filtered with a light [one euro filter](http://www.lifl.fr/~casiez/1euro/) for even lower jitter.

![Screenshot](http://i.imgur.com/5QphqMi.png)

Currently it is only hooked up for using the PS3 Eye driver because of the need for 60FPS tracking, but the code could trivially be modified to work with normal webcams.

##Building

CMake and Halide are required to build SmartHeadTracker. You will also need ensure you clone the PS3EyeDriver git submodule.

###OSX or Linux with Make
```bash
# do things in the build directory so that we don't clog up the main directory
mkdir build
cd build
cmake ../
make
```

###On OSX with XCode
```bash
mkdir build
./cmakeBuild.sh
```
then open the XCode project in the build folder and run from there.

###On Windows
There is some way to use CMake on Windows but I am not familiar with it.

## License

This software is licensed under the MIT license (see the `LICENSE` file).
However the `PS3EYEDriver` submodule is released under a combination MIT/GPLv2 license because of its use of Linux kernel code.
