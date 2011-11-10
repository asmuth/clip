require 'rubygems'
require "eventmachine"
require 'em-hiredis'
require 'redis'
require 'yajl'

#require 'sinatra/base'
#require 'haml'

module FnordMetric

  @@namespaces = {}

  def self.namespace(key=nil, &block)    
    @@namespaces[key] = block    
  end

  def self.run(opts={})
    opts[:redis_prefix] ||= "fnordmetric"        

    EM.run do

      4.times do
        FnordMetric::Worker.new(@@namespaces.clone, opts)                
      end

      EventMachine::PeriodicTimer.new(1) do
        print_stats!(opts)
      end

    end


    #loop{ sleep 2; print_stats!(redis, opts[:redis_prefix]) }  
  end

  #def self.start_worker!(opts)      
  #  Process.fork do       
      
  #  end
  #end

  def self.print_stats!(opts)      
    #EM.defer proc{        
      redis = Redis.new
      stats_key = "#{opts[:redis_prefix]}-stats"
      time_str = Time.now.strftime("%y-%m-%d %H:%M:%S")
      data = {
        :events_processed => redis.hget(stats_key, :events_processed)
      }      
      puts "[#{time_str}] #{data.map{|k,v| "#{k} => #{v}"}.join(", ")}"
      #redis.keys("#{opts[:redis_prefix]}-stats*").each do |k|
    #}    
  end

end

require "fnordmetric/worker"
require "fnordmetric/namespace"
require "fnordmetric/context"
require "fnordmetric/gauge"


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
