Command Line
============

The primary method for translating input expressions to SVG or PNG output files is
the `clip` command line program. A typical invocation looks like this:

    $ clip --in my_chart.clp --out my_chart.svg

See below for a full list of supported command line options. Besides the command
line interface, clip also offers a C API that can be used to render charts
directly from your application.


List of command line flags
--------------------------

Below you can find a list of all currently supported command line arguments.

    Usage: $ clip [OPTIONS]
      --in <path>               Path to the input file
      --out <path>              Path to the output file
      --stdin                   Read the input file from stdin
      --stdout                  Write the output file from sdout
      --format <format>         Output format. If no format is given, it is inferred from the
                                filename. Valid values: 'png', 'svg'
      --font-defaults <bool>    Enable or disable default font loading. Default is enabled.
                                Valid values: 'on' and 'off'
      --font-load <path>        Add a font file to the default font list
      --debug                   Run in debug mode
      --help                    Display this help text and exit
      --version                 Display the version of this binary and exit

    Examples:
      $ clip --in my_chart.clp --out my_chart.svg

