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

  def matches?(_event)
    event = filter_flatten(_event)
    @filters.all? do |filter|
      if event[filter[0]].nil?
        false
      elsif filter[1] == :equals && filter[2].is_a?(String)
        event[filter[0]].to_s == filter[2]
      elsif filter[1] == :list_include && filter[2].first.is_a?(String)
        filter[2].include?(event[filter[0]].to_s)
      elsif filter[1] == :exists
        !!event[filter[0]]
      elsif !(event[filter[0]].to_s.match(/^[0-9]+(.[0-9]+)?$/))
        false
      elsif filter[1] == :equals && filter[2].is_a?(Fixnum)
        event[filter[0]].to_i == filter[2]
      elsif filter[1] == :equals && filter[2].is_a?(Float)
        event[filter[0]].to_f == filter[2]
      elsif filter[1] == :less_than && filter[2].is_a?(Integer)
        event[filter[0]].to_i < filter[2]
      elsif filter[1] == :less_than && filter[2].is_a?(Float)
        event[filter[0]].to_f < filter[2]
      elsif filter[1] == :greater_than && filter[2].is_a?(Integer)
        event[filter[0]].to_i > filter[2]
      elsif filter[1] == :greater_than && filter[2].is_a?(Float)
        event[filter[0]].to_f > filter[2]
      elsif filter[1] == :range_include && filter[2].is_a?(Range)
        filter[2].include?(event[filter[0]].to_f)
      elsif filter[1] == :list_include && filter[2].first.is_a?(Integer)
        filter[2].include?(event[filter[0]].to_i)
      elsif filter[1] == :list_include && filter[2].first.is_a?(Float)
        filter[2].include?(event[filter[0]].to_f)
      else
        false
      end
    end
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

  def eval_filter(arg)
    arg.gsub!("\\'", "\x7") # FIXPAUL: hack! ;)
    key_regex = "([^ ]+)"
    key_clean = lambda{ |s| 
      s.gsub("\x7", "'")
       .gsub(/([^\\])\./){ |m| m[0] + "\x06" }
       .gsub('\\.', ".") 
    }
    if m = arg.match(/^#{key_regex} *= *'([^']*)'$/)
      @filters << [key_clean[m[1]], :equals, m[2]]
    elsif m = arg.match(/^#{key_regex} *= *([0-9]+)$/)
      @filters << [key_clean[m[1]], :equals, m[2].to_i]
    elsif m = arg.match(/^#{key_regex} *= *([0-9]+\.[0-9]+)$/)
      @filters << [key_clean[m[1]], :equals, m[2].to_f]
    elsif m = arg.match(/^#{key_regex} *< *([0-9]+)$/)
      @filters << [key_clean[m[1]], :less_than, m[2].to_i]
    elsif m = arg.match(/^#{key_regex} *< *([0-9]+\.[0-9]+)$/)
      @filters << [key_clean[m[1]], :less_than, m[2].to_f]
    elsif m = arg.match(/^#{key_regex} *> *([0-9]+)$/)
      @filters << [key_clean[m[1]], :greater_than, m[2].to_i]
    elsif m = arg.match(/^#{key_regex} *> *([0-9]+\.[0-9]+)$/)
      @filters << [key_clean[m[1]], :greater_than, m[2].to_f]
    elsif m = arg.match(/^#{key_regex} *~ *([0-9]+)-([0-9]+)$/)
      @filters << [key_clean[m[1]], :range_include, (m[2].to_i..m[3].to_i)]
    elsif m = arg.match(/^#{key_regex} *~ *([0-9]+\.[0-9]+)-([0-9]+\.[0-9]+)$/)
      @filters << [key_clean[m[1]], :range_include, (m[2].to_f..m[3].to_f)]
    elsif m = arg.match(/^#{key_regex} *& *(([0-9]+),)+([0-9]+)$/)
      @filters << [key_clean[m[1]], :list_include, m[2..-1].map(&:to_i)]
    elsif m = arg.match(/^#{key_regex} *& *(([0-9]+\.[0-9]+),)+([0-9]+\.[0-9]+)$/)
      @filters << [key_clean[m[1]], :list_include, m[2..-1].map(&:to_f)]
    elsif m = arg.match(/^#{key_regex} *& *('[^']*',)+'[^']*'$/)
      @filters << [key_clean[m[1]], :list_include, arg
        .match(/^#{key_regex} *& *(.*)/)[2].scan(/'([^']*)',?/).map do |x|
          x.first.gsub("\x7", "'")
      end.to_a]
    elsif m = arg.match(/^#{key_regex}$/)
      @filters << [key_clean[m[1]], :exists, nil]
    else
      raise InvalidQueryError.new("invalid filter: filter(#{arg})")
    end
  end

  def parse_time(str)
    return :now     if str == "now"
    return str.to_i if str =~ /^[0-9]+$/
    raise InvalidQueryError.new("invalid time: #{str}")
  end

  def filter_flatten(hash, prefix = [])
    Hash[hash.inject([]) do |m, (k,v)| 
      m + (v.is_a?(Hash) ? filter_flatten(v, (prefix+[k])).to_a : [[(prefix+[k])*"\x06",v]])
    end]
  end
    
end

