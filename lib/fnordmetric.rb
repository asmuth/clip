module FnordMetric 

  def self.included(base)
    base.class_eval do

      def self.insert(hash)      	      	
      	at_time = hash.delete(:time)
      	self.create(:data => hash.to_json).tap do |object|
      		object.update_attribute(:created_at, at_time) if at_time
      	end        
      end

      def self.generate_report(options={})
      	FnordMetric::Report.new(self, options)
      end

    end
  end

end

require "fnordmetric/report"