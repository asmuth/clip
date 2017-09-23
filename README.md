libzdb
======

[![Build Status](https://travis-ci.org/paulasmuth/libzdb.png?branch=master)](http://travis-ci.org/paulasmuth/libzdb)

libzdb is a lightweight embedded database:

  * Data is organized into tables and rows; tables have a strict schema
  * Column-oriented internal storage layout for efficient compression and I/O
  * Allows reading data without copying it (zero-copy)
  * Supports non-blocking I/O via io_submit/aio
  * Implemented as a lightweight C/C++ library
  * Safe to use from multiple threads and processes

[Documentation](http://libzdb.org/) | [Download](http://libzdb.org/download/)

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

Copyright (c) 2011-2017 Paul Asmuth, Laura Schlimmer

The libzdb library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option) any
later version.

libzdb is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with libzdb. If not, see <http://www.gnu.org/licenses/>.
