class FnordMetric::Widget

  attr_accessor  :key

  def initialize(opts={})
    @opts = opts
    @key = rand(8**32).to_s(36)

    unless opts.has_key?(:title)
      error! "widget can't be initialized without a title"
    end
  end

  def on(handler, &block)
  end

  def opts
    { 
      :width => 100,
      :klass => self.class.name.split("::").last,
      :widget_key => @key,
      :key_nouns => ["Key", "Keys"]
    }.merge(@opts)
  end

end