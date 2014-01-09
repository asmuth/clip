class FnordMetric::Logger

  def self.import(logfile_path)
    expire = FnordMetric.options[:event_queue_ttl]
    redis  = Redis.new
 
    @opts[:channels] ||= []
    @opts[:channels] = @opts[:channels].map(&:to_s) 

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

  def initialize(opts)
    @namespaces = FnordMetric.namespaces
    @opts = FnordMetric.options(opts)
    opts.fetch(:file)
    @channels = @opts[:channels] || []

    FnordMetric.register(self)
  end

  def initialized 
    logfile_path = @opts[:file]

    events = Queue.new
    dump_file = File.open(logfile_path, 'a+')

    Thread.new do
      loop do
        event = events.pop

        dump_file.write(event.to_json+"\n")
        dump_file.flush
      end
    end

    redis.pubsub.subscribe(pubsub_key) do |data|
      event = JSON.parse(data)
      events << event if log_channel?(event["_channel"])
    end

    FnordMetric.log "logging to #{logfile_path}"
  end


private

  def pubsub_key
    [@opts[:redis_prefix], 'announce'].join("-")
  end

  def redis
    @redis ||= EM::Hiredis.connect(FnordMetric.options[:redis_url])
  end

  def log_channel?(channel)
    return @channels.empty? if !channel
    @channels.include?(channel.to_s)
  end

end