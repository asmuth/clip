Rewrite Rules
=============

There is one rewrite mechanism that allows you to override the labels an metric
ids on input measurements. The stanza is `label_set`.


## The label_set stanza

The `label\_set` stanza allows you override a value for one of the labels. This
is the syntax of the label_set stanza:

    label_set <label_name> <label_value> [default]

The `default` modifier means that the label should only be set if it is not
already set. So for example, this would set the `hostname` label to `localhost`
for all measurements that do not alrady have a hostname label:

    label_set hostname "localhost" default

While this line would override the hostname label for _all_ measurements, even
if they already have a hostname label.

    label_set hostname "localhost"

The `label\_set` stanza is valid in the global context (where it applies to all metrics),
within a metric block (where it applies only to the single metric) and in every `collect\_*`
and `listen\_*` blocks (where it applies to measurements received through the
respective transport).The order in which non-default overrides are applied is
metric, transport, global. The order in which default overrides are applied is
the reverse of the previous list.
