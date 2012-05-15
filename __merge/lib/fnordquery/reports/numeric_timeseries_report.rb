class FnordQuery::NumericTimeseriesReport < FnordQuery::Report

  def execute(runner, _backend)
    super
    backend = _backend[0].new(_backend[1])

    colors = ["#2F635E", "#606B36", "#727070", "#936953", "#CD645A", "#FACE4F", "#42436B"]

    t_since = @opts["since"].to_i
    t_until = @opts["until"].to_i

    @tick = (t_until - t_since) / 150

    @series_timelines = Hash.new
    @series_queries = Hash.new

    timeline = Hash[149.times.inject([t_until]) do |a, n|
      a + [a.first - n*@tick]
    end.map do |t|
      [t, 0]
    end]

    @opts["series"].each do |skey, series|
      @opts["series"][skey]["color"] ||= colors.unshift(colors.pop).first
      @series_timelines[skey] = timeline.dup
      @series_queries[skey] = FnordQuery::Query.new("#{series["query"]}")
    end

    query = FnordQuery::Query.new("#{@opts["query"]} since(#{t_since}) until(#{t_until})")

    puts "tick: #{@tick}"
    puts "time: #{Time.at(t_since).to_s}-#{Time.at(t_until).to_s}"

    backend.on_finish do
      render_result
      runner.send(:shutdown, true)
    end

    backend.subscribe(query) do |event|

      @series_queries.each do |skey, squery|
        if squery.matches?(event)
          (event["_time"]-@tick..event["_time"]).to_a.reverse.detect do |t|
            if @series_timelines[skey].has_key?(t)
              @series_timelines[skey][t] += 1
            end
          end
        end
      end

    end
  end

private

  def render_result
    @series_render = @opts["series"].map do |skey, sopts|
      {
        :data  => @series_timelines[skey].map { |t,v| 
          { :x => t, :y => v } 
        }.reverse,
        :name  => sopts["name"],
        :color => sopts["color"]
      }
    end
    render_haml(:numeric_timeseries_report, 'report.html')
  end

end