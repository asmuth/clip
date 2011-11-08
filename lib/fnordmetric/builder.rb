class FnordMetric::Builder

  def initialize(opts={})  	

  	opts[:redis] = Redis.new
  	opts[:redis_prefix] ||= "fnordmetric"    
    
    4.times do 
      start_worker!(opts)
    end

    puts "started!"

    loop{ sleep 1 }
    #Thread.stop
  end

  def start_worker!(opts)
    Process.fork do        
      FnordMetric::Worker.new(opts)
    end
  end

end