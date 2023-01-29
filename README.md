# Image Cropping Tool

![logo](/images/ict-logo.svg)

Herramienta para recortar imagenes agregando la posibilidad de hacer "growing". Esto significa que, si el recorte queda por fuera de la imagen, la herramienta agregará los pixeles necesarios para no dejar areas vacías. Los pixeles que se agregan pueden ser elegidos por el usuario: "color" o "imagen".

## License

Licensed under [GPL-3.0-or-later](/docs/COPYING).

## Build

For building this project, please see platform-specific documentation. For [Linux](/docs/linux-build.txt) or [MSW](/docs/msw-build.txt).

## Features

A continuacion se presentan las funcionalidades que deben ser reflejadas en el programa final:

- Elección de la relacion de aspecto: Libre o fija.
- Forma de recorte: Cuadrilatero, circulo, triangulo, forma, etc.
- Dibujar contorno del area de recorte. Permitir modificar su color, grosor y posicion (interior, exterior o en el medio).
- Permitir growing y elegir su relleno (color o imagen). Si es una imagen debe existir un control de desenfoque o "blur".

## Versioning

This project follows the [Semantic Versioning](https://semver.org/) rules.
