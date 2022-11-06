# Image Cropping Tool

Herramienta para recortar imagenes agregando la posibilidad de hacer "growing". Esto significa que, si el recorte queda por fuera de la imagen, la herramienta agregará los pixeles necesarios para no dejar el area vacía. Los pixeles que se agregan pueden ser elegidos por el usuario entre: "color" o "imagen".<br>

## Build

	mkdir build
	cd build
	cmake ..
	cmake --build .

## Funcionalidades importantes

A continuacion se presentan las funcionalidades que deben ser reflejadas en el programa final:<br>

- Elección de la relacion de aspecto: Libre o fija.
- Forma de recorte: Cuadrilatero, circulo, triangulo, forma, etc.
- Dibujar contorno del area de recorte. Permitir modificar su color, grosor y posicion (interior, exterior o en el medio).
- Permitir growing y elegir su relleno (color o imagen). Si es una imagen debe existir un control de desenfoque o "blur".

## Diseño de GUI

La interfaz grafica tendrá una barra de menu superior, panel de canvas a la izquierda y panel de controles junto con la vista previa a la derecha.<br>

![gui](/images/screenshot.png)
