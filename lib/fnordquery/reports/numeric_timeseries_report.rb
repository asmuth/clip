class FnordQuery::NumericTimeseriesReport < FnordQuery::Report

  def execute(runner, _backend)
  	backend = _backend[0].new(_backend[1])

  	t_since = @opts["since"].to_i
  	t_until = @opts["until"].to_i

  	@tick = (t_until - t_since) / 150

  	@timeline = Hash[149.times.inject([t_until]) do |a, n|
  	  a + [a.last - n*@tick]
  	end.map do |t|
  	  [t, 0]
	end]

  	query = FnordQuery::Query.new("#{@opts["query"]} since(#{t_since}) until(#{t_until})")

  	backend.on_finish do
  	  render_result
  	  runner.send(:shutdown, true)
  	end

  	backend.subscribe(query) do |event|

  	  (event["_time"]-@tick..event["_time"]).to_a.detect do |t|
  	  	if @timeline.has_key?(t)
          @timeline[t] += 1
  	  	end
  	  end

  	end
  end

private

  def render_result
  	puts @timeline.inspect
  end

end