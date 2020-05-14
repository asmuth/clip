Getting Started
===============

Welcome to clip! This page will guide you through creating a simple first plot
and then give you pointers to more in-depth documentation. If you don't have
clip installed on your machine yet, please take a look at the [Installation](/documentation/installation)
page first.

In essence, clip is a data plotting program; it reads an input text file containing
a list of drawing instructions outputs an image, either in a vector graphics format
such as SVG, or as a bitmap.

A clip input file consists of a list of drawing commands. Most drawing commands
produce high-level graphical elements such as charts, lines or text boxes. Each
command has a number of arguments that control its contents and appearance.

However, instead of loosing too many words, here is a minimal example file that
you can save to your machine and run to get you started:

Introductory Example
--------------------

### Step 1: First lines

The text box below contains a simple clip script. Paste the contents to a file
called `example_chart.clp`:

    (figure/plot
        lines (
            data-x (100 200 300 400 500 600 700 800 900)
            data-y (1.2   1.8   1.3   1.6   1.5   1.3   1.8   1.9   2.0)
            limit-y (0 3)
            limit-x (0 1000)
            marker-shape (pentagon)
            marker-size 8pt))

In the script, we call a single command named `figure/plot` with a number of
keyword arguments. The exact meaning of each of the arguments doesn't matter for
now; the specifics are documented on the [reference page of the command](/plot/lines)
and you can probably guess what most of them do anyway.

Coming from other languages, it's probably a bit surprising to see a forward slash
as part of the command name, but this is allowed in clip. Since, at this point,
all commands in clip live in the same global namespace, the forward slash is
used to group them into modules. In other words, the `figure/plot` is the
`plot` command from the `figure` module.

But back to our example. In the `lines` argument, we're instructing clip to read
the input data table, transforms it for display and then plot the data as a line.
To see what that looks like, save the content from above to a file called
`example_chart.clp` and run it through clip using the following invocation:

    $ clip --in example_chart.clp --out example_chart.svg

After running the example, you should have an output file similar to the one below:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart1.svg" />
</figure>

### Step 2: Adding axes

To make this into a proper plot, we have to add some axes. For that, we extend
the script to add another argument `axes` to the `figure/plot` command. Add this
to the `example_chart.clp` file above the `lines` argument:

    axes (
        limit-y (0 3)
        limit-x (0 1000)
        label-format-x (scientific)
        label-placement-x (linear-interval 100 100 900))

After re-running clip on the updated script, the output should now look much
more like the kind of chart we all know and love:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart2.svg" />
</figure>

When adding the `axes` command to the script, make sure to add it
_before_ the existing, `lines` command; the order of statements in clip
is significant. The main reason for this is that drawing in clip is performed
using the "painters algorithm", i.e. later commands draw over the output of
earlier commands, so changing the order of commands will generally give a different
result.

### Step 3: Adding the legend

To close things out on this example, we're going to add an explanatory legend to
our chart. To do so, we will use the `legend` argument. Simply add the
snippet from below to the `figure/plot` command:

    legend (
        position (bottom left)
        item (label "Example Data" marker-shape (pentagon)))

Also, let's get rid of the duplicate `limit-x` and `limit-y` arguments. This leaves
us with this final script:

    (figure/plot
        limit-y (0 3)
        limit-x (0 1000)
        axes (
            label-format-x (scientific)
            label-placement-x (linear-interval 100 100 900))
        lines (
            data-x (100 200 300 400 500 600 700 800 900)
            data-y (1.2   1.8   1.3   1.6   1.5   1.3   1.8   1.9   2.0)
            marker-shape (pentagon)
            marker-size 8pt)
        legend (
            position (bottom left)
            item (label "Example Data" marker-shape (pentagon))))


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

For more information, please take a look at the remaining documentation chapters,
in particular at the [Examples](/examples) page and the documentation pages for
the individual commands.


