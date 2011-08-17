module FnordMetric 

  def self.included(base)
    base.class_eval do

      def self.create_datapoint(hash)
        self.create(:data => hash.to_json)
      end

    end
  end

end

require "fnordmetric/base"