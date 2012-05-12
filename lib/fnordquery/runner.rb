class FnordQuery::Runner

  require "optparse"

  def initialize
    @opts = {}

    tasks    = %w(query web udp tcp exec)
    backends = %w(redis fyrehose)
    shorts   = { redis: :r, fyrehose: :x, query: :q }

    OptionParser.new do |opts|
      opts.on("-h", "--help") do
        print_usage
        exit!
      end
      opts.on("-v", "--version") do
        print_version
        exit!
      end
      { :task => tasks, :backend => backends }.each do |lkey, list|
        list.each do |key|
          okeys = ["--#{key} [ARG]"]
          okeys << "-#{shorts[key.to_sym]} [ARG]" if shorts[key.to_sym]
          opts.on(*(okeys << String)) do |arg|
            unless arg
              puts "error: missing argument: --#{key}"
              exit!(1)
            end
            if @opts[lkey]
              puts "error: only one of #{list.join(", ")} can be given"
              exit!(1)
            end
            @opts[lkey] = [key, arg]
          end
        end
      end
    end.parse!

    if [@opts[:task], @opts[:backend]].compact.size == 0
      print_usage
    elsif @opts[:task].nil?
      puts "error: no task given"
    elsif @opts[:backend].nil?
      puts "error: no backend specified"
    end

    @task = nil

    if @opts[:task].first == "query"
      begin
        @task = FnordQuery::Query.new(@opts[:task].last)
      rescue FnordQuery::Query::InvalidQueryError => e
        puts e.to_s; exit!(1)
      end
    end

    if %w(tcp udp).include?(@opts[:task].first)
      @task = FnordQuery::Acceptor.new(
        :protocol => @opts[:task].first.to_sym,
        :listen   => @opts[:task].last.split(":")
      )
    end

    puts @task.inspect

    EM.run do

      trap("TERM", &method(:shutdown))
      trap("INT",  &method(:shutdown))

      backend = get_backend(*@opts[:backend])

      EM.next_tick do
        @task.execute(self, backend)
      end

    end
  end

private

  def shutdown(graceful = false)
    puts "shutting down, byebye" unless graceful
    EM.stop
  end

  def get_backend(klass, opts)
    klass[0] = klass[0].upcase
    [FnordQuery.const_get("#{klass}Backend"), opts]
  end

  def print_usage
    print_version
    print_help
  end

  def print_version
    puts "fnordquery 0.0.1\n\n"
  end

  def print_help
    help = <<-EOH
      Usage: fnordquery [OPTIONS...]
        -r <address>      use redis backend
        -x <address>      use fyrehose backend

        --web <address>   start web interface
        --tcp <address>   listen on tcp for events
        --udp <address>   listen on udp for events
        --query <query>   print all matching events 
        --exec <file>     run this file
    EOH

    examples = <<-EOH
      Examples:
        fnordquery -f --emit "FILTER(_channel = 'fnord')"  
        fnordquery -r localhost:6379 --udp 0.0.0.0:2323
        fnordquery -r --web 0.0.0.0:8080
    EOH

    puts [help.lstrip.gsub(/^ {6}/, ""), nil]
    puts [examples.lstrip.gsub(/^ {6}/, ""), nil]
    puts "http://github.com/paulasmuth/fnordquery"
  end

end
