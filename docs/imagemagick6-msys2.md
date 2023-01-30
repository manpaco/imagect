# Compilation of Legacy ImageMagick on MSYS2

The ImageMagick repository provides a compilation guide, but under MSYS2 differs a bit.

The environment variable ``MINGW_PREFIX`` stores the filesystem prefix of the running subsystem.

## Steps

Download a tarball from Legacy ImageMagick repository and unpack it:

    tar -xvf ImageMagick6-6.9.12-70.tar.gz

Then, create a out-of-source build directory:

    mkdir im-build & cd im-build

Export the following flags and lib:

    export LIBS="-lurlmon"

    export CPPFLAGS="-I${MINGW_PREFIX}/include -D__USE_MINGW_ANSI_STDIO=1"

    export LDFLAGS="-L${MINGW_PREFIX}/lib"

Configure with parameters:

    ../ImageMagick6-6.9.12-70/configure "--prefix=${MINGW_PREFIX}" '--enable-shared' '--with-modules' '--disable-static' '--disable-deprecated' '--enable-hdri=no' '--with-fftw' '--with-gslib' '--with-gvc' '--with-flif' '--with-lcms' '--with-lqr' '--with-openexr' '--with-openjp2' '--with-rsvg' '--with-webp' '--with-wmf' '--with-xml' '--without-autotrace' '--without-dps' '--without-fpx' '--with-jbig' '--without-perl' '--without-x' '--with-raqm' '--with-magick-plus-plus'

Finally, do:

    make

    make install

## Test installation

To test the installation run:
    
    identify example.jpg

You should get this output message:

    example.jpg JPEG 2400x1600 2400x1600+0+0 8-bit sRGB 200286B 0.00u 0:00.001

To show build information:

    identify -list Configure

