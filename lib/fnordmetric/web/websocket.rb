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
    @reactor = FnordMetric::Reactor.new

    @uuid = "websocket-#{get_uuid}"

    start_websocket
  end

private

  def start_websocket
    EventMachine::WebSocket.start(@opts) do |socket|
      socket.onopen do

        socket.onmessage do |message|
          puts "received: #{message}"
          begin
            message = JSON.parse(message)
          rescue
            puts "websocket: invalid json"
          else
            message["_eid"] ||= get_uuid
            message["_sender"] = @uuid
            msg = @reactor.execute(socket, message) # FIXPAUL
          end
        end

      end
    end
  end

  def get_uuid
    rand(8**64).to_s(36)
  end

end