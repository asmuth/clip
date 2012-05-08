require "eventmachine"
require "em-websocket"
require "em-hiredis"
require "json"

EventMachine.run do

    @opts = {
      :host => "0.0.0.0", 
      :port => 8080, 
      :debug => true
    }

    sub_redis = EM::Hiredis.connect
    pub_redis = EM::Hiredis.connect

    sub_redis.subscribe('fnordmetric')

    @chan_feed     = EM::Channel.new
    @chan_upstream = EM::Channel.new

    sub_redis.on(:message) do |channel, message|
      @chan_feed.push(JSON.parse(message)) rescue nil
    end

    @chan_upstream.subscribe do |message|
      puts "PUBLISHING"
      pub_redis.publish('fnordmetric', message.to_json)
    end

    EventMachine::WebSocket.start(@opts) do |socket|

      socket.onopen do

        sid = @chan_feed.subscribe do |messsage| 
          socket.send(messsage.to_json)
        end
        
        socket.onmessage do |message|
          begin
            message = JSON.parse(message)
          rescue
          else
            message["_eid"] ||= (8**64).to_s(36)
            @chan_upstream.push(message)
          end
        end

        socket.onclose do
          @chan_feed.unsubscribe(sid)
        end

      end

    end

end
