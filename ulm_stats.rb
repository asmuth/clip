$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

# todos: old stylesheet, numbers widget, dump-to-file(+reimport)


FnordMetric.namespace :ulmstats do

  # user activity
  gauge :skip_votes, :tick => 1.day.to_i
  gauge :yes_votes, :tick => 1.day.to_i
  gauge :maybe_votes, :tick => 1.day.to_i

  event(:skip_vote){ incr :skip_votes }
  event(:action_skip){ incr :skip_votes }
  event(:yes_vote){ incr :yes_votes }
  event(:action_yes){ incr :yes_votes }
  event(:maybe_vote){ incr :maybe_votes }
  event(:action_maybe){ incr :maybe_votes }

  widget 'UserActivity', {
    :title => "Yes/No/Skip-Votes",
    :type => :timeline,
    :include_current => false,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'UserActivity', {
    :title => "Yes/No/Skip Numbers",
    :type => :numbers,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }


  gauge :messages_sent, :tick => 1.day.to_i
  gauge :messages_read, :tick => 1.day.to_i
  gauge :winks_sent, :tick => 1.day.to_i

  event(:action_wink){ incr :winks_sent }
  event(:wink_sent){ incr :winks_sent }
  event(:message_sent){ incr :messages_sent }
  event(:message_read){ incr :messages_read }

  widget 'UserActivity', {
    :title => "Messages sent/read",
    :type => :timeline,
    :include_current => false,
    :gauges => [:messages_sent, :messages_read]
  }

  widget 'UserActivity', {
    :title => "Winks sent",
    :type => :timeline,
    :include_current => false,
    :gauges => [:winks_sent]
  }

  widget 'UserActivity', {
    :title => "Messages sent+read/Winks Numbers",
    :type => :numbers,
    :gauges => [:messages_sent, :messages_read, :winks_sent]
  }

  gauge :events_per_minute, :tick => 60
  gauge :uniques_daily, :tick => 1.day.to_i, :unique => true

  event :"*" do
    incr :events_per_minute
    incr :uniques_daily
  end

  widget 'TrafficChannels', {
    :title => "Daily Uniques",
    :type => :timeline,
    :width => 70,
    :gauges => :uniques_daily
  }

  widget 'TrafficChannels', {
    :title => "Events per Minute",
    :type => :timeline,
    :width => 30,
    :gauges => :events_per_minute
  }

  #widget 'TrafficChannels', {
  #  :title => "Daily uniques / EPM",
  #  :type => :numbers,
  #  :gauges => [:uniques_daily, :events_per_minute]
  #}

  gauge :mails_sent, :tick => 1.day.to_i
  gauge :mails_clicked, :tick => 1.day.to_i

  event(:mail_sent){ incr :mails_sent }
  event(:mail_clicked){ incr :mails_clicked }

  widget 'TrafficChannels', {
    :title => "Mails sent/read",
    :type => :timeline,
    :gauges => [:mails_sent, :mails_clicked]
  }

  widget 'TrafficChannels', {
    :title => "Mail sent/read Numbers",
    :type => :numbers,
    :gauges => [:mails_sent, :mails_clicked]
  }

  gauge :app_requests_sent, :tick => 1.day.to_i
  gauge :app_requests_clicked, :tick => 1.day.to_i

  event(:app_request_sent){ incr :app_requests_sent }
  event(:app_request_click){ incr :app_requests_clicked }

  widget 'TrafficChannels', {
    :title => "App-Requests sent/clicked",
    :type => :timeline,
    :gauges => [:app_requests_sent, :app_requests_clicked]
  }

  widget 'TrafficChannels', {
    :title => "App-Requests sent/clicked Numbers",
    :type => :numbers,
    :gauges => [:app_requests_sent, :app_requests_clicked]
  }

  gauge :wall_posts_sent, :tick => 1.day.to_i
  gauge :wall_posts_clicked, :tick => 1.day.to_i

  event(:wallpost_sent){ incr :wall_posts_sent }
  event(:wallpost_click){ incr :wall_posts_clicked }

  widget 'TrafficChannels', {
    :title => "Wall-Posts sent/clicked",
    :type => :timeline,
    :gauges => [:wall_posts_sent, :wall_posts_clicked]
  }

  widget 'TrafficChannels', {
    :title => "Wall-Posts sent/clicked Numbers",
    :type => :numbers,
    :gauges => [:wall_posts_sent, :wall_posts_clicked]
  }

end

fm_opts = {}
fm_opts.merge!(:web_interface => ["0.0.0.0", "2323"]) if ENV['DEV']

FnordMetric.run(fm_opts)
