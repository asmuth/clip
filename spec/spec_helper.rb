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

def EM.defer; yield; end
def EM.next_tick; yield; end

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

def key_error_klass
  RUBY_VERSION =~ /1.9.\d/ ? KeyError : IndexError
end