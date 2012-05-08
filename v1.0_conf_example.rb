
FnordMetric.options = {
  :redis_url => "fnord"
}

FnordMetric::Acceptor.new(:protocol => :udp, :port => 2323)
FnordMetric::Web.new(:host => "0.0.0.0", :port => 4242)
FnordMetric::Logger.new('/home/paul/fnordmetric.log')

FnordMetric::NumericGauge.new(
  :title => "fnord..."
)

FnordMetric::EventHandler do |event|
  puts "received: " + event.inspect
end