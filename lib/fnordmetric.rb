require 'rubygems'
require "eventmachine"
require 'em-hiredis'
require 'redis'
require 'yajl'

#require 'sinatra/base'
#require 'haml'

module FnordMetric

  @@stat_keys = [:events_received, :events_processed]
  @@namespaces = {}

  def self.namespace(key=nil, &block)    
    @@namespaces[key] = block    
  end


  def self.run(opts={})
    start_em(opts) 
  rescue Exception => e
    log "!!! eventmachine died, restarting... #{e.message}"
    sleep(1); run(opts)  
  end

  def self.start_em(opts)

    opts[:redis_uri] = "redis://localhost:6379"
    opts[:redis_prefix] ||= "fnordmetric"            

    # events that aren't processed after 2 min get dropped
    opts[:event_queue_ttl] ||= 120

    # event data is kept for one month
    opts[:event_data_ttl] ||= 3600*24*30

    # session data is kept for one month
    opts[:session_data_ttl] ||= 3600*24*30 

    EM.run do

      redis = EM::Hiredis.connect(opts[:redis_uri])
      FnordMetric::Worker.new(@@namespaces.clone, opts)

      begin
        FnordMetric::InboundStream.configure(opts)
        EventMachine::start_server "0.0.0.0", 1337, FnordMetric::InboundStream
        log "listening on tcp#1337 for json event data"
      rescue
        log "cant start FnordMetric::InboundStream. port in use?"
      end

      EventMachine::PeriodicTimer.new(1){ heartbeat!(opts, redis) }

      proc{
        log "shutting down, byebye"
        EM.stop
      }.tap do |shutdown|
        trap("TERM", shutdown)
        trap("INT", shutdown)
      end
      
    end 
  end

  def self.heartbeat!(opts, redis, keys=@@stat_keys) 
    redis.llen("#{opts[:redis_prefix]}-queue") do |queue_length|      
      redis.hmget("#{opts[:redis_prefix]}-stats", *keys) do |data|
        data_human = keys.size.times.map{|n|"#{keys[n]}: #{data[n]}"}.join(", ")
        log "#{data_human}, queue_length: #{queue_length}"
      end  
    end
  end

  def self.log(msg)
    puts "[#{Time.now.strftime("%y-%m-%d %H:%M:%S")}] #{msg}"
  end

end

require "fnordmetric/inbound_stream"
require "fnordmetric/worker"
require "fnordmetric/namespace"
require "fnordmetric/gauge_modifiers"
require "fnordmetric/context"
require "fnordmetric/gauge"
require "fnordmetric/session"



#require "fnordmetric/app"
#require "fnordmetric/metric_api"
#require "fnordmetric/dashboard"
#require "fnordmetric/event"
#require "fnordmetric/cache"
#require "fnordmetric/report"
#require "fnordmetric/metric"
#require "fnordmetric/average_metric"
#require "fnordmetric/count_metric"
#require "fnordmetric/combine_metric"
#require "fnordmetric/sum_metric"
#require "fnordmetric/widget"
#require "fnordmetric/numbers_widget"
#require "fnordmetric/timeline_widget"
#require "fnordmetric/funnel_widget"
