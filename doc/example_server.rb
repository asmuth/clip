$: << ::File.expand_path('../../lib', __FILE__)

require "rubygems"
require "fnordmetric"
require "thin"

FnordMetric.define(:passengers_total, :sum => :passengers, :types => [:car_seen])
FnordMetric.define(:blubbs, :sum => :passengers, :types => [:car_seen])
FnordMetric.define(:passengers_red_car, :sum => :passengers, :filter => { :colors => :red }, :types => [:car_seen]) 
FnordMetric.define(:passengers_blue_car, :sum => :passengers, :filter => { :colors => :blue }, :types => [:car_seen]) 

FnordMetric.define(:blue_to_red_ratio, :combine => lambda{ |x|
  x.passengers_blue_car / x.passengers_red_car
})


FnordMetric.dashboard 'Passengers' do |passengers| 
   
	passengers.add_widget FnordMetric.widget(:passenger_br_ratio_graph, 
	  :metrics => :blue_to_red_ratio, 
	  :title => "Passenger blue/red Ratio", 
	  :type => :graph
	)

	passengers.add_widget FnordMetric.widget(:passenger_blue_red_graph, 
	  :metrics => [:passengers_blue_car, :passengers_red_car], 
	  :title => "Passengers (red/blue)", 
	  :type => :graph
	)
	
	passengers.add_widget FnordMetric.widget(:passengers_total_graph, 
	  :metrics => :passengers_total,
	  :title => "Passenger blue/red Ratio", 
	  :type => :graph
	)

end

Mongoid.configure do |c| 
  c.master = Mongo::Connection.new.db("myfnordmetric") 
end

app = FnordMetric::App.new
Thin::Server.start('127.0.0.1', 2323, app)
