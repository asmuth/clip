Installation
============

fviz is written in C++. You can either install a binary release or compile
from source. Once the installation is complete, you will have a `fviz` binary
on your system.

You can find the complete [source code on Github](http://github.com/fviz/fviz)

### Compile from source

To build fviz, you need an up-to-date C++ compiler, cmake, libharfbuzz and
libfreetype. Run:

    $ git clone git@github.com:fviz/fviz.git
    $ cd fviz
    $ cmake .
    $ make

To install the `fviz` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test

