require 'rubygems'
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

  def initialize(redis)
    @redis = redis
  end

  def method_missing(m, *args)
    puts ">> REDIS: #{m} #{args.join(" ")}"
    @last_return = @redis.send(m, *args)
    return self
  end

  def callback(&block)
    block.call(*@last_return)
  end

end