class FnordMetric::ToplistGauge < FnordMetric::MultiGauge

  def initialize(opts)
    @cmds = [:observe]

    super(opts)

    if !opts[:ticks].is_a?(Array) || opts[:ticks].size == 0
      raise "numeric_gauge #{opts[:key]}: missing option ticks"
    end

    if opts[:ticks].any?{ |t| !t.is_a?(Fixnum) }
      raise "numeric_gauge #{opts[:key]}: these are not numbers: #{opts[:ticks].inspect}"
    end
  end

  def render
    super.merge(
      :template => render_template(:toplist_gauge),
      :widgets => {

      }
    )
  end

  def fetch_gauge(series, tick)
    nil
  end

end