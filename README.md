# ImageCT

ImageCT is a easy-to-use, free and open source image cropping tool.

![logo](/images/ict-logo.svg)

Tool to crop images adding the possibility of "growing". This means that if the crop is outside the image, the necessary pixels will be added to avoid leaving empty areas. The pixels that are added can be chosen by the user: color, image or void.

This project use [Legacy ImageMagick](https://legacy.imagemagick.org/) and [wxWidgets](https://www.wxwidgets.org/)

## Features

### Aspect

![aspect-block](/images/aspect-block.png)

Enable or disable the fix ratio.

Width and height setters.

### Shape

![shape-block](/images/shape-block.png)

Crop shape selector.

Stroke width setter.

Stroke color selector.

### Growing

![grow-block](/images/grow-block.png)

Fill pixel selector: Color, Image or Void.

Color selector.

Image selector and blur slider.

### Zoom

![zoom-control](/images/zoom-control.png)

Zoom-out and Zoom-in buttons.

Display box to show current zoom value.

### Preview

![preview-panel](/images/preview-panel.png)

To show the cropping preview.

### Canvas

![canvas-panel](/images/canvas-panel.png)

Contains the opened image and a draggable/resizable crop rectangle.

## License

Licensed under [GPL-3.0-or-later](/docs/COPYING).

## Build

For building this project, please see platform-specific documentation. For [Linux](/docs/linux-build.md) or [MSW](/docs/msw-build.md).

To compile this project on Windows you can use [MSYS2](https://www.msys2.org/).

## Versioning

This project follows the [Semantic Versioning v2.0.0](https://semver.org/spec/v2.0.0.html) rules.

## Credits

The images used for the demonstrations were taken from the page of their authors.

[Stone photo](https://unsplash.com/photos/u4ijcCaprRc) - Colin Watts

[Computer image](https://github.com/GNOME/adwaita-icon-theme) - GNOME
