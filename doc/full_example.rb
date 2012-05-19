$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.options = {
  :event_queue_ttl  => 10, # all data that isn't processed within 10s is discarded to prevent memory overruns
  :event_data_ttl   => 10,
  :session_data_ttl => 1,  # we don't care about session data for now
  :redis_prefix => "fnordmetric-dawanda" 
}

FnordMetric.namespace :ulm do

  gauge :skip_votes, :tick => 1.day.to_i, :title => "Skip-Votes"
  gauge :yes_votes, :tick => 1.day.to_i, :title => "Yes-Votes"
  gauge :maybe_votes, :tick => 1.day.to_i, :title => "Maybe-Votes"

  gauge :messages_sent, :tick => 1.day.to_i, :title => "Messages (sent)"
  gauge :messages_read, :tick => 1.day.to_i, :title => "Messages (read)"
  gauge :winks_sent, :tick => 1.day.to_i, :title => "Winks sent"


  timeseries_gauge :gross_search_volume,
    :tick => 30.seconds,
    :group => "Search",
    :title => "Gross Search Volume",
    :key_nouns => ["Search", "Searches"],
    :series => [:without_filter, :with_filter]
    #:enable_punchcard => true,
    #:enable_realtime => true


  widget 'Overview', {
    :title => "Yes/No/Skip-Votes",
    :type => :timeline,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'Overview', {
    :title => "Yes/No/Skip Numbers",
    :type => :numbers,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'Overview', {
    :title => "Messages sent/read",
    :type => :timeline,
    :gauges => [:messages_sent, :messages_read, :winks_sent]
  }

  widget 'Overview', {
    :title => "Messages sent+read/Winks Numbers",
    :type => :numbers,
    :gauges => [:messages_sent, :messages_read, :winks_sent]
  }
  
  widget 'Overview', {
    :title => "User-Activity",
    :type => :timeline,
    :width => 67,
    :autoupdate => 30,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

end

FnordMetric.options = {
  :print_stats => 3
}

FnordMetric::Logger.new(
  :file => '/home/paul/fnordmetric.log',
  :channels => ["dawanda-firehose"]
)

FnordMetric::Web.new(:port => 4242)

FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)

FnordMetric.run