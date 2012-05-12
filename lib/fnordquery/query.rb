class FnordQuery::Query
  
  class InvalidQueryError < StandardError; end

  X_VALIDATE = /^(([a-z]+\([^\)]*\)) *)+$/
  X_EXTRACT  = /(([a-z]+)\(([^\)]*)\))/
  FUNCTIONS  = %w(filter since until stream)

  attr_accessor :raw, :since, :until, :filters

  def initialize(str)
    @raw = str
    @filters = []

    unless str.match(X_VALIDATE)
      raise InvalidQueryError.new("invalid query: #{str}") 
    end

    str.scan(X_EXTRACT) do |part|
      eval_part(*part[1..-1])
    end

    @since ||= :now
    @until ||= :now
  end

  def execute(runner, _backend)
    backend = _backend[0].new(_backend[1])
    backend.on_finish do
      runner.send(:shutdown, true)
    end
    backend.subscribe(self) do |event|
      puts event.inspect
    end
  end

  def matches?(event)
    true
  end

private

  def eval_part(func, arg) 
    unless FUNCTIONS.include?(func)
      raise InvalidQueryError.new("unknown function: ... #{func}(#{arg}) ...") 
    end
    send("eval_#{func}", arg)
  end

  def eval_stream(arg)
    unless arg == ""
      raise InvalidQueryError.new("too many arguments: ... stream(#{arg}) ...") 
    end
    unless @until.nil?
      raise InvalidQueryError.new("until specified twice: ... stream(#{arg}) ...")  
    end
    @until = :stream
  end

  def eval_since(arg)
    unless @since.nil?
      raise InvalidQueryError.new("since specified twice: ... since(#{arg}) ...")  
    end
    @since = parse_time(arg)
  end

  def eval_until(arg)
    unless @until.nil?
      raise InvalidQueryError.new("since specified twice: ... until(#{arg}) ...")
    end
    @until = parse_time(arg)
  end

  def parse_time(str)
    return :now     if str == "now"
    return str.to_i if str =~ /^[0-9]+$/
    raise InvalidQueryError.new("invalid time: #{str}")
  end
    
end

