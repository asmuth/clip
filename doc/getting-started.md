Getting Started
===============

Welcome to clip! This page will guide you through creating a simple first plot
and then give you pointers to more in-depth documentation. If you don't have
clip installed on your machine yet, please take a look at the [Installation](/documentation/installation)
page first.

In essence, clip is an automated drawing program; it reads an input text file
containing a list of drawing instructions and then outputs a result image.

### Step 1: First lines

Being a highly visual tool, clip is best explained by example, so here is a
minimal example file to get you started. Paste the contents to a file called
`example_chart.clp`:

    class: plot;

    lines {
      data-x: list(100 200 300 400 500 600 700 800 900);
      data-y: list(1.2 1.8 1.3 1.6 1.5 1.3 1.8 1.9 2.0);
      limit-y: 0 3;
      limit-x: 0 1000;
      marker-shape: pentagon;
      marker-size: 8pt;
    }

You can then run the script through clip using the following command:

    $ clip --in example_chart.clp --out example_chart.svg

After running the example, open the output file `example_chart.svg`. It should
look similar to the one below:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart1.svg" />
</figure>

Let's analyze the clip script we just ran. The first line of a clip script
normally contains a `(class ...)` declaration. The `class` controls which module
is used to interpret the subsequent expressions. In this example, we are using the
`plot` module. Once the plotting module is loaded, the remainder of the script
contains a list of plotting commands.

In the next line, we call a command named `lines` with a number of keyword
arguments. The exact meaning of each of the arguments doesn't matter for
now; the specifics are documented on the [reference page of the command](/plot/lines)
and you can probably guess what most of them do anyway.

### Step 2: Adding axes

To make this into a proper plot, we have to add some axes. For that, we extend
the script with a call to the `axes` command. Add this to the beginning of the file:

    axes {
      limit-y: 0 3;
      limit-x: 0 1000;
      label-format-x: scientific();
      label-placement-x: linear-interval(100 100 900);
    }

After re-running clip on the updated script, the output should now look much
more like the kind of chart we know and love:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart2.svg" />
</figure>

When adding the `axes` command to the script, make sure to add it _before_ the
existing, `lines` command. The order of statements in clip is significant. Commands
generally draw over the output of earlier commands, so changing the order of
commands will generally give a different result.

### Step 3: Adding the legend

To close things out on this example, we're going to add an explanatory legend to
our chart using the `legend` command. Simply add the snippet from below to the
file:

    legend {
      position: bottom left;

      item {
        label: "Example Data";
        marker-shape: pentagon;
      }
    }

Also, let's get rid of the duplicate `limit-x` and `limit-y` arguments. This leaves
us with this final script:

    class: plot;

    axes {
      limit-y: 0 3;
      limit-x: 0 1000;
      label-format-x: scientific();
      label-placement-x: linear-interval(100 100 900);
    }

    lines {
      data-x: list(100 200 300 400 500 600 700 800 900);
      data-y: list(1.2 1.8 1.3 1.6 1.5 1.3 1.8 1.9 2.0);
      limit-y: 0 3;
      limit-x: 0 1000;
      marker-shape: pentagon;
      marker-size: 8pt;
    }

    legend {
      position: bottom left;

      item {
        label: "Example Data";
        marker-shape: pentagon;
      }
    }


Running the above file through clip again should now yield the following final result:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart3.svg" />
</figure>


Further reading
---------------

In the interest of keeping this Getting Started page short and easy to digest,
this is it for now. You have seen how to create a simple plot using clip -- the rest
is just adding more elements to your file and fine-tuning the appearance of
individual elements using the arguments described in the documentation of each
individual command.

For more information, please take a look at the [remaining documentation chapters](/plot),
in particular at the [Examples](/examples) page and the documentation pages for
the individual commands.


