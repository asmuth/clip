Installation
============

clip is written in C++. You can either install a binary release or compile
from source. Once the installation is complete, you will have a `clip` binary
on your system.

You can find the complete [source code on Github](http://github.com/asmuth/clip)


### Compile from source

To build clip, you need an up-to-date C++ compiler, cmake, libharfbuzz and
libfreetype. To install these dependencies, run the following command:

    # Ubuntu
    $ apt-get install cmake libfreetype6-dev libharfbuzz-dev libfribidi-dev libcairo2-dev libfmt-dev

    # Fedora
    $ dnf install cmake freetype-devel harfbuzz-devel fribidi-devel cairo-devel fmt-devel

    # OSX
    $ brew install cmake freetype harfbuzz fribidi cairo cppformat

To compile `clip`, Run:

    $ git clone git@github.com:asmuth/clip.git
    $ cd clip
    $ cmake .
    $ make -j

To install the `clip` binary into your system, run `make install`:

    $ make install

To run the test suite, run `make test`:

    $ make test

