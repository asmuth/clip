class FnordMetric::ZeroConfigGauge

  TYPES = [:_incr, :_decr, :_avg, :_min, :_max, :_set]

  Handler = proc do
    if data[:gauge]
      gauge_key = data[:gauge].to_sym
    else
      FnordMetric.error("missing key for zero config event: gauge")
      next
    end

    unless data[:value]
      FnordMetric.error("missing key for zero config event: value")
      next
    end

    gauge = if namespace.gauges.has_key?(gauge_key)
      namespace.gauges[gauge_key]
    else
      namespace.opt_gauge(gauge_key,
        :flush_interval => data[:flush_interval],
        :average => (type == :_avg),
        :zero_config => true)
    end

    case type

      when :_set
        set_value gauge, data[:value].to_i

      when :_incr
        incr_tick gauge, data[:value].to_i

      when :_decr
        FnordMetric.error("_decr is not yet implemented")

      when :_avg
        incr_avg gauge, data[:value].to_i

      when :_min, :_max
        FnordMetric.error("_min/_max is not yet implemented")

    end
  end

  class EmtpyGauge

    def self.tick
      FnordMetric.options[:default_flush_interval]
    end

    def self.retention
      tick * 10
    end

    def self.tick_at(time, _tick=tick)
      (time/_tick.to_f).floor*_tick
    end

    def group
      "Gauges"
    end

    def unit
      nil
    end

    def key_nouns
      ["Key", "Keys"]
    end

    def key(_append=nil)
      "----empty-gauge----"
    end

    def tick_key(_time, _append=nil)
      "----empty-gauge----"
    end

    def tick_keys(_range, _append=nil)
        ["----empty-gauge----"]
    end

    def retention_key(_time, _append=nil)
        "----empty-gauge----"
    end

    def two_dimensional?
      true
    end

    def three_dimensional?
      false
    end

    def progressive?
      false
    end

    def unique?
      false
    end

    def average?
      false
    end

    def has_series?
      false
    end

  end

  def initialize(key, namespace)
    @key = key
    @namespace = namespace
  end

  def name
    @key
  end

  def title
    @key
  end

  def method_missing(*msg)
    gauge = @namespace.gauges[@key]

    if gauge
      gauge.send(*msg)
    else
      EmtpyGauge.send(*msg)
    end
  end

end
