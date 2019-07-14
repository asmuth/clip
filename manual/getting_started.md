Getting Started
===============

Welcome to fviz! This page will guide you through creating a simple first plot
and then give you pointers to more in-depth documentation. If you don't have
fviz installed on your machine yet, please take a look at the [Installation](/documentation/installation)
page first.


### 1) The first image

fviz is a data plotting program; it reads an input text file containing a
description of the plot and outputs an image, either in a vector graphics format
such as SVG, or as a bitmap.

A fviz input file consists of a tree of 'element' specifications which describe
graphical elements such as lines or text boxes. Each element can have a number
of key-value properties that control its contents and appearance.

However, instead of loosing too many words on the terminology, here is a minimal
example file that you can save to your machine and run:

    width: 800px;
    height: 400px;

    points {
      xs: 90px, 650px, 420px;
      ys: 180px, 300px, 120px;
      sizes: 3pt, 6pt, 11pt;
      colors: #06c, #0c6, #c06;
    }

The example defines a new image which contains a single `points` element. The
points element will draw a circle at each coordinate that is specified using the
`xs` and `ys` properties. Note that for now, we will specify the coordinates
as pixel values.

Save the file to `example.fvz` and then run it using command line below:

    fviz --in example.fvz --out example.svg

As the output file, fviz will produce an image with three coloured dots as
displayed below:


<section class="info_box">
  <h2>Example Output</h2>
  <div class="example">
    <img src="/documentation/figures/getting_started1.svg">
  </div>
</section>

It's not the most exciting example in the world, but it illustrates the general
working principle of fviz and shows you how to run files through it.

You might have noticed that fviz uses a coordinate system where (0, 0) is at
the bottom left corner of the screen and positive values of the y axis extend
upwards. This is different to the usual computer graphics convention, but is
much more natural for creating plots.


### 2) Adding axis labels

To make our example into something that looks more like a normal chart, we need to
draw axis labels. This is accomplished by adding two more element definitions to
the file. This time we will add `axis` elements:

    axis {
      position: left;
    }

    axis {
      position: bottom;
    }

Once we run the file again, we end up with the image below, which already
looks much better. However it still doesn't make a lot of sense, since all axes
go from zero to one and have no relationship to the coloured circles.

<section class="info_box">
  <h2>Example Output</h2>
  <div class="example">
    <img src="/documentation/figures/getting_started2.svg">
  </div>
</section>


### 3) Defining a scale

In the next step, we will use a scale for the plot, i.e. a method of mapping
"naked" numerical values to a position on the screen. What that means is that
instead of specifying the circle positions as pixel values, we will specify
them as "naked" values and let fviz take care of mapping them to screen
positions.

    // before
    xs: 90px, 650px, 420px;
    ys: 180px, 300px, 120px;

    // after
    xs: 10, 15, 20,  30, 40, 50;
    ys: 34, 18, 43, 19, 25, 33;

By default, fviz will automatically fit a `linear` scale so that all coordinates
are inside of the screen. We can use the `scale-x-min` and `scale-x-max` properties
to override the automatic fitting and explicitly set the lower and upper limits of
the axes:

    scale-x-min: 0;
    scale-x-max: 60;

The new example with all modifications is displayed below. It's starting to look
like a proper data plot now!

<section class="info_box">
  <h2>Example Output</h2>
  <div class="example">
    <img src="/documentation/figures/getting_started3.svg">
  </div>
</section>

Below is the full source code for the final example.  You might notice that the
`scale-*` properties are defined at the top of the file and not inside any of the
elements. They still affect the elements below them, since fviz allows elements
to inherit property values from their parents. You can play around with moving
these properties inside one of the `points` or `axis` elements and see how
it changes the result.

    width: 800px;
    height: 400px;

    scale-x-min: 0;
    scale-x-max: 60;

    scale-y-min: 0;
    scale-y-max: 80;

    points {
      xs: 10, 15, 20,  30, 40, 50;
      ys: 34, 18, 43, 19, 25, 33;
      sizes: 3pt;
      colors: #06c;
    }

    axis {
      position: left;
    }

    axis {
      position: bottom;
    }



### 4) Fin

In the interest of keeping this Getting Started page short and easy to digest,
this is it for now. You have seen all major concepts in fviz -- the rest
is just adding more elements to your file and fine-tuning the appearance of
individual elements using the properties described in the specification.

For more information, please take a look at the remaining documentation chapters
or the [Examples](/examples) page. Here are some pointers to pages that might
be interesting to read next:

  - [Adding a legend](/elements/legend)
  - [Loading data from CSV files](/documentation/datasource-csv)

