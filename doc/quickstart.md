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

    (plot/lines
        data-x (100 200 300 400 500 600 700 800 900)
        data-y (1.2   1.8   1.3   1.6   1.5   1.3   1.8   1.9   2.0)
        limit-y (0 3)
        limit-x (0 1000)
        marker-shape (pentagon)
        marker-size 8pt)

<figcaption style="margin-top: -.4em;"><code>example_chart.clp</code></figcaption>

In the script, we call a single command called `plot/lines` with a number
of arguments such as `data-x` or `limit-y`. The exact meaning of each of the
arguments doesn't matter for now. The specifics of each argument are documented
on the [reference page of the command](/commands/plot/lines) and you can
probably guess what most of them do anyway.

What's important for now is that the syntax from above represents a call to a
command called `plot/lines` with a number of keyword arguments.

Coming from other languages, it's probably a bit surprising to see a forward slash
as part of the command name, but this is allowed in clip. Since, at this point,
all commands in clip live in the same global namespace, the forward slash is
used to group them into modules. In other words, the `plot/lines` is the
`lines` command from the `plot` module.

But back to our example. The `plot/lines` command that we're calling takes
the input data table, transforms it for display and then plots the data as a line.
To see what that looks like, save the content from above to a file called
`example_chart.clp` and run it through clip using the following command:

    $ clip --in example_chart.clp --out example_chart.svg

After running the example, you should have an output file similar to the one below:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart1.svg">
  <figcaption><code>example_chart.svg</code></figcaption>
</figure>

### Step 2: Adding axes

To make this into a proper plot, we have to add some axes. For that, we extend
the script to invoke a second command called `plot/axes`. Add this to the
`example_chart.clp` file:

    (plot/axes
        limit-y (0 3)
        limit-x (0 1000)
        label-format-x (scientific)
        label-placement-x (linear-interval 100 100 900))

After re-running clip on the updated script, the output should now look much
more like the kind of chart we all know and love:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart2.svg">
  <figcaption><code>example_chart.svg</code></figcaption>
</figure>

When adding the `plot/axes` command to the script, make sure to add it
_before_ the existing, `plot/lines` command; the order of commands in clip
is significant! The main reason for this is that drawing in clip is performed
using the "painters algorithm", i.e. later commands draw over the output of
earlier commands, so changing the order of commands will generally give a different
result.

However, in this case there is a second reason why command order matters. Commands
in clip are allowed to have two types of side effects. The one obvious side effect
is that commands are allowed to modify the implicit current "image" by adding
graphical elements to it. 

The other permissible side effect is that commands may modify the so called
"transform stack". The `plot/axes` command in particular modifies the transform
stack so that further painting takes place within the rectangular region described
by the axes. In other words, it makes it so that lines and other shapes drawn
after the `axes` command will be drawn within the axes.

To learn more about the transform stack and how to modify it manually, please
refer to the [Layout](#FIXME) page.


### Step 3: Adding the legend

To close things out on this example, we're going to add an explanatory legend to
our chart. To do so, we will use the `figure/legend` command. Simply add the
snippet from below to the end of the script:

    (figure/legend
        position (bottom left)
        item (label "Example Data" marker-shape (pentagon)))

Also, let's get rid of the duplicate `limit-x` and `limit-y` arguments. To prevent
us from typing common arguments over and over, clip allows us to declare default
arguments using the `default` statement. A default argument will be passed to
any command that accepts it.

This leaves us with this final script:

    (default limit-y (0 3))
    (default limit-x (0 1000))

    (plot/axes
        label-format-x (scientific)
        label-placement-x (linear-interval 100 100 900))

    (plot/lines
        data-x (100 200 300 400 500 600 700 800 900)
        data-y (1.2   1.8   1.3   1.6   1.5   1.3   1.8   1.9   2.0)
        marker-shape (pentagon)
        marker-size 8pt)

    (figure/legend
        position (bottom left)
        item (label "Example Data" marker-shape 

Running the above file through clip again should now yield the following final result:

<figure>
  <img class="small" alt="Example Chart" src="/figures/quickstart3.svg">
  <figcaption><code>example_chart.svg</code></figcaption>
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


