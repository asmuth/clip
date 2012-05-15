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

    @backend = FnordMetric.backend

    @uuid = "websocket-#{get_uuid}"

    start_websocket
  end

private

  def start_websocket
    EventMachine::WebSocket.start(@opts) do |socket|
      socket.onopen do

        subscribed_channles = []

        @backend.subscribe do |message| 
          if message["_sender"] != @uuid &&
            (message["_class"] == "widget_response" ||
             message["_class"] == "widget_push" ||
             message["_class"] == "discover_response" ||
             message["_class"] == "render_response")
            socket.send(message.to_json)
          end
        end
        
        socket.onmessage do |message|
          puts "received: #{message}"
          begin
            message = JSON.parse(message)
          rescue
            puts "websocket: invalid json"
          else
            message["_eid"] ||= get_uuid
            message["_sender"] = @uuid
            @backend.publish(message)
          end
        end

        socket.onclose do
          @backend.hangup
        end

      end
    end
  end

  def get_uuid
    rand(8**64).to_s(36)
  end

end