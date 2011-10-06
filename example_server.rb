$: << ::File.expand_path('../lib', __FILE__)

require "rubygems"
require "fnordmetric"
require "thin"

FnordMetric.define(:passengers_total, :sum => :passengers, :types => [:car_seen])
FnordMetric.define(:passengers_red_car, :sum => :passengers, :filter => { :colors => :red }, :types => [:car_seen]) 
FnordMetric.define(:passengers_blue_car, :sum => :passengers, :filter => { :colors => :blue }, :types => [:car_seen]) 

FnordMetric.define(:blue_to_red_ratio, :combine => lambda{ |x|
  x.passengers_blue_car / x.passengers_red_car
})

FnordMetric.dashboard 'Passengers' do |passengers|    
  passengers.widget :blue_to_red_ratio, :title => "Passenger blue/red Ratio", :type => :graph
  passengers.widget [:passengers_blue_car, :passengers_red_car], :title => "Passengers (red/blue)", :type => :graph
  passengers.widget :passengers_total, :title => "Passengers (total)", :type => :graph
end  

Mongoid.configure do |c| 
  c.master = Mongo::Connection.new.db("myfnordmetric") 
end

app = FnordMetric::App.new
Thin::Server.start('127.0.0.1', 2323, app)
