## SmartHeadTracker

A work in progress open source head tracker based on computer vision with a normal web cam or a PS3 Eye.
Initially it will use a marker placed on a microphone headset for more accurate tracking.

##Building

CMake is required to build SmartHeadTracker.

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
However the PS3 Eye driver is released under a combination MIT/GPLv2 license because of its use of Linux kernel code.
