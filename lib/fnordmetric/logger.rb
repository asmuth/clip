class FnordMetric::Logger

  def self.start(logfile_path)    
    require 'json'
    events = Queue.new
    dump_file = File.open(logfile_path, 'a+')

    fetcher = Thread.new do
      redis = Redis.new
      loop do
        event = events.pop

        dump_file.write(event+"\n")
        dump_file.flush
      end
    end

    listener = Thread.new do  
      redis = Redis.new
      redis.subscribe("fnordmetric-announce") do |on|
        on.message do |channel, event|
          events << event
        end
      end
    end

    listener.join
  end

  def self.import(logfile_path)
    expire = FnordMetric.options[:event_queue_ttl]
    redis  = Redis.new

    dump_file = File.open(logfile_path, 'r')
    num_lines = %x{wc -l #{logfile_path}}.to_i
    puts "importing #{num_lines} events..."

    dump_file.each_with_log(num_lines) do |line, ind|
      (8**64).to_s(36).tap do |uuid|
        redis.set    "fnordmetric-event-#{uuid}", line
        redis.lpush  "fnordmetric-queue"        , uuid
        redis.expire "fnordmetric-event-#{uuid}", expire
      end
    end
  end

end