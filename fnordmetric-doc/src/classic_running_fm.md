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

FIXPAUL!

    FnordMetric.options = {
      ...
    }


<table>
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
</table>
<br />


      :redis_url => "redis://localhost:6379",
      :redis_prefix => "fnordmetric",
      :inbound_stream => ["0.0.0.0", "1337"],
      :inbound_protocol => :tcp,
      :web_interface => ["0.0.0.0", "4242"],
      :web_interface_server => "thin",
      :start_worker => true,
      :print_stats => 3,
      :event_queue_ttl => 120,
      :event_data_ttl => 3600*24*30,
      :session_data_ttl => 3600*24*30
