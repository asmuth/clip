Storage Backends
================

To use the metric data collection facilities in FnordMetric Server, it must be
started with a storage backend. To start the server with a storage backend,
pass the `--storage_backend` in startup:

    $ fnordmetric-server --storage_backend=<backend>

The currently implemented storage backends are `inmemory`, `disk`, `mysql` and
`hbase`.


Local Disk Backend
------------------

To store the metric data on local disk, you must also pass the `--datadir` flag
to specify a folder in which FnordMetric should store the data. The file pointed
to by `--datadir` must exist and must be a folder.

    $ fnordmetric-cli --storage_backend=disk --datadir=<path>

For example:

    $ mkdir -p /tmp/fnordmetric-data
    $ fnordmetric-cli --storage_backend=disk --datadir=/tmp/fnordmetric-data



In-Memory Backend
-----------------

The in memory backend should only be used for testing purpose since it will
use an unbounded amount of memory. (Seriously, do not use this in production)


    $ fnordmetric-cli --storage_backend=inmemory


MySQL Backend
-------------

_The MySQL Backend is not implemented yet._

HBase Backend
-------------

_The HBase Backend is not implemented yet._
