libzdb
======

[![Build Status](https://travis-ci.org/paulasmuth/zdb.png?branch=master)](http://travis-ci.org/paulasmuth/zdb)
[![License: BSD](https://img.shields.io/badge/License-BSD-green.svg)](https://opensource.org/licenses/BSD-3-Clause)

libzdb is a lightweight embedded database:

  * Data is organized into tables and rows; tables have a strict schema
  * Column-oriented internal storage layout for efficient compression and I/O
  * Allows reading data without copying it (zero-copy)
  * Supports non-blocking I/O via io_submit/aio
  * Implemented as a lightweight C/C++ library
  * Safe to use from multiple threads and processes

## Building

To build and install libzdb from a distribution tarball or git checkout, run
these commands:

    $ cmake .
    $ make
    $ sudo make install

To run the test suite:

    $ make test

## Documentation

You can find the full documentation and screenshots of example programs at
https://libzdb.org/

## Contributing

To contribute, please fork this repository, make your changes and run the 
tests, commit them to your github repository and send a pull request.
Need help, head on over to our [Google Groups](htt//groups.google.com/group/libzdb)
page to discuss any ideas that you might have.

## License

    The libzdb License (BSD-3-Clause)

    Copyright (c) 2016, Paul Asmuth, Laura Schlimmer
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
