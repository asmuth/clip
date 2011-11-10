require 'rubygems'
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

    redis = Redis.new
    
    1.times do 
      start_worker!(opts)
    end

    loop{ sleep 2; print_stats!(redis, opts[:redis_prefix]) }  
  end

  def self.start_worker!(opts)      
    Process.fork do       
      FnordMetric::Worker.new(@@namespaces.clone, opts)
    end
  end

  def self.print_stats!(redis, prefix)
    redis.keys("#{prefix}-stats*").each do |k|
      t = Time.now.strftime("%y-%m-%d %H:%M:%S")
      puts "[#{t}] #{k.gsub("#{prefix}-stats-", '')} => #{redis.get(k)}"
    end
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
