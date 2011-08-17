module FnordMetric 

  def self.included(base)
    base.class_eval do

      def self.insert(hash)
        self.create(:data => hash.to_json)
      end

      def self.generate_report(options={})
      	FnordMetric::Report.new(self, options)
      end

    end
  end

end

require "fnordmetric/report"