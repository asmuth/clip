$: << ::File.expand_path("../../../fnordmetric/lib/", __FILE__)
require "fnordmetric"

FnordMetric.namespace :myapp do

  # hide_overview
  # hide_active_users


  # NEW DSL (v1.0 upwards)

  timeseries_gauge :number_of_signups,
    :group => "My Group",
    :title => "Number of Signups",
    :key_nouns => ["Singup", "Signups"],
    :series => [:via_twitter, :via_facebook],
    :resolution => 2.minutes


  distribution_gauge :user_age_distribution,
    :title => "User Age Distribution",
    :value_ranges => [(10..16), (16..20), (20..24), (24..28), (28..32), (32..36), (40..44), (44..48),
                      (48..52), (52..56), (60..64), (64..68), (68..72), (72..76), (70..74), (74..78)],
    :value_scale  => 1,
    :resolution => 2.minutes


  toplist_gauge :popular_keywords,
    :title => "Popular Keywords",
    :resolution => 2.minutes


  event :search do
    observe :popular_keywords, data[:keyword]
  end

  event :signup do
    if data[:referrer] == "facebook"
      incr :number_of_signups, :via_facebook, 1
    elsif data[:referrer] == "twitter"
      incr :number_of_signups, :via_twitter, 1
    end
  end

  # OLD DSL (will be supported forever, allows finer-grained control)

  gauge :events_per_hour, :tick => 1.hour
  gauge :events_per_second, :tick => 1.second
  gauge :events_per_minute, :tick => 1.minute

  event :"*" do
    incr :events_per_hour
    incr :events_per_minute
    incr :events_per_second
  end


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


  gauge :age_distribution_female_monthly,
    :tick => 1.month.to_i,
    :three_dimensional => true,
    :title => "Age Distribution (female) monthly"

  gauge :age_distribution_male_monthly,
    :tick => 1.month.to_i,
    :three_dimensional => true,
    :title => "Age Distribution (male) monthly"

  gauge :age_distribution_female_daily,
    :tick => 1.day.to_i,
    :three_dimensional => true,
    :title => "Age Distribution (female) daily"

  gauge :age_distribution_male_daily,
    :tick => 1.day.to_i,
    :three_dimensional => true,
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
    observe :user_age_distribution, data[:age]
  end


end

FnordMetric.options = {
  :event_queue_ttl  => 10, # all data that isn't processed within 10s is discarded to prevent memory overruns
  :event_data_ttl   => 3600, # event data is stored for one hour (needed for the active users view)
  :session_data_ttl => 3600, # session data is stored for one hour (needed for the active users view)
  :redis_prefix => "fnordmetric"
}

def start_example_data_generator

  api = FnordMetric::API.new
  Thread.new do
    loop do
      api.event(:_type => :signup, :referrer => (rand(3) == 1 ? :twitter : :facebook))
      api.event(:_type => :search, :keyword => (%w(Donau Dampf Schiff Fahrts Kaptitaens Muetzen Staender).shuffle[0..2] * ""))
      api.event(:_type => :user_demography, :age => rand(15..55), :gender => (rand(2)==1 ? :female : :male) )
      sleep (rand(10)/10.to_f)
    end
  end

end

start_example_data_generator

FnordMetric::Web.new(:port => 4242)
FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)
FnordMetric::Worker.new
FnordMetric.run



