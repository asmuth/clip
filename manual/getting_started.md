Getting Started
===============

Welcome to fviz! This page will guide you through creating a simple first plot
and then give you pointers to more in-depth documentation. If you don't have
fviz installed on your machine yet, please take a look at the [Installation](/documentation/installation)
page first.


fviz is a data plotting program; it reads an input text file containing a
description of the plot and outputs an image, either in a vector graphics format
such as SVG, or as a bitmap.

A fviz input file consists of a tree of 'element' specifications which describe
graphical elements such as charts, lines or text boxes. Each element can have a
number of properties that control its contents and appearance.

However, instead of loosing too many words on the terminology, here is a minimal
example file that you can save to your machine and run:

Example #1
----------

The example below defines a new image which contains a single `chart/scatterplot`
element. The scatterplot element will draw axes and a dot at each coordinate
that is specified in the `data-x` and `data-y` properties. 

    (chart/scatterplot
        data-x (csv tests/testdata/gauss2d.csv x)
        data-y (csv tests/testdata/gauss2d.csv y)
        limit-x (0 400)
        limit-y (0 200)
        axes (bottom left)
        grid (color #fff)
        background #eee
        border none)

Save the content from above to a file called `example_chart.fvz` and run it
through fviz using the following command:

    $ fviz --in example_chart.fvz --out example_chart.svg

When running the example locally, you can use your own input CSV file, or you
can download the example CSV file [from here](https://github.com/asmuth/fviz/tree/master/tests/testdata/gauss2d.csv).
If everything works, you should get an output file similar to the one below
(`example_chart.svg`):

[![A simple scatterplot](/examples/charts-basic/scatterplot.svg)](https://fviz.org/examples/charts-basic/scatterplot)

It's not the most exciting example in the world, but it illustrates the general
working principle of fviz and shows you how to run files through it.


Further reading
---------------

In the interest of keeping this Getting Started page short and easy to digest,
this is it for now. You have seen how to create a simple plot using fviz -- the rest
is just adding more elements to your file and fine-tuning the appearance of
individual elements using the properties described in the specification.

For more information, please take a look at the remaining documentation chapters
or the [Examples](/examples) page. Here are some pointers to pages that might
be interesting to read next:

  - [Example Gallery](/examples)

