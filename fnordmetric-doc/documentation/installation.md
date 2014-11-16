Installing FnordMetric
======================

FnordMetric is written in C++. You can either install a binary release or compile
from source. Fnordmetric will install two binaries into your system: `fnordmetric-cli`
and `fnordmetric-server`.

### Compile from source

You need cmake and a modern c++ compiler. Optional dependencies are libmysqlclient.
You can find the complete [source code on Github](http://github.com/paulasmuth/fnordmetric)

To compile fnordmetric

    $ git clone https://github.com/paulasmuth/fnordmetric.git && cd fnordmetric/fnordmetric-core
    $ make
    $ sudo make install

To run the tests

    $ make test

### Binary Releases

_The first binary release will be published 2014-12-01_
