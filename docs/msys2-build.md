# MSYS2

To build ImageCT under Windows, I will use the [MSYS2](https://www.msys2.org/#installation) environment.

## Info

To build ImageCT for ``x86_64`` target system we need to run the UCRT64 or MINGW64 subsystem.

To build ImageCT for ``i686`` target system we need to run the MINGW32 subsystem.

The environment variable ``MINGW_PREFIX`` stores the filesystem prefix of the running subsystem.

The environment variable ``MINGW_PACKAGE_PREFIX`` stores the package prefix for the runing subsystem.

# Requirements

Globally required tools:

- git

## Subsystems requirements

- ``${MINGW_PACKAGE_PREFIX}``-toolchain
    
- ``${MINGW_PACKAGE_PREFIX}``-wxwidgets3.x-msw; with x >= 2

For more info about package management go to MSYS2 docs.

# Build

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch v0.1.0 https://github.com/manpaco/imagect.git

Now, create a build directory:

    cd imagect
    mkdir build & cd build

Run setup process:

    ../setup --msys2 --compilation

Finally run make:

    mingw32-make

