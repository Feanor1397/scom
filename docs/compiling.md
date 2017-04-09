---
layout: default
title: Compiling from source
permalink: /compiling/
---
## Download source code
Firstly you need obtain source code. You can download zip and tar ball from
site or clone full project repository.

    git clone git://github.com/Feanor1397/scom.git
    cd scom

Next you need create separate building directory.

    mkdir build
    cd build

Now use this bunch of commands:

    cmake .. -DCMAKE_INSTALL_PREFIX=/usr \
             -DCMAKE_BUILD_TYPE=Release
    make -j3

Right now you can test application by running `./scom`.
If you want install it than use prefered method for your package manager
(e.g. makepkg for Arch, or checkinstall for Debian based distros)
If you don't want use package manager than simply run

    sudo make install

[Back to project page](/scom/)
