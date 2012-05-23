$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.options = {
  :event_queue_ttl  => 10, # all data that isn't processed within 10s is discarded to prevent memory overruns
  :event_data_ttl   => 10,
  :session_data_ttl => 1,  # we don't care about session data for now
  :redis_prefix => "fnordmetric" 
}




FnordMetric.namespace :myapp do

  hide_overview
  hide_active_users

  timeseries_gauge :number_of_signups,
    :title => "Number of Signups",
    :series => [:signup_via_twitter, :signup_via_openid],
    :resolution => 30.seconds


  distribution_gauge :user_age_distribution,
    :title => "User Age Distribution",
    :resolution => 30.seconds


  toplist_gauge :popular_keywords,
    :title => "Popular Keywords",
    :resolution => 30.seconds
  

  gauge :events_per_hour, :tick => 1.hour
  gauge :events_per_second, :tick => 1.second
  gauge :events_per_minute, :tick => 1.minute

  event :"*" do
    incr :events_per_hour
    incr :events_per_minute
    incr :events_per_second
  end

  widget 'TechStats', {
    :title => "Events per Minute",
    :type => :timeline,
    :width => 100,
    :gauges => :events_per_minute,
    :include_current => true,
    :autoupdate => 30
  }

  widget 'TechStats', {
    :title => "Events per Hour",
    :type => :timeline,
    :width => 50,
    :gauges => :events_per_hour,
    :include_current => true,
    :autoupdate => 30
  }


  widget 'TechStats', {
    :title => "Events/Second",
    :type => :timeline,
    :width => 50,
    :gauges => :events_per_second,
    :include_current => true,
    :plot_style => :areaspline,
    :autoupdate => 1
  }

  widget 'TechStats', {
    :title => "Events Numbers",
    :type => :numbers,
    :width => 100,
    :gauges => [:events_per_second, :events_per_minute, :events_per_hour],
    :offsets => [1,3,5,10],
    :autoupdate => 1
  }

end


def start_example_data_generator

  api = FnordMetric::API.new
  Thread.new do
    loop do      
      api.event(:_type => :unicorn_seen)
      sleep rand(10)/10.to_f
    end
  end

end

start_example_data_generator

FnordMetric::Web.new(:port => 4242)
FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)
FnordMetric::Worker.new
FnordMetric.run



