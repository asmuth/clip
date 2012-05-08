require "eventmachine"
require "em-websocket"
require "em-hiredis"
require "json"

class FnordMetric::WebSocket

  def initialize(opts)
    @opts = {
      :host => "0.0.0.0", 
      :port => 8080, 
      :debug => true
    }.merge(opts)

    start_websocket
  end

private

  def start_websocket
    EventMachine::WebSocket.start(@opts) do |socket|
      socket.onopen do

        sid = @opts[:chan_feed].subscribe do |messsage| 
          socket.send(messsage.to_json)
        end
        
        socket.onmessage do |message|
          begin
            message = JSON.parse(message)
          rescue
          else
            message["_eid"] ||= (8**64).to_s(36)
            @opts[:chan_upstream].push(message)
          end
        end

        socket.onclose do
          @opts[:chan_feed].unsubscribe(sid)
        end

      end
    end
  end

end