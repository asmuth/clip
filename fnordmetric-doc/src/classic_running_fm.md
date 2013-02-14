Running FnordMetric
-------------------

FnordMetric is based on ruby eventmachine and needs to run in a seperate ruby process.
The preferred way to start it is to create a ruby file (where you put your DSL statements)
and execute this:

Save this to a file `my_fnordmetric.rb`

    require "fnordmetric"

    FnordMetric.namespace :myapp do
      # your configuration here...
    end

    FnordMetric.standalone

To start the application run:

    $ ruby my_fnordmetric.rb


### Running Standalone

FnordMetric Classic consists of multiple components that can either be started
in a single ruby process or in seperate processes.

FnordMetric#standalone allows you to run all components in a single ruby process.
This allows for easy deployments.

_Example: This will start a web server on port 4242, a tcp acceptor on port 1337 and a worker_

    FnordMetric.standalone


### Web Interface

The web interface is a thin server which server the dashboard the websocket connection.
You can start multple web interface instances per FnordMetric deployment to scale out.

_Example: This will start the web server on port 4242_

    FnordMetric::Web.new(:port => 4242)


### Acceptor

The Acceptor is the component that accepts incoming events and puts them into the internal
FnordMetric queue. There are a number of included acceports for TCP, UDP, AMQP, STOMP, etcetera.
(see [Sending Data](/documentation/classic_sending_data) for more info). You can start multiple
acceptors per FnordMetric deployment to scale out.

_Example: This will start a TCP listener on port 2323_

    FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)


### Worker

The worker process connects to redis, pops events from the internal queue (where they hav
been put by the acceptor) and executes the event handlers. This is where the actual gauge
manipulations in redis take place. You can start multiple multiple workers per FnordMetric
deployment to scale out.

_Example: This will start a worker_

    FnordMetric::Worker.new


### Configuration

You set configuration options by calling the `FnordMetric#options=` method
before starting FnordMetric. To set e.g. a different redis url than localhost
use:

    FnordMetric.options = {
      :redis_url => "redis://my_redis_server.domain.com:6379"
    }

The valid configuration options are:

<table>
  <tr>
    <th>redis_url</th>
    <td>
      you redis server address. default is `redis://localhost:6379`
    </td>
  </tr>
  <tr>
    <th>redis_prefix</th>
    <td>
      FnordMetric prefixes all keys it saves in redis. The default prefix is `fnordmetric`
    </td>
  </tr>
  <tr>
    <th>default_flush_interval</th>
    <td>
      default gauge flush interval, default value is 10 seconds
    </td>
  </tr>
  <tr>
    <th>enable_active_users</th>
    <td>
      enable the active users plugin, default value is true
    </td>
  </tr>
  <tr>
    <th>enable_gauge_explorer</th>
    <td>
      enable the gauge explorer plugin, default value is true
    </td>
  </tr>
  <tr>
    <th>event_queue_ttl</th>
    <td>
      this controls how long events are allowed to stay in the internal queue
      before being processed (events that are not processed in this timeframe
      are lost). the default is 2 minutes.
    </td>
  </tr>
  <tr>
    <th>event_data_ttl</th>
    <td>
      this controls how long event data is kept in redis. you can use this
      to drastically lower redis memory usage. the default is 30 days.
    </td>
  </tr>
  <tr>
    <th>session_data_ttl</th>
    <td>
      this controls how long user sessions are saved in redis (this
      option is void if you disable the active_users plugin). the default is
      30 days.
    </td>
  </tr>
  <tr>
    <th>http_websocket_only</th>
    <td>
      if set to true, the http server (thin) will accept websocket connections, but
      will not start the web interface
    </td>
  </tr>
</table>
<br />
