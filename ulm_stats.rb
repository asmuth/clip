$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)

require "fnordmetric"

FnordMetric.namespace :blubber do

  gauge :events_total, :tick => 10
  gauge :uniques_daily, :tick => 1.day.to_i, :unique => true

  event :"*" do
    incr :events_total
    incr :uniques_daily
  end

  gauge :skip_votes, :tick => 60
  gauge :yes_votes, :tick => 60
  gauge :maybe_votes, :tick => 60

  gauge :mails_sent, :tick => 60
  gauge :mails_clicked, :tick => 60

  event(:skip_vote){ incr :skip_votes }
  event(:yes_vote){ incr :yes_votes }
  event(:maybe_vote){ incr :maybe_votes }

  widget 'Channels', {
    :title => "Daily Uniques",
    :type => :timeline,
    :width => 70,
    :gauges => :uniques_daily
  }

  widget 'Channels', {
    :title => "Events per Minute",
    :type => :timeline,
    :width => 30,
    :gauges => :events_total
  }

  widget 'Channels', {
    :title => "Yes/No/Skip-Votes",
    :type => :timeline,
    :include_current => false,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

end

fm_opts = {}
fm_opts.merge!(:web_interface => ["0.0.0.0", "2323"]) if ENV['DEV']

FnordMetric.run(fm_opts)
