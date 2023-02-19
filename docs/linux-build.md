# Required Dependencies

ImageCT depends on two components:

- [Magick++](https://github.com/ImageMagick/ImageMagick6) 6.x.x

- [wxWidgets](https://github.com/wxWidgets/wxWidgets) >= 3.0.5

Yout also need some tools:

- git

- cmake

- make

- A C++ compiler

# Build

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch v0.1.0 https://github.com/manpaco/imagect.git

Now, create a build directory:

    cd imagect
    mkdir build & cd build

And finally, run CMake:

    cmake ..
    make

For building on [MSW](/docs/msw-build.md)
