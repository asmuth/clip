require "eventmachine"
require 'em-hiredis'
require 'redis'
require "active_support/core_ext"
require 'yajl'
require 'sinatra/base'
require 'haml'
require 'json'
require "thin"
require 'rack/server'
require 'rack/websocket'

require "fnordmetric/ext"
require "fnordmetric/version"

module FnordMetric

  @@options = nil
  @@pool = []

  @@namespaces = {}

  def self.namespace(key=nil, &block)
    @@namespaces[key] = block
  end

  def self.namespaces
    {}.tap do |_namespaces|
      @@namespaces.each do |key, block|
        _namespaces[key] = FnordMetric::Namespace.new(key, options.clone)
        _namespaces[key].instance_eval(&block)
        _namespaces[key].instance_eval(&FnordMetric::DEFAULT_PROC)
      end
    end
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

  def self.mk_redis
    host, port = options[:redis_url].gsub("redis://", "").split(":")
    redis_opts = { :host => host }
    redis_opts.merge!(:port => port) if port
    Redis.new(redis_opts)
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
    log "[ERROR] #{msg}"; nil
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

  def self.server_configuration=(configuration)
    puts "DEPRECATION WARNING - FIXPAUL"
    self.options=(configuration)
  end

  def self.standalone
    puts "DEPRECATION WARNING - FIXPAUL"
    require "fnordmetric/standalone"
    start_em
  end

end

require "fnordmetric/gauge_calculations"
require "fnordmetric/gauge_modifiers"
require "fnordmetric/gauge_validations"
require "fnordmetric/gauge_rendering"
require "fnordmetric/gauge"
require "fnordmetric/gauges/timeseries_gauge"
require "fnordmetric/gauges/toplist_gauge"
require "fnordmetric/gauges/distribution_gauge"
require "fnordmetric/context"
require "fnordmetric/histogram"
require "fnordmetric/timeseries"
require "fnordmetric/toplist"
require "fnordmetric/namespace"
require "fnordmetric/session"
require "fnordmetric/api"
require "fnordmetric/worker"
require "fnordmetric/logger"
require "fnordmetric/defaults"
require "fnordmetric/web/web"
require "fnordmetric/web/app_helpers"
require "fnordmetric/web/app"
require "fnordmetric/web/websocket"
require "fnordmetric/web/reactor"
require "fnordmetric/web/event"
require "fnordmetric/web/dashboard"
require "fnordmetric/acceptors/acceptor"
require "fnordmetric/acceptors/tcp_acceptor"
require "fnordmetric/acceptors/udp_acceptor"
require "fnordmetric/widget"
require "fnordmetric/widgets/timeseries_widget"
require "fnordmetric/widgets/numbers_widget"
require "fnordmetric/widgets/bars_widget"
require "fnordmetric/widgets/toplist_widget"
require "fnordmetric/widgets/pie_widget"
require "fnordmetric/widgets/html_widget"
