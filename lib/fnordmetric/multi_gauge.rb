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

  def method_missing(method, *args, &block)
    if (m = method.to_s.match(/cmd_([a-zA-Z_]+)/))
      method = m[1]
      unless @cmds.try(:include?, method.to_sym)
        puts "error: unknown command: #{method}"  
      else
        send(method, *args, &block)
      end
    end
    puts "error: unknown command: #{method}"
  end

end