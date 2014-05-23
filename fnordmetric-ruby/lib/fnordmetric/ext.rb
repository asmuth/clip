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

class Range

  def size
    self.last - self.first
  end

end

class Array

  def mean
    return 0 if empty?
    inject(&:+).to_f / size
  end

  alias :average :mean

  def median
    return 0 if empty?
    (_sorted = self.dup.sort)[_sorted.size/2]
  end

  def range
    return 0 if empty?
    max - min
  end

  def mode
    return 0 if empty?
    inject({}){ |h,v| h[v] = h[v].to_i+1; h }.to_a
    .sort{ |a,b| b.last <=> a.last }[0][0]
  end
  
  def emtpy
    self.size == 0
  end

end

class Thin::Connection

  alias :pre_process_orig :pre_process

  def pre_process
    @request.env['async.connection'] = self
    pre_process_orig
  end

end