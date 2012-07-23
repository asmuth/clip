module FnordMetric::GaugeValidations

  def validate_series!
    if !@opts[:series].is_a?(Array) || @opts[:series].size == 0
      raise "#{@opts[:key]}: missing option series"
    end

    if @opts[:series].size != @opts[:series].uniq.size
      raise "#{@opts[:key]}: series are not unique"
    end

    @opts[:series] = @opts[:series].map(&:to_sym)
  end

end