class FnordQuery::Runner

  require "optparse"

  def initialize
    @opts = {}
    @task_opts = {}

    tasks       = %w(query web udp tcp exec)
    backends    = %w(redis fyrehose)
    executables = %w(ReportBuilder CategoricalTopKReport NumericTimeseriesReport)
    shorts      = { redis: :r, fyrehose: :x, query: :q }

    OptionParser.new do |opts|
      opts.on("-h", "--help") do
        print_usage
        exit!
      end
      opts.on("-v", "--version") do
        print_version
        exit!
      end
      opts.on("--force") do
        @task_opts.merge!(:force => true) 
      end
      opts.on("-p [PATH]", "--path [PATH]") do |path|
        @task_opts.merge!(:path => path) 
      end
      %w(since until).each do |key|
        opts.on("--#{key} [ARG]") do |arg|
          @task_opts.merge!(key.to_sym => arg)
        end
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
            if lkey == :backend
              @task_opts[:opt_str] = [
                @task_opts[:opt_str], "--#{key} #{arg}"
              ].compact.flatten
            end
            @opts[lkey] = [key, arg]
          end
        end
      end
    end.parse!

    if [@opts[:task], @opts[:backend]].compact.size == 0
      print_usage
      exit!(1)
    elsif @opts[:task].nil?
      puts "error: no task given"
      exit!(1)
    elsif @opts[:backend].nil?
      puts "error: no backend specified"
      exit!(1)
    end

    @task = nil
    @task_opts[:path] ||= '/tmp/fnordquery/'

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

    if @opts[:task].first == "exec"
      execcfg = JSON.parse(File.open(@opts[:task].last).read)
      if !execcfg["klass"] || !executables.include?(execcfg["klass"])
        puts "error: unknown executable: #{execcfg["klass"]}"
        exit!(1)
      end
      execcfg[:basedir] ||= File.expand_path("../", @opts[:task].last)
      execcfg.merge!(@task_opts)
      @task = FnordQuery.const_get(execcfg["klass"]).new(execcfg)
    end

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
        -p <path>         set basedir (/tmp/fnordquery)

        --web <address>   start web interface
        --tcp <address>   listen on tcp for events
        --udp <address>   listen on udp for events
        --query <query>   print all matching events 
        --exec <file>     run this file
    EOH

    examples = <<-EOH
      Examples:
        fnordquery -f --query "FILTER(_channel = 'fnord')"  
        fnordquery -r localhost:6379 --udp 0.0.0.0:2323
        fnordquery -r --web 0.0.0.0:8080
    EOH

    puts [help.lstrip.gsub(/^ {6}/, ""), nil]
    puts [examples.lstrip.gsub(/^ {6}/, ""), nil]
    puts "http://github.com/paulasmuth/fnordquery"
  end

end
