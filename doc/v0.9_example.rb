$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.namespace :ulm do

  gauge :skip_votes, :tick => 1.day.to_i, :title => "Skip-Votes"
  gauge :yes_votes, :tick => 1.day.to_i, :title => "Yes-Votes"
  gauge :maybe_votes, :tick => 1.day.to_i, :title => "Maybe-Votes"

  gauge :messages_sent, :tick => 1.day.to_i, :title => "Messages (sent)"
  gauge :messages_read, :tick => 1.day.to_i, :title => "Messages (read)"
  gauge :winks_sent, :tick => 1.day.to_i, :title => "Winks sent"


  gauge :gross_search_volume,
    :type => :numeric_timeseries,
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

FnordMetric.standalone
