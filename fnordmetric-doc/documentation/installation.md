Installing FnordMetric
======================

FnordMetric is written in C++. You can either install a binary release or compile
from source. Fnordmetric will install two binaries into your system: `fnordmetric-cli`
and `fnordmetric-server`.

#### Download FnordMetric

+ [Download FnordMetric v0.0.1 \(latest\) -- OSX 10.8](/downloads/fnordmetric-ui-1.2.9.tar) (.tar, 340k) <br /> MD5: cd4eb89d9a0353840d5f343690e4ff2e

+ [Download FnordMetric v0.0.1 \(latest\) -- linux-generic-x86_46](/downloads/fnordmetric-ui-1.2.9.tar) (.tar, 340k) <br /> MD5: cd4eb89d9a0353840d5f343690e4ff2e


#### Install via apt-get

    $ apt-get install fnordmetric

#### Install via homebrew

    $ brew install fnordmetric

#### Install via emerge

    $ emerge fnordmetric


### Compile from source

You need cmake and a modern c++ compiler. Optional dependencies are libmysqlclient.
You can find the complete [source code on Github](http://github.com/paulasmuth/fnordmetric)
or download the release tarball from this website (recommended):

+ [Download FnordMetric v0.0.1 \(latest\) source](/downloads/fnordmetric-ui-1.2.9.tar) (.tar, 340k) <br /> MD5: cd4eb89d9a0353840d5f343690e4ff2e

To compile fnordmetric

    $ make

To run the tests

    $ make test
