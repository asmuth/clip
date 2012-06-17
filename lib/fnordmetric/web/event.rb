module FnordMetric
  class Event  

    attr_accessor :time, :type, :event_id

    #def self.track!(event_type, event_data)
    #end

    def self.all(opts)
      opts[:limit] ||= 100

      range_opts = { :withscores => true }
      range_opts.merge!(:limit => [0,opts[:limit]]) if opts[:limit]

      events = opts[:redis].zrevrangebyscore(
        "#{opts[:namespace_prefix]}-timeline", 
        '+inf', opts[:since]||'0',
        range_opts
      )

      events = events[0..opts[:limit] - 1]

      unless events.first.is_a?(Array)
        events = events.in_groups_of(2).map do |event_id, ts|
          [event_id, Float(ts)]
        end
      end

      events.map do |event_id, ts|
        next if event_id.blank?
        find(event_id, opts).tap{ |e| e.time = "%.f" % ts }
      end
    end

    def self.by_type(_type, opts)
      opts[:redis].lrange(
        "#{opts[:namespace_prefix]}-type-#{_type}", 
        0, 200).map do |event_id|
        next if event_id.blank?
        find(event_id, opts).tap{ |e| }
      end
    end

    def self.by_session_key(_session_key, opts)
      session = Session.find(_session_key, opts)
      session.fetch_event_ids!(200).reject(&:blank?).map do |event_id|
        find(event_id, opts)
      end
    end

    def self.find(event_id, opts)
      self.new(event_id, opts).tap do |event|
        event.fetch!
      end
    end

    def initialize(event_id, opts)
      @opts = opts
      @event_id = event_id
    end

    def fetch!
      @data = JSON.parse(fetch_json).tap do |event|
        @type = event.delete("_type")
      end
    end

    def fetch_json
      @opts[:redis].get(redis_key) || "{}"
    end

    def redis_key
      [@opts[:redis_prefix], :event, @event_id].join("-")
    end

    def session_key
      @data["_session"] ? Digest::MD5.hexdigest(@data["_session"]) : nil
    end

    def id
      @event_id
    end

    def data(key=nil)
      key ? @data[key.to_s] : @data
    end

    alias :[] :data

    def to_json
      @data.merge!(
        :_type => @type, 
        :_session_key => session_key,
        :_eid => @event_id,
        :_time => @time
      )
    end
  end
end
