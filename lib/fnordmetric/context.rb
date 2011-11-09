class FnordMetric::Context

  def initialize(opts)
  	@opts = opts
  end

  def call(event)
  	@event = event
  	self.instance_eval(&@opts[:block])
  end

  private

  def data
  	@event
  end

end