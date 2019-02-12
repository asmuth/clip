Getting Started
===============

Welcome to PlotFX! This page will guide you through creating a simple first plot
and then give you pointers to more in-depth documentation. If you don't have
PlotFX installed on your machine yet, please take a look at the [Installation](/documentation/installation)
page first.

### The first image

Here is a minimal PlotFX example file that you can save to your machine and run:

    width: 1200px;
    height: 600px;

    lines {
      xs: 10px 40px;
      ys: 20px 400px;
      colors: #06c;
    }

Save the file to `example.ptx` and then run it using command line below:

    plotfx --in example.ptx --out example.svg

If you do that, PlotFX will produce a white 1200x600 pixel image with a blue line:

    [ image ]

It's not the most useful example in the world, but it shows you how to run files
through PlotFX.


### Adding scales


### Adding a title and legend


