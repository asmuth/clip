$: << ::File.expand_path("../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.namespace :ulikeme do

  gauge :skip_votes, :tick => 1.day.to_i, :title => "Skip-Votes"
  gauge :yes_votes, :tick => 1.day.to_i, :title => "Yes-Votes"
  gauge :maybe_votes, :tick => 1.day.to_i, :title => "Maybe-Votes"

  gauge :pageviews_daily_unique, :tick => 1.day.to_i, :unique => true, :title => "Unique Visits (Daily)"
  gauge :pageviews_hourly_unique, :tick => 1.hour.to_i, :unique => true, :title => "Unique Visits (Hourly)"
  gauge :pageviews_monthly_unique, :tick => 40.days.to_i, :unique => true, :title => "Unique Visits (Month)"

  gauge :messages_sent, :tick => 1.day.to_i, :title => "Messages (sent)"
  gauge :messages_read, :tick => 1.day.to_i, :title => "Messages (read)"
  gauge :winks_sent, :tick => 1.day.to_i, :title => "Winks sent"

  gauge :pageviews_per_url_daily, 
    :tick => 1.day.to_i, 
    :title => "Daily Pageviews per URL", 
    :three_dimensional => true

  gauge :pageviews_per_url_monthly, 
    :tick => 30.days.to_i, 
    :title => "Monthly Pageviews per URL", 
    :three_dimensional => true

  event :_pageview do
    incr :pageviews_daily_unique
    incr :pageviews_hourly_unique
    incr :pageviews_monthly_unique
    incr_field :pageviews_per_url_daily, data[:url]
    incr_field :pageviews_per_url_monthly, data[:url]
  end

  event(:action_wink){ incr :winks_sent }
  event(:wink_sent){ incr :winks_sent }
  event(:message_sent){ incr :messages_sent }
  event(:message_read){ incr :messages_read }

  event(:skip_vote){ incr :skip_votes }
  event(:action_skip){ incr :skip_votes }
  event(:yes_vote){ incr :yes_votes }
  event(:action_yes){ incr :yes_votes }
  event(:maybe_vote){ incr :maybe_votes }
  event(:action_maybe){ incr :maybe_votes }


  gauge :mails_sent, :tick => 1.day.to_i, :title => "Mails (sent)"
  gauge :mails_clicked, :tick => 1.day.to_i, :title => "Mails (clicked)"

  event(:mail_sent){ incr :mails_sent }
  event(:mail_clicked){ incr :mails_clicked }


  gauge :app_requests_sent, :tick => 1.day.to_i, :title => "App-Requests (sent)"
  gauge :app_requests_clicked, :tick => 1.day.to_i, :title => "App-Requests (clicked)"

  gauge :app_invites_sent, :tick => 1.day.to_i, :title => "App-Invites (sent)"
  gauge :app_invites_clicked, :tick => 1.day.to_i, :title => "App-Invites (clicked)"

  event(:app_request_sent){ incr :app_requests_sent }
  event(:app_request_click){ incr :app_requests_clicked }

  event(:app_invite_sent){ incr :app_invites_sent }
  event(:app_invite_click){ incr :app_invites_clicked }

  gauge :rockyou1_ppis, :tick => 1.day.to_i, :title => "RockYou (12/11): Paid Installs"
  gauge :rockyou1_requests, :tick => 1.day.to_i, :title => "RockYou (12/11): Invites sent"
  gauge :rockyou1_refs, :tick => 1.day.to_i, :title => "RockYou (12/11): Ref. Installs"




  widget 'Overview', {
    :title => "Unique Visits per Day",
    :type => :timeline,
    :width => 67,
    :gauges => :pageviews_daily_unique,
    :include_current => true,
    :autoupdate => 30
  }

  widget 'Overview', {
    :title => "Unique Visits per Hour",
    :type => :timeline,
    :width => 33,
    :gauges => :pageviews_hourly_unique,
    :include_current => true,
    :autoupdate => 30
  }

  widget 'Overview', {
    :title => "uLikeMe Key Metrics",
    :type => :numbers,
    :width => 67,
    :autoupdate => 20,
    :gauges => [
      :pageviews_daily_unique, :pageviews_monthly_unique, :skip_votes, :yes_votes, 
      :maybe_votes, :messages_sent, :messages_read, :winks_sent,
      :mails_sent, :mails_clicked, :app_requests_sent, :app_requests_clicked,
      :app_invites_sent, :app_invites_clicked
    ]
  }

  widget 'Overview', {
    :title => "Top Pages",
    :type => :toplist,
    :autoupdate => 20,
    :width => 33,
    :gauges => [ :pageviews_per_url_daily, :pageviews_per_url_monthly ]
  }


  widget 'Overview', {
    :title => "User-Activity",
    :type => :timeline,
    :width => 67,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'Overview', {
    :title => "RockYou Campaign (12/11) Metrics",
    :type => :numbers,
    :width => 33,
    :autoupdate => 20,
    :gauges => [ :rockyou1_ppis, :rockyou1_refs, :rockyou1_requests ]
  }




  # user activity

  widget 'UserActivity', {
    :title => "Yes/No/Skip-Votes",
    :type => :timeline,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'UserActivity', {
    :title => "Yes/No/Skip Numbers",
    :type => :numbers,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }


  widget 'UserActivity', {
    :title => "Messages sent/read",
    :type => :timeline,
    :gauges => [:messages_sent, :messages_read, :winks_sent]
  }

  widget 'UserActivity', {
    :title => "Messages sent+read/Winks Numbers",
    :type => :numbers,
    :gauges => [:messages_sent, :messages_read, :winks_sent]
  }


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

  widget 'TrafficChannels', {
    :title => "App-Invites sent/clicked",
    :type => :timeline,
    :gauges => [:app_invites_sent, :app_invites_clicked]
  }

  widget 'TrafficChannels', {
    :title => "App-Invites sent/clicked Numbers",
    :type => :numbers,
    :gauges => [:app_invites_sent, :app_invites_clicked]
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



  event :campaign_install do
    if %w(ry201112a ry201112b).include?(data[:campaign_key])
      incr :rockyou1_ppis
    end
    if %w(ry201112_ref).include?(data[:campaign_key])
      incr :rockyou1_refs
    end
  end

  event :app_request_sent do
    if %w(ry201112a ry201112b ry201112_ref).include?(data[:campaign_key])
      incr :rockyou1_requests
    end
  end  

  widget 'Campaigns', {
    :title => "RockYou (1) - PPI vs. Requests vs. Refs",
    :gauges => [:rockyou1_requests, :rockyou1_ppis, :rockyou1_refs],
    :type => :timeline
  }

  widget 'Campaigns', {
    :title => "RockYou (1) - Installs via PPI",
    :type => :timeline,
    :gauges => [:rockyou1_ppis]
  }

  widget 'Campaigns', {
    :title => "RockYou (1) - Installs via Referral",
    :gauges => [:rockyou1_refs],
    :type => :timeline
  }

  widget 'Campaigns', {
    :title => "RockYou (1) - AppRequests sent",
    :gauges => [:rockyou1_requests],
    :type => :timeline
  }


  gauge :events_per_minute, :tick => 60
  gauge :events_per_hour, :tick => 1.hour.to_i
  
  event :"*" do
    incr :events_per_minute
    incr :events_per_hour
  end

  widget 'TechStats', {
    :title => "Events per Minute",
    :type => :timeline,
    :width => 50,
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

end

#task :setup do
#  @fm_opts = {:web_interface => ["0.0.0.0", "2323"]} if ENV['DEV']
#end

FnordMetric.standalone
