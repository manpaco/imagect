# Required Dependencies

ImageCT depends on two components:

- [Magick++](https://github.com/ImageMagick/ImageMagick6) >= 6.9.11, <= 6.9.12

- [wxWidgets](https://github.com/wxWidgets/wxWidgets) >= 3.0.5, <= 3.2

Yout also need some tools:

- git

- cmake

- make

- A C++ compiler

# Building

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch 0.1.0 https://github.com/manpaco/imagect.git

Now, create a build directory and run CMake:

    cd imagect
    mkdir build & cd build
    cmake ..
    make

For building on [MSW](/docs/msw-build.md)
