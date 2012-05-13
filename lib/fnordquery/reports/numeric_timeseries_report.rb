class FnordQuery::NumericTimeseriesReport < FnordQuery::Report

  def execute(runner, backend)
  	runner.send(:shutdown, true)
  end

end