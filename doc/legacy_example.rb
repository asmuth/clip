$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.namespace :ulm do

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
    :title => "ULM Key Metrics",
    :type => :numbers,
    :width => 100,
    :autoupdate => 30,
    :gauges => [
      :pageviews_daily_unique, :pageviews_monthly_unique, :app_requests_sent, :app_requests_clicked, :mails_sent, :mails_clicked,
      :app_invites_sent, :app_invites_clicked
    ]
  }

  widget 'Overview', {
    :title => "User-Activity",
    :type => :timeline,
    :width => 67,
    :autoupdate => 30,
    :gauges => [:skip_votes, :yes_votes, :maybe_votes]
  }

  widget 'Overview', {
    :title => "Top Pages",
    :type => :toplist,
    :autoupdate => 30,
    :width => 33,
    :gauges => [ :pageviews_per_url_daily, :pageviews_per_url_monthly ]
  }


  widget 'Overview', {
    :title => "ULM User Metrics",
    :type => :numbers,
    :width => 67,
    :autoupdate => 30,
    :gauges => [
      :skip_votes, :yes_votes,
      :maybe_votes, :winks_sent, :messages_sent, :messages_read,
    ]
  }

  widget 'Overview', {
    :title => "RockYou Campaign (12/11) Metrics",
    :type => :numbers,
    :width => 33,
    :autoupdate => 30,
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



  gauge :abtest_sidebar_btn_totals,
    :tick => 36000.days.to_i,
    :title => "(A/B) sidebar_btn: Totals",
    :three_dimensional => true

  gauge :abtest_sidebar_btn_daily,
    :tick => 1.day.to_i,
    :title => "(A/B) sidebar_btn: Daily",
    :three_dimensional => true

  gauge :abtest_sidebar_btn_leute_treffen,
    :tick => 1.day.to_i,
    :title => "leute_treffen"

  gauge :abtest_sidebar_btn_jetzt_losflirten,
    :tick => 1.day.to_i,
    :title => "jetzt_losflirten"

  gauge :abtest_sidebar_btn_dates_finden,
    :tick => 1.day.to_i,
    :title => "dates_finden"

  gauge :abtest_sidebar_btn_leute_treffen_monthly,
    :tick => 1.month.to_i,
    :title => "leute_treffen"

  gauge :abtest_sidebar_btn_jetzt_losflirten_monthly,
    :tick => 1.month.to_i,
    :title => "jetzt_losflirten"

  gauge :abtest_sidebar_btn_dates_finden_monthly,
    :tick => 1.month.to_i,
    :title => "dates_finden"


  event :abtest_sidebar_btn_click do
    incr :abtest_sidebar_btn_leute_treffen if data[:variant] == "leute_treffen"
    incr :abtest_sidebar_btn_jetzt_losflirten if data[:variant] == "jetzt_losflirten"
    incr :abtest_sidebar_btn_dates_finden if data[:variant] == "dates_finden"
    incr :abtest_sidebar_btn_leute_treffen_monthly if data[:variant] == "leute_treffen"
    incr :abtest_sidebar_btn_jetzt_losflirten_monthly if data[:variant] == "jetzt_losflirten"
    incr :abtest_sidebar_btn_dates_finden_monthly if data[:variant] == "dates_finden"
    incr_field :abtest_sidebar_btn_totals, data[:variant]
    incr_field :abtest_sidebar_btn_daily, data[:variant]
  end


  widget 'ABTests', {
    :title => "Sidebar-Button (A/B): Daily Clicks per Variant",
    :type => :timeline,
    :autoupdate => 5,
    :width => 67,
    :gauges => [
      :abtest_sidebar_btn_leute_treffen,
      :abtest_sidebar_btn_jetzt_losflirten,
      :abtest_sidebar_btn_dates_finden
    ]
  }

  widget 'ABTests', {
    :title => "Top Pages",
    :type => :toplist,
    :autoupdate => 5,
    :width => 33,
    :gauges => [ :abtest_sidebar_btn_totals, :abtest_sidebar_btn_daily ]
  }


  widget 'ABTests', {
    :title => "Sidebar-Button (A/B): Daily Clicks per Variant (Numbers)",
    :type => :numbers,
    :autoupdate => 5,
    :width => 67,
    :gauges => [
      :abtest_sidebar_btn_leute_treffen,
      :abtest_sidebar_btn_jetzt_losflirten,
      :abtest_sidebar_btn_dates_finden
    ]
  }

  widget 'ABTests', {
    :title => "Sidebar-Button (A/B): Monthly Clicks per Variant",
    :type => :pie,
    :autoupdate => 5,
    :width => 33,
    :gauges => [
      :abtest_sidebar_btn_leute_treffen_monthly,
      :abtest_sidebar_btn_jetzt_losflirten_monthly,
      :abtest_sidebar_btn_dates_finden_monthly
    ]
  }


  gauge :age_distribution_female_monthly,
    :tick => 1.month.to_i,
    :three_dimensional => true,
    :unique => true,
    :title => "Age Distribution (female) monthly"

  gauge :age_distribution_male_monthly,
    :tick => 1.month.to_i,
    :three_dimensional => true,
    :unique => true,
    :title => "Age Distribution (male) monthly"

  gauge :age_distribution_female_daily,
    :tick => 1.day.to_i,
    :three_dimensional => true,
    :unique => true,
    :title => "Age Distribution (female) daily"

  gauge :age_distribution_male_daily,
    :tick => 1.day.to_i,
    :three_dimensional => true,
    :unique => true,
    :title => "Age Distribution (male) daily"


  widget 'Demography', {
    :title => "Age Distribution: Female Users (Monthly)",
    :type => :bars,
    :width => 50,
    :autoupdate => 5,
    :order_by => :field,
    :gauges => [ :age_distribution_female_monthly ]
  }

  widget 'Demography', {
    :title => "Age Distribution: Male Users (Monthly)",
    :type => :bars,
    :width => 50,
    :autoupdate => 5,
    :order_by => :field,
    :gauges => [ :age_distribution_male_monthly ]
  }


  widget 'Demography', {
    :title => "Age Distribution: Female Users",
    :type => :toplist,
    :width => 50,
    :autoupdate => 5,
    :gauges => [ :age_distribution_female_monthly, :age_distribution_female_daily ]
  }

  widget 'Demography', {
    :title => "Age Distribution: Male Users",
    :type => :toplist,
    :width => 50,
    :autoupdate => 5,
    :gauges => [ :age_distribution_male_monthly, :age_distribution_male_daily ]
  }

  event "user_demography" do
    if data[:gender] == "female"
      incr_field(:age_distribution_female_monthly, data[:age], 1)
      incr_field(:age_distribution_female_daily, data[:age], 1)
    end
    if data[:gender] == "male"
      incr_field(:age_distribution_male_monthly, data[:age], 1)
      incr_field(:age_distribution_male_daily, data[:age], 1)
    end
  end


  gauge :competitors_badoo_mau,
    :tick => 1.day.to_i,
    :title => "Badoo (MAU)"

  gauge :competitors_badoo_dau,
    :tick => 1.day.to_i,
    :title => "Badoo (DAU)"

  gauge :competitors_zoosk_mau,
    :tick => 1.day.to_i,
    :title => "zoosk (MAU)"

  gauge :competitors_zoosk_dau,
    :tick => 1.day.to_i,
    :title => "zoosk (DAU)"

  gauge :competitors_areyouinterested_mau,
    :tick => 1.day.to_i,
    :title => "areyouinterested (MAU)"

  gauge :competitors_areyouinterested_dau,
    :tick => 1.day.to_i,
    :title => "areyouinterested (DAU)"

  gauge :competitors_onetwolike_mau,
    :tick => 1.day.to_i,
    :title => "onetwolike (MAU)"

  gauge :competitors_onetwolike_dau,
    :tick => 1.day.to_i,
    :title => "onetwolike (DAU)"

  gauge :competitors_kizzle_mau,
    :tick => 1.day.to_i,
    :title => "kizzle (MAU)"

  gauge :competitors_kizzle_dau,
    :tick => 1.day.to_i,
    :title => "kizzle (DAU)"

  event :competition_data do
    set_value(:competitors_badoo_mau, data[:badoo_mau]) if data[:badoo_mau]
    set_value(:competitors_badoo_dau, data[:badoo_dau]) if data[:badoo_dau]
    set_value(:competitors_areyouinterested_mau, data[:areyouinterested_mau]) if data[:areyouinterested_mau]
    set_value(:competitors_areyouinterested_dau, data[:areyouinterested_dau]) if data[:areyouinterested_dau]
    set_value(:competitors_zoosk_mau, data[:zoosk_mau]) if data[:zoosk_mau]
    set_value(:competitors_zoosk_dau, data[:zoosk_dau]) if data[:zoosk_dau]
    set_value(:competitors_onetwolike_mau, data[:onetwolike_mau]) if data[:onetwolike_mau]
    set_value(:competitors_onetwolike_dau, data[:onetwolike_dau]) if data[:onetwolike_dau]
    set_value(:competitors_kizzle_mau, data[:kizzle_mau]) if data[:kizzle_mau]
    set_value(:competitors_kizzle_dau, data[:kizzle_dau]) if data[:kizzle_dau]
  end

  widget "Competition", {
    :title => "Competition: Badoo",
    :type => :timeline,
    :ticks => 90,
    :gauges => [:competitors_badoo_mau, :competitors_badoo_dau]
  }

  widget "Competition", {
    :title => "Competition: Badoo (Numbers)",
    :type => :numbers,
    :offsets => [10, 30, 90],
    :gauges => [:competitors_badoo_mau, :competitors_badoo_dau]
  }

  widget "Competition", {
    :title => "Competition: zoosk",
    :type => :timeline,
    :ticks => 90,
    :gauges => [:competitors_zoosk_mau, :competitors_zoosk_dau]
  }

  widget "Competition", {
    :title => "Competition: zoosk (Numbers)",
    :type => :numbers,
    :offsets => [10, 30, 90],
    :gauges => [:competitors_zoosk_mau, :competitors_zoosk_dau]
  }

  widget "Competition", {
    :title => "Competition: onetwolike",
    :type => :timeline,
    :ticks => 90,
    :gauges => [:competitors_onetwolike_mau, :competitors_onetwolike_dau]
  }

  widget "Competition", {
    :title => "Competition: onetwolike (Numbers)",
    :type => :numbers,
    :offsets => [10, 30, 90],
    :gauges => [:competitors_onetwolike_mau, :competitors_onetwolike_dau]
  }

  widget "Competition", {
    :title => "Competition: kizzle",
    :type => :timeline,
    :ticks => 90,
    :gauges => [:competitors_kizzle_mau, :competitors_kizzle_dau]
  }

  widget "Competition", {
    :title => "Competition: kizzle (Numbers)",
    :type => :numbers,
    :offsets => [10, 30, 90],
    :gauges => [:competitors_kizzle_mau, :competitors_kizzle_dau]
  }

  widget "Competition", {
    :title => "Competition: areyouinterested",
    :type => :timeline,
    :ticks => 90,
    :gauges => [:competitors_areyouinterested_mau, :competitors_areyouinterested_dau]
  }

  widget "Competition", {
    :title => "Competition: areyouinterested (Numbers)",
    :type => :numbers,
    :offsets => [10, 30, 90],
    :gauges => [:competitors_areyouinterested_mau, :competitors_areyouinterested_dau]
  }



  gauge :events_per_minute, :tick => 60
  gauge :events_per_hour, :tick => 1.hour.to_i
  gauge :events_per_second, :tick => 1
  gauge :votes_per_second, :tick => 1

  event :"*" do
    incr :events_per_minute
    incr :events_per_hour
    incr :events_per_second
  end


  event(:skip_vote){ incr :votes_per_second }
  event(:action_skip){ incr :votes_per_second }
  event(:yes_vote){ incr :votes_per_second }
  event(:action_yes){ incr :votes_per_second }
  event(:maybe_vote){ incr :votes_per_second }
  event(:action_maybe){ incr :votes_per_second }


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
    :title => "Votes/Second",
    :type => :timeline,
    :width => 50,
    :gauges => :votes_per_second,
    :include_current => true,
    :plot_style => :areaspline,
    :autoupdate => 1
  }

end

FnordMetric.server_configuration = {
  :redis_url => "redis://localhost:6379",
  :redis_prefix => "fnordmetric",
  :inbound_stream => ["0.0.0.0", "1337"],
  :web_interface => ["0.0.0.0", "4242"],
  :start_worker => true,
  :print_stats => 3,

  # events that aren't processed after 2 min get dropped
  :event_queue_ttl => 120,

  # event data is kept for one month
  :event_data_ttl => 3600*24*30,

  # session data is kept for one month
  :session_data_ttl => 3600*24*30
}

#task :setup do
#  @fm_opts = {:web_interface => ["0.0.0.0", "2323"]} if ENV['DEV']
#end

FnordMetric.standalone
