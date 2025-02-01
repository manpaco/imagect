# imagect

imagect is an easy-to-use, free and open source image cropping tool.

![imagect imagotype](/resources/imagect-logo.svg)

Tool to crop images using a shape of your choice. You can add custom pixels to avoid leaving empty areas.

This project use [wxWidgets](https://www.wxwidgets.org/)

## License

Licensed under [GPL-3.0-or-later](/COPYING).

## Build

Here, I will show you how to build in Linux. For Windows build go to [MSW guide](/docs/msys2-build.md).

### Required dependencies

- [wxWidgets](https://github.com/wxWidgets/wxWidgets) >= v3.2.0

You also need some build tools:

- git

- cmake

- C++ compiler (at least C++11 support)

### Steps

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch v0.1.0 https://github.com/manpaco/imagect.git

This project implement an out-of-source building process, so do mkdir and cd into build directory:

    cd imagect
    mkdir build && cd build

Run cmake from build directory with options that you want:

    cmake .. -DCMAKE_BUILD_TYPE=Debug

And finally run make:

    make

## Versioning

This project follows the [Semantic Versioning v2.0.0](https://semver.org/spec/v2.0.0.html) rules.

## Credits

The images used for the demonstrations were taken from the page of their authors.

[Stone photo](https://unsplash.com/photos/u4ijcCaprRc) - Colin Watts

[Computer image](https://github.com/GNOME/adwaita-icon-theme) - GNOME
