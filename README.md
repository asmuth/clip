plotfx
======

A command line tool for creating charts from CSV files.

**!!! WARNING !!!! -- WORK IN PROGRESS**: This is the old "FnordMetric" ChartSQL
code which had been unmaintained for some time. In hindsight, I now think that the
tight coupling of the charting code to the built-in SQL engine has somewhat limited
its usability. Rather than just letting the old code rot, I'm currently working on
removing the SQL parts and making the plotting toolkit usable with a simple
css-like interface. This might take a while.

[Examples](https://github.com/plotfx/plotfx/tree/master/examples) |
[Documentation](https://plotfx.org/)


Building
--------

To build plotfx, you need an up-to-date C++ compiler, cmake and libcairo. Run:

    $ cmake .
    $ make

To run the test suite, run `make check`:

    $ make check

License
-------

    The plotfx License (BSD-3-Clause)

    Copyright (c) 2018, Paul Asmuth, Laura Schlimmer
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
