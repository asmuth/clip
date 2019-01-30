Frequently Asked Questions
==========================

### What are some differences between PlotFX and other chart generators?

While PlotFX's primary usecase *is* creating charts, it is at the core not a
chart generator, but a parametric drawing application that happens to be very
well suited to drawing charts.

When building charts, this approach does not always result in the shortest possible
configuration files, but it gives you a lof of flexibility for customizing your
illustrations in ways that were unforseen by the PlotFX authors.

Other things that differentiate PlotFX from other chart generators are its license,
the fact that it is written in C++ and can be embedded into any application and
its CSS-like configuration language.


### How does PlotFX compare to HTML/CSS?

Most readers will know CSS so it might be useful to point out the similarities
and differences between PlotFX and HTML/CSS, since both lay out elements as a
nested tree of boxes and have very similar syntax.

However, that is pretty much where the similarities end. A CSS file contains a
list of rules that are later applied to a tree of elements defined elsewhere. A
PlotFX files contains a list of definitions. Each definition corresponds 1:1
to an element which is implicitly created.

Also, since CSS is (also) intended for creating text documents, it is based around a 
concept of universal "page flow", i.e. by default block elements in CSS will be
stacked vertically from top to bottom, similar to the way rows of text are placed
one after another other in a page.

This concept of page flow, or page layout does not exit in PlotFX.
PlotFX is a tool for creating illustrations, not a document publishin system, so
it does make sense to assume "row by row" as a default element placement strategy.
Most of the rest of CSS's layout features build from this core assumption so they
are equally unapplicable to PlotFX.

Instead of being stacked horizontally within the parent box, like in CSS, boxes
in PlotFX will, by default, simply be drawn on top of each other with each one
filling up the entirety of the parent box. This is a much more useful default
behaviour for creating illustrations where in many cases an element more closely
resembles the concept of a 'layer' as found in graphics editing software such as
Photoshop or Illustrator than that of a row of text.


### I'm getting build errors when compiling the code

PlotFX needs a reasonably modern C++ compiler and standard library. In most
cases where the code doesn't build it's due to some problem with the local build
environment. We always appreciate bug reports so we improve our build system, but
often the best workaround for the moment is to download a binary release from the
downloads page.

