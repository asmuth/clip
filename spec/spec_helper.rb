require 'rubygems'
require 'json'
require 'rspec'
require 'redis'
require 'rack'
require 'rack/test'
require 'delorean'

ENV['RACK_ENV'] = "test"
ENV['FNORDMETRIC_ENV'] = 'test'

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"
require "eventmachine"
require 'em-hiredis'

class RedisWrap

  def initialize(redis, callbackable=true)
    @redis = redis
    @callbackable = callbackable
  end

  def method_missing(m, *args)    
    @last_return = @redis.send(m, *args)
    if ENV["VERBOSE_REDIS"]
      puts ">> REDIS: #{m} #{args.join(" ")} => #{@last_return}" 
    end
    @callbackable ? self : @last_return
  end

  def callback(&block)
    block.call(*@last_return)
  end

end