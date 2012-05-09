class FnordMetric::Widget

  attr_accessor  :key

  def initialize(opts={})
    @handlers = {}
    @opts = opts
    @key = rand(8**32).to_s(36)

    unless opts.has_key?(:title)
      error! "widget can't be initialized without a title"
    end
  end

  def on(handler, &block)
    @handlers[handler.to_s] = block
    self
  end

  def opts
    { 
      :width => 100,
      :klass => self.class.name.split("::").last,
      :widget_key => @key,
      :key_nouns => ["Key", "Keys"]
    }.merge(@opts)
  end

private

  def call_handler(handler, *args)
    raise "handler not defined" unless @handlers.has_key?(handler.to_s)
    @handlers[handler.to_s].call(*args)
  end

end