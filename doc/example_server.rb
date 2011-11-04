$: << ::File.expand_path('../../lib', __FILE__)

require "rubygems"
require "fnordmetric"
require "thin"

FnordMetric.metric(:passengers_total, :count => true, :types => [:car_seen])
FnordMetric.metric(:blubbs, :sum => :passengers, :types => [:car_seen])
FnordMetric.metric(:passengers_red_car, :sum => :passengers, :filter => { :colors => :red }, :types => [:car_seen]) 
FnordMetric.metric(:passengers_blue_car, :sum => :passengers, :filter => { :colors => :blue }, :types => [:car_seen]) 

FnordMetric.metric(:blue_to_red_ratio, :combine => lambda{ |x|
  x.passengers_blue_car / x.passengers_red_car
})


FnordMetric.dashboard 'Passengers' do |passengers| 
   
	passengers.add_widget FnordMetric.widget(:passengers_total_timeline, 
	  :metrics => :passengers_total,
	  :tick => 4.minutes,
	  :title => "Passengers total", 
	  :type => :timeline
	)

	passengers.add_widget FnordMetric.widget(:passenger_blue_red_timeline, 
	  :metrics => [:passengers_blue_car, :passengers_red_car], 
	  :title => "Passengers (red/blue)", 
	  :type => :timeline
	)
	
	passengers.add_widget FnordMetric.widget(:passenger_br_ratio_timeline, 
	  :metrics => :blue_to_red_ratio,
	  :title => "Passenger blue/red Ratio", 
	  :type => :timeline
	)

end

FnordMetric.dashboard 'Cars & Bikes' do |cars| 
   
	cars.add_widget FnordMetric.widget(:passenger_br_ratio_timeline, 
	  :metrics => :blue_to_red_ratio, 
	  :title => "Passenger blue/red Ratio", 
	  :type => :timeline
	)

end


Mongoid.configure do |c| 
  c.master = Mongo::Connection.new.db("myfnordmetric") 
end

app = FnordMetric::App.new
Thin::Server.start('127.0.0.1', 2323, app)
