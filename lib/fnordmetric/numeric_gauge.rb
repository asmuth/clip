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
    if args.size == 0 || (args.size == 1 && args.first.is_a?(Fixnum))
      incr_series(*args.unshift(:_default))
    elsif args.size == 1 || (args.size == 2 && args.last.is_a?(Fixnum))
      incr_series(*args)
    else
      raise "invalid arguments for incr: #{args.inspect}"
    end
  end

  def incr_series(series, value = 1)
    if (series == :_default) && @opts[:series].size > 1
      raise "don't know which series to increment - available: #{series}"
    elsif series == :_default
      series = @opts[:series].first
    end

    puts "increment #{series} by #{value}"
  end

end