class FnordMetric::NumericGauge < FnordMetric::MultiGauge

  def initialize(opts)
    @cmds = [:incr]

    super(opts)

    if !opts[:series].is_a?(Array) || opts[:series].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option series"
    end

    if !opts[:ticks].is_a?(Array) || opts[:ticks].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option ticks"
    end

    opts[:series] = opts[:series].map(&:to_sym)

    if opts[:ticks].any?{ |t| !t.is_a?(Fixnum) }
      raise "numeric_gauge #{opts[:key]}: these are not numbers: #{opts[:ticks].inspect}"
    end

    if opts[:series].size != opts[:series].uniq.size
      raise "numeric_gauge #{opts[:key]}: series are not unique"
    end
  end

  def incr(*args)
    puts "inrcrement called: #{args.inspect}"
  end

end