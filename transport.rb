require "rubygems"
require "eventmachine"
require "yajl"
require "redis"

module FnordTransport  

  def post_init
     @parser = Yajl::Parser.new(:symbolize_keys => true)
     @parser.on_parse_complete = method(:event_received)
     @redis = Redis.new
  end

  def event_received(event)
    event.merge!(:received_at => Time.now.getutc.to_i)    
    my_uuid = rand(9999999999999999999).to_s
    @redis.lpush("fnordmetric-queue", my_uuid) 
    @redis.set("fnordmetric-event-#{my_uuid}", Yajl::Encoder.encode(event))
  end


  def receive_data(data)
    @parser << data
  end

end


EventMachine::run do  
  EventMachine::start_server "0.0.0.0", 1337, FnordTransport
end

