class FnordMetric::Reactor

  def initialize
    @namespaces = FnordMetric.namespaces
  end

  def execute(event)
  	return false unless event["type"] == "widget_request"
  	return false unless @namespaces.keys.include?(event["namespace"].to_sym)
  	"FnordMetric::#{event["klass"]}".constantize.execute(@namespaces[event["namespace"].to_sym], event) # FIXPAUL
  end

end