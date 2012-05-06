class FnordMetric::MultiGauge

  def initialize(opts)
    opts.fetch(:key) && opts.fetch(:key_prefix)
    @opts = opts
  end

  def name
    @opts[:key]
  end

  def title
    @opts[:title] || name
  end
  
  def key(_append=nil)
    [@opts[:key_prefix], "multigauge", name, _append].flatten.compact.join("-")
  end

  def add_redis(_redis)
    @opts[:redis] = _redis
  end

end