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


### What are some differences between fviz and other chart generators?

While fviz's primary usecase *is* creating charts, it is at the core not a
chart generator, but a parametric drawing application that happens to be very
well suited to drawing charts.

When building charts, this approach does not always result in the shortest possible
configuration files, but it gives you a lof of flexibility for customizing your
illustrations in ways that were unforseen by the fviz authors.

Other things that differentiate fviz from other chart generators are its license,
the fact that it is written in C++ and can be embedded into any application and
its CSS-like configuration language.


### How does fviz compare to HTML/CSS?

Most readers will know CSS so it might be useful to point out the similarities
and differences between fviz and HTML/CSS, since both lay out elements as a
nested tree of boxes and have very similar syntax.

However, that is pretty much where the similarities end. A CSS file contains a
list of rules that are later applied to a tree of elements defined elsewhere. A
fviz files contains a list of definitions. Each definition corresponds 1:1
to an element which is implicitly created.

Also, since CSS is (also) intended for creating text documents, it is based around a 
concept of universal "page flow", i.e. by default block elements in CSS will be
stacked vertically from top to bottom, similar to the way rows of text are placed
one after another other in a page.

This concept of page flow, or page layout does not exit in fviz.
fviz is a tool for creating illustrations, not a document publishing system, so
it does make sense to assume "top to bottom" as a default element placement strategy.

So instead of being stacked horizontally within the parent box, like in CSS, boxes
in fviz will, by default, simply be drawn on top of each other with each one
filling up the entirety of the parent box. This is a much more useful default
behaviour for creating illustrations where in many cases an element more closely
resembles the concept of a 'layer' as found in graphics editing software such as
Photoshop or Illustrator than that of a row of text.
