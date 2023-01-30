# Requirements

To build ImageCT under Windows, I will use the [MSYS2](https://www.msys2.org/#installation) environment.

Globally required tools:

- git

- make

# Building for ``x86_64`` and ``i686``

To build ImageCT for ``x86_64`` target system we need to run the UCRT64/MINGW64 subsystem of MSYS2.

To build ImageCT for ``i686`` target system we need to run the MINGW32 subsystem of MSYS2.

The environment variable ``MINGW_PACKAGE_PREFIX`` stores the package prefix for the runing subsystem.

## Required Dependencies

- ``${MINGW_PACKAGE_PREFIX}``-toolchain
    
- ``${MINGW_PACKAGE_PREFIX}``-wxwidgets3.x-msw; with x = 0, 1, 2

- ``${MINGW_PACKAGE_PREFIX}``-cmake

ImageCT require Legacy Magick++ (v6.x.x), but MSYS2 come with ImageMagick7 (v7.x.x). Therefore, we must to compile from source:

- Own [compilation of Legacy Magick++](/docs/imagemagick6-msys2.md) (v6.x.x)

## Build

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch 0.1.0 https://github.com/manpaco/imagect.git

Now, create a build directory:

    cd imagect
    mkdir build & cd build

And finally, run CMake with Generator parameter:

    cmake -G "MinGW Makefiles" ..
    make

For building on [Linux](/docs/linux-build.md)
