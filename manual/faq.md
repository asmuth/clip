Frequently Asked Questions
==========================

### I'm getting build errors when compiling the code

fviz needs a reasonably modern C++ compiler and standard library. In most
cases where the code doesn't build it's due to some problem with the local build
environment. We always appreciate bug reports so we improve our build system, but
often the best workaround for the moment is to download a binary release from the
downloads page.


### The test suite fails on my machine because text placement is slightly offset

The test suite requires that you have the original Microsoft Arial TTF files
installed on your machine. To verify that this is the case, run `fc-match
'Arial,Helvetica,Helvetica Neue:style=Regular,Roman'` and check that it returns
the correct 'arial.ttf' file.

