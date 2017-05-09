Collecting Data via Shell
=========================

You can collect measurements by (periodically) executing any program or shell
script. The standard output of the program will be parsed and interpreted as
measurements. To collect metrics by executing an external program, add a
`collect_proc` block to your configuration.

    collect_http {
      cmd my_script.sh
      interval 10s
      format statsd
    }

This is the list of valid stanzas within the `collect_http` block:

<table>
  <thead>
    <tr>
      <th>Setting</th>
      <th>Description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>cmd</strong></code></td>
      <td>The command to execute. Note that this must be a list of argv arguments and is not interpreted by any shell.</td>
    </tr>
    <tr>
      <td><code><strong>format</strong></code></td>
      <td>The expected input sample format ('statsd', 'borgmon' or 'json')</td>
    </tr>
    <tr>
      <td><code><strong>interval</strong></code></td>
      <td>Controls how often the program should be executed. Valid values are any number (interpreted as seconds) or strings of the format
`N[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For
example: `1s`, `30min` or `6hours`.</td>
    </tr>
    <tr>
      <td><code><strong>label_set</strong></code></td>
      <td>Set a (default) label value.  See <a href="/documentation/rewrite-rules">Rewrite Rules</a></td>
    </tr>
  </tbody>
</table>


### Example

Save this file as `~/example.conf`:

    backend "sqlite:///tmp/test.sqlite"

    metric temperature {
      kind gauge(float64)
    }

    collect_proc {
      cmd "collect_temp.sh"
      format statsd
      interval 10s
    }

Then save this script to `~/collect_temp.sh` and set the executable bit. (The
script should be in the same folder as the `example.conf` file)

    #!/bin/bash
    echo 'temperature 23.5'

Now simply run the `collect` command to start the collection process. Let's
also add the `--verbose` switch so that we see what's going on

    $ metrictl --verbose --config ~/example.conf collect

