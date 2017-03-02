Installing FnordMetric
======================

FnordMetric is written in C++. You can either install a binary release or compile
from source. Fnordmetric will install two main binaries into your system:
`metric-collectd` and `metric-queryd`.

You can find the complete [source code on Github](http://github.com/paulasmuth/fnordmetric)

### Compile from source

You need autotools and a modern c++ compiler. To compile fnordmetric from a git
checkout run:

    $ git clone https://github.com/paulasmuth/fnordmetric.git && cd fnordmetric
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install


