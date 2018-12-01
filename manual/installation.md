Installing PlotFX
======================

PlotFX is written in C++. You can either install a binary release or compile
from source. Once the installation is complete, you will have a `plotfx` binary
on your system.

You can find the complete [source code on Github](http://github.com/plotfx/plotfx)

### Compile from source

You need autotools and a modern c++ compiler. To compile fnordmetric from a git
checkout run:

    $ git clone https://github.com/paulasmuth/fnordmetric.git && cd fnordmetric
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install


