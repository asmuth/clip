_opts = FnordMetric.options

if _opts[:web_interface]
  FnordMetric::Web.new(
    :host => _opts[:web_interface][0],
    :port => _opts[:web_interface][1]
  )
end

if _opts[:inbound_stream]
  FnordMetric::Acceptor.new(
    :protocol => _opts[:inbound_protocol],
    :host => _opts[:inbound_stream][0],
    :port => _opts[:inbound_stream][1]
  )
end

if _opts[:start_worker]
  FnordMetric::Worker.new()
end