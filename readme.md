FnordMetric
===========

FnordMetric is a highly configurable (and pretty fast) realtime app/event 
tracking thing based on [ruby eventmachine](http://rubyeventmachine.com/) and [redis](http://redis.io/). You define your own 
plotting and counting functions as ruby blocks! [See it in action! (RailsCasts)](http://railscasts.com/episodes/378-fnordmetric)

[ ![Build status - Travis-ci](https://secure.travis-ci.org/paulasmuth/fnordmetric.png) ](http://travis-ci.org/paulasmuth/fnordmetric)

[ ![Screenshot](https://github.com/paulasmuth/fnordmetric/raw/v1.0-alpha/doc/preview3.png) ](http://github.com/paulasmuth/fnordmetric)



Getting Started
---------------

Simple Example: this will listen for json-events with `type=unicorn_seen` 
and render a timeline-plot showing the number of received events per hour.

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  # timeline+plot and punchcard
  timeseries_gauge :unicorns_seen_per_hour, 
    :resolution => 5.minutes,
    :title => "Unicorns seen per Hour",
    :punchcard => true,
    :series => [:num_unicorns]

  # on every event like { _type: 'unicorn_seen' }
  event(:unicorn_seen) do
    # increment the unicorns_seen_per_hour gauge by 1
    incr :unicorns_seen_per_hour
  end

end

FnordMetric.standalone
```

This is the easiest way to submit an event:

    echo '{"_type": "unicorn_seen"}' | nc localhost 1337



Installation
------------

    gem install fnordmetric

or in your Gemfile:

    gem 'fnordmetric', '>= 1.0.0'


Documentation
-------------

Check out the docs in [the wiki](http://github.com/paulasmuth/fnordmetric/wiki) or have a look at `doc/full_example.rb`. There are also a few blogs/howtos:

+ [Blog: Monitor your Python App With FnordMetric](http://stephenholiday.com/articles/2012/monitor-your-python-app-with-fnordmetric/)
+ [Blog: FnordMetric and C (Blog/Howto)](http://johnmurray.io/log/2012/01/19/FnordMetrics-and-C%23.md)
+ [Blog: FnordMetric HowTo (russian)](http://www.pvsm.ru/ruby/2723)
+ [RailsCast: FnordMetric (378)](http://railscasts.com/episodes/378-fnordmetric)


### More Resources

+ [PHP API for FnordMetric (github.com/leemachin/fnordmetric-php-api)](https://github.com/leemachin/fnordmetric-php-api)
+ [Python API for FnordMetric (github.com/sholiday/pyfnordmetric)](https://github.com/sholiday/pyfnordmetric)
+ [Another Ruby API for FnordMetric](https://github.com/savonarola/fnordmetric-client)
+ [Beanstalk Monitoring with FnordMetric](https://github.com/sholiday/fnordstalk)
+ [The "FnordMetric" google group](http://groups.google.com/group/fnordmetric)



Full Example
------------

```ruby
require "fnordmetric"

FnordMetric.namespace :myapp do

  hide_overview
  hide_active_users


  # NEW DSL (v1.0 upwards)

  timeseries_gauge :number_of_signups,
    :group => "My Group",
    :title => "Number of Signups",
    :key_nouns => ["Signup", "Signups"],
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
  gauge :pageviews_monthly_unique, :tick => 40.days.to_i, :unique => true, :title => "Unique Visits (Monthly)"

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
    :title => "Events per Second",
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
  :event_data_ttl   => 10,
  :session_data_ttl => 1,  # we don't care about session data for now
  :redis_prefix => "fnordmetric" 
}

def start_example_data_generator

  api = FnordMetric::API.new
  Thread.new do
    loop do
      api.event(:_type => :signup, :referrer => (rand(3) == 1 ? :twitter : :facebook))
      api.event(:_type => :search, :keyword => (%w(Donau Dampf Schiff Fahrts Kaptitaens Muetzen Staender).shuffle[0..2] * ""))
      api.event(:_type => :user_demography, :age => [*15..85].sample, :gender => (rand(2)==1 ? :female : :male) )
      sleep (rand(10)/10.to_f)
    end
  end

end

start_example_data_generator

FnordMetric::Web.new(:port => 4242)
FnordMetric::Acceptor.new(:protocol => :tcp, :port => 2323)
FnordMetric::Worker.new
FnordMetric.run
```




Contributors
------------

http://github.com/paulasmuth/fnordmetric/graphs/contributors

(One patch or more)

+ Simon Menke (http://github.com/fd)
+ Bruno Michel (http://github.com/nono)
+ Marco Borromeo (http://github.com/mborromeo)
+ Leo Lou (http://github.com/l4u)
+ Andy Lindeman (http://github.com/alindeman)
+ Jurriaan Pruis (http://github.com/jurriaan)
+ Kacper Bielecki (http://github.com/kazjote)
+ John Murray (http://github.com/JohnMurray)
+ Lars Gierth (http://github.com/lgierth)
+ Ross Kaffenberger (http://github.com/rossta)
+ Kunal Modi (http://github.com/kunalmodi)
+ Michael Fairchild (http://github.com/fairchild)
+ James Cox (http://github.com/imajes)
+ Pieter Noordhuis (http://github.com/pietern)
+ Tadas Ščerbinskas (http://github.com/tadassce)

To contribute, please fork this repository, make your changes and run the 
specs, commit them to your github repository and send me a pull request.
Need help, head on over to our [Google Groups][1]  page to discuss any ideas
that you might have.


License
-------

Copyright (c) 2011 Paul Asmuth

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to use, copy and modify copies of the Software, subject 
to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  [1]: http://groups.google.com/group/fnordmetric
  [2]: http://www.screenr.com/KiJs
  [3]: https://secure.travis-ci.org/paulasmuth/fnordmetric.png
  [4]: http://travis-ci.org/paulasmuth/fnordmetric
