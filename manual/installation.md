Installation
============

PlotFX is written in C++. You can either install a binary release or compile
from source. Once the installation is complete, you will have a `plotfx` binary
on your system.

You can find the complete [source code on Github](http://github.com/plotfx/plotfx)

### Compile from source

To build PlotFX, you need an up-to-date C++ compiler, cmake, libharfbuzz and
libfreetype. Run:

    $ git clone git@github.com:plotfx/plotfx.git
    $ cd plotfx
    $ cmake .
    $ make

To install the `plotfx` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test

