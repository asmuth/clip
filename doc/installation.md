Installing metrictools
======================

The metrictools are written in C++. You can either install a binary release or
compile from source. Two main programs will be installed into your system:
`metrictl` and `metricd`.

You can find the complete [source code on Github](http://github.com/paulasmuth/metrictools)

### Compile from source

You need autotools and a modern c++ compiler. To compile metrictools from a git
checkout run:

    $ git clone https://github.com/paulasmuth/metrictools.git && cd metrictools
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install


