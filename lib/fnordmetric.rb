module FnordMetric 

  def self.included(base)
    base.class_eval do

      def self.foobar(hash)      	      	      
      end

    end
  end

end

require "fnordmetric/report"
require "fnordmetric/event"