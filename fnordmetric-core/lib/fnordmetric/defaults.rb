FnordMetric::COLORS = ["#4572a7", "#aa4643", "#89a54e", "#80699b", "#3d96ae", "#db843d"].reverse

FnordMetric::DEFAULT_PROC = lambda{ |arg| }

FnordMetric::TICKS = lambda{ |tick, span| [tick, 60, 300, 1200, 3600, 86400]
  .select{ |t| (t >= tick) && ((span/t) > 5) }
  .uniq }

FnordMetric::DEFAULT_OPTIONS = {
  :redis_url => "redis://localhost:6379",
  :redis_prefix => "fnordmetric",
  :inbound_stream => nil,
  :inbound_protocol => nil,
  :web_interface => ["0.0.0.0", "4242"],
  :web_interface_server => "thin",
  :start_worker => true,
  :print_stats => 3,
  :event_queue_ttl => 120,
  :event_data_ttl => 3600*24*30,
  :session_data_ttl => 3600*24*30,
  :default_flush_interval => 10
}
