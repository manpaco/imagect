# ImageCT

ImageCT is a easy-to-use, free and open source image cropping tool.

Tool to crop images using an arbitrary shape and, if the crop is outside the image the necessary, pixels will be added to avoid leaving empty areas. The pixels that are added can be chosen by the user: color, image or void.

This project use [wxWidgets](https://www.wxwidgets.org/)

## License

Licensed under [GPL-3.0-or-later](/COPYING).

## Build

### Required dependencies

ImageCT depends on one component:

- [wxWidgets](https://github.com/wxWidgets/wxWidgets) >= v3.2.0

Yout also need some build tools:

- git

- make

- A C++ compiler

- To compile this project on Windows you can use [MSYS2](https://www.msys2.org/).

### Steps

Here, I will show you how to build in Linux. For Windows build go to [MSW guide](/docs/msys2-build.md).

First clone a release branch:

    git clone --branch <tag_name> https://github.com/manpaco/imagect.git

For example:

    git clone --branch v0.1.0 https://github.com/manpaco/imagect.git

This project implement an out-of-source building process, so do mkdir and cd into build directory:

    cd imagect
    mkdir build && cd build

Run setup scripts from build directory with options that you want:

    ../setup [--editorconfig] [--compile_flags] --linux

And finally run make:

    make

## Versioning

This project follows the [Semantic Versioning v2.0.0](https://semver.org/spec/v2.0.0.html) rules.

## Credits

The images used for the demonstrations were taken from the page of their authors.

[Stone photo](https://unsplash.com/photos/u4ijcCaprRc) - Colin Watts

[Computer image](https://github.com/GNOME/adwaita-icon-theme) - GNOME
