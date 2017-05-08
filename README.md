metrictools
===========

[![Build Status](https://travis-ci.org/paulasmuth/metrictools.png?branch=unstable)](http://travis-ci.org/paulasmuth/metrictools)

metrictools is a collection of lightweight tools that allow you to record
measurements and build beautiful real-time dashboards within minutes. All using
your favorite backend database (currently supported are SQLite, MySQL, Postgres
and EventQL). **If you're new to metrictools it's highly recommended that you start
with the [Getting Started](https://metrictools.org/documentation/getting-started) page.**.

Here's a quick rundown of the key features for people who don't like reading
documentation.:

  - Add your own custom metrics and labels
  - Insert data, run queries and create charts using a standalone command line interface
  - Pull ("scrape") measurements via HTTP, shell scripts or SNMP or push via HTTP or UDP/statsd
  - Include charts and tables in any HTML page using the HTTP API
  - All metrics are explicitly configured and have a number of rich settings (like units of measurement)
  - Data is stored in a pluggable storage backend (SQLite, MySQL, Postgres, EventQL)
  - Optional background daemon that runs data collection and exposes a HTTP API
  - Plugins can contain metric configuration, exporters and dashboards (check out
    the linux plugin)
  - Implemented as collection of lightweight composable commands in the spirit of the original daemontools project
  - Written in self-contained C++; quick to set up and does not require a lot of resources
  - The metrictools project was started over 7 years ago and is still actively maintained

More Information:
[Documentation](http://metrictools.org/),
[Download](https://metrictools.org/download/),
[Getting Started](https://metrictools.org/documentation/getting-started)


## Build

Before we can start we need to install some build dependencies. Currently
you need a modern c++ compiler, libz and autotools.

    # Ubuntu
    $ apt-get install clang make automake autoconf libtool zlib1g-dev

    # OSX
    $ brew install automake autoconf

To build metrictools from a distribution tarball:

    $ ./configure
    $ make
    $ sudo make install

To build metrictools from a git checkout:

    $ git clone git@github.com:paulasmuth/metrictools.git
    $ cd metrictools
    $ ./autogen.sh
    $ ./configure
    $ make V=1
    $ sudo make install

To run the test suite:

    $ make check


## Documentation

You can find the full documentation at https://metrictools.org/


## Contributing

To contribute, please fork this repository, make your changes and run the 
tests, commit them to your github repository and send a pull request.
Need help, head on over to our [Google Groups](htt//groups.google.com/group/metrictools)
page to discuss any ideas that you might have.


## License

Copyright (c) 2011-2017 Paul Asmuth, Laura Schlimmer et al.

metrictools is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

metrictools is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
metrictools. If not, see <http://www.gnu.org/licenses/>.
