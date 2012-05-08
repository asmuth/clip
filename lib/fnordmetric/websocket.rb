require "eventmachine"
require "em-websocket"
require "em-hiredis"
require "json"

class FnordMetric::WebSocket

  def initialize(opts)
    @opts = {
      :host => "0.0.0.0", 
      :port => 8080
    }.merge(opts)

    start_websocket
  end

private

  def start_websocket
    EventMachine::WebSocket.start(@opts) do |socket|
      socket.onopen do

        subscribed_channles = []

        sid = FnordMetric.chan_feed.subscribe do |messsage| 
          socket.send(messsage.to_json)
        end
        
        socket.onmessage do |message|
          begin
            message = JSON.parse(message)
          rescue
          else
            message["_eid"] ||= (8**64).to_s(36)
            FnordMetric.chan_upstream.push(message)
          end
        end

        socket.onclose do
          FnordMetric.chan_feed.unsubscribe(sid)
        end

      end
    end
  end

end