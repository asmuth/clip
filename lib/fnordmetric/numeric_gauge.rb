class FnordMetric::NumericGauge < FnordMetric::MultiGauge

  def initialize(opts)
    opts.fetch(:key) && opts.fetch(:key_prefix)
    @opts = opts
  end

end