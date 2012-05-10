require "eventmachine"
require 'em-hiredis'
require 'redis'
require "active_support/core_ext"
require 'yajl'
require 'sinatra/base'
require 'haml'
require 'rack/server'

require "fnordmetric/ext"
require "fnordmetric/version"

module FnordMetric

  @@options = nil
  @@pool = []

  @@firehose = EM::Channel.new

  def self.backend
    FnordMetric::RedisBackend.new(options)
  end

  def self.options(opts = {})
    default_options(@@options || {}).merge(opts)
  end

  def self.options=(opts)
    @@options = opts
  end

  def self.register(obj)
    @@pool.push(obj)
  end

  def self.firehose
    @@firehose
  end

  def self.default_options(opts = {})
    {
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
    }.merge(opts)
  end

  def self.log(msg)
    puts "[#{Time.now.strftime("%y-%m-%d %H:%M:%S")}] #{msg}"
  end

  def self.error(msg)
    log "[ERROR] #{msg}"
  end

  def self.error!(msg)
    raise msg if ENV['FNORDMETRIC_ENV'] == 'test'
    puts(msg); exit!
  end

  def self.run
    start_em
  rescue Exception => e
    raise e
    log "!!! eventmachine died, restarting... #{e.message}"
    sleep(1); run
  end

  def self.shutdown(fnord=nil)
    log "shutting down, byebye"
    EM.stop
  end

  def self.start_em
    EM.run do

      trap("TERM", &method(:shutdown))
      trap("INT",  &method(:shutdown))

      EM.next_tick do
        (@@pool || []).map(&:initialized)
      end

    end
  end

  # LEGACY / BACKWARDS COMPATBILE STUFF

  def self.server_configuration=(configuration)
    self.options=(configuration)
  end

  def self.namespace(*args, &block)
    FnordMetric::Web.namespace(*args, &block)
  end

end



require "fnordmetric/remote_gauge"
require "fnordmetric/multi_gauge"
require "fnordmetric/event_handler"


require "fnordmetric/backends/redis_backend"
require "fnordmetric/backends/memory_backend"

require "fnordmetric/acceptors/acceptor"
require "fnordmetric/acceptors/tcp_acceptor"
require "fnordmetric/acceptors/udp_acceptor"

require "fnordmetric/gauges/dummy_gauge"
require "fnordmetric/gauges/numeric_gauge"

require "fnordmetric/widgets/widget"
require "fnordmetric/widgets/timeline_widget"
require "fnordmetric/widgets/numbers_widget"

require "fnordmetric/metrics/redis_metric_calculations"
require "fnordmetric/metrics/redis_metric_modifiers"
require "fnordmetric/metrics/redis_metric"

require "fnordmetric/web/web"
require "fnordmetric/web/namespace"
require "fnordmetric/web/app"
require "fnordmetric/web/websocket"
require "fnordmetric/web/event"
require "fnordmetric/web/dashboard"
require "fnordmetric/web/session"

require "fnordmetric/logger"


 # require "fnordmetric/context"

# require "fnordmetric/api"
# require "fnordmetric/udp_client"
# require "fnordmetric/worker"

# require "fnordmetric/"
# require "fnordmetric/numbers_widget"
# require "fnordmetric/bars_widget"
# require "fnordmetric/toplist_widget"
# require "fnordmetric/pie_widget"
# require "fnordmetric/html_widget"

# require "fnordmetric/namespace"

# require "fnordmetric/numeric_gauge"
# require "fnordmetric/toplist_gauge"
