class FnordMetric::Logger

  def self.start(logfile_path)
    require 'json'
    event_ids = Queue.new
    dump_file = File.open(logfile_path, 'a+')

    fetcher = Thread.new do
      redis = Redis.new
      loop do
        event_id = event_ids.pop
        event_data = redis.get("fnordmetric-event-#{event_id}")
        event_hash = MultiJson.decode(event_data) rescue next

        event_hash.merge!(:_time => Time.now.to_i)

        dump_file.write(event_hash.to_json+"\n")
        dump_file.flush

        print "\033[1;34m"
        print event_hash.inspect
        print "\033[0m\n"
      end
    end

    listener = Thread.new do
      redis = Redis.new
      redis.subscribe("fnordmetric-announce") do |on|
        on.message do |channel, event_id|
          event_ids << event_id
        end
      end
    end

    fetcher.join
  end

  def self.import(logfile_path)
    redis = Redis.new
    dump_file = File.open(logfile_path, 'r')

    puts "reading #{logfile_path}..."
    dump_lines = dump_file.read.split("\n")

    puts "importing #{dump_lines.length} events..."
    pre_uuid = rand(999999999999999999999)
    log_every = (dump_lines.length / 150)
    dump_lines.each_with_index do |line,n|
      puts "#{n}/#{dump_lines.length} (#{((n/dump_lines.length.to_f)*100).to_i}%)" if n%log_every==0
      my_uuid = "#{pre_uuid}-#{n}"
      redis.set("fnordmetric-event-#{my_uuid}", line)
      redis.lpush("fnordmetric-queue", my_uuid)
      redis.expire("fnordmetric-event-#{my_uuid}", 3600*12)
    end
  end

end
