class ZeroConfigGauge

  class EmtpyGauge

    def self.tick
      60
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
