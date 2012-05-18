module Enumerable

  def each_with_log(total = nil, &block)
    log_every = ((total ||= self.count) / 150)
    self.each_with_index do |item, index|
      if index % log_every == 0
        STDOUT.puts "#{index}/#{total} (#{((index/total.to_f)*100).to_i}%)" 
      end
      block.call(item, index)
    end
  end

end

module Haml::Filters::Gaugejs
  include Haml::Filters::Base

  def render(text)
    "<FNORDMETRIC-GAUGEJS>#{text}</FNORDMETRIC-GAUGEJS>"
  end
end

class Symbol
  alias :intern :to_sym
end
