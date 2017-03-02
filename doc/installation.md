Installing FnordMetric
======================

FnordMetric is written in C++. You can either install a binary release or compile
from source. Fnordmetric will install two main binaries into your system: `metricd`
and `metricctl`.

### Compile from source

You need autotools and a modern c++ compiler.

You can find the complete [source code on Github](http://github.com/paulasmuth/fnordmetric)

To compile fnordmetric

    $ git clone https://github.com/paulasmuth/fnordmetric.git && cd fnordmetric
    $ ./autogen.sh
    $ ./configure
    $ make
    $ sudo make install

To run the tests

    $ make check

### Run via Docker

To run FnordMetric Server using docker:

    $ sudo mkdir -p /var/lib/metricd
    $ sudo docker run -p 8175:8175 -p 8125:8125/udp -v /var/lib/metricd:/data/fnmetricd -t fnordmetric/metricd

To build a Docker Image for FnordMetric Server, use the included dockerfile:

    $ git clone https://github.com/paulasmuth/fnordmetric.git && cd metricd/contrib/docker
    $ sudo docker build -t myrepo/metricd .

