Hacking
-------

This is a random collection of short tips.


### Config in multiple files

You can easily split up your config into multiple files like this:

*fnordmetric_main.rb*

    FnordMetric.namespace :myapp do
      instance_eval(&MY_FNORDMETRIC_MODULE1)
      instance_eval(&MY_FNORDMETRIC_MODULE2)
    end

*fnordmetric_module1.rb*

    MY_FNORDMETRIC_MODULE1 = proc {

      # DSL statements go here

    }

<br />


### Redis Memory usage

If you want to reduce redis memory you can do it by using larger
flush_timeouts. Apart from that there are two few low hanging
fruits:

**Disable active users plugin**

    FnordMetric.options = {
      :enable_active_users => false
    }


**Drastically reduce queue TTLs**

    FnordMetric.options = {
      :event_queue_ttl => 60,
      :event_data_ttl => 60,
      :session_data_ttl => 1
    }

<br />


### Writing custom widgets / views

There is one straightforward hack that you can use to build
your own widgets without having to understand the full FnordMetric
source:

Save this to a file blubb_widget.rb

    class FnordMetric::BlubbWidget < FnordMetric::Widget

      def render
        data.merge(:klass => "HtmlWidget", :html => my_html)
      end

      def my_html
        "<h1>Yay, I can render custom HTML in FnordMetric. The current time is: #{Time.now.to_s}</h1>"
      end

    end

and then use it like this:

    require "blubb_widget.rb"

    widget "My Dashboard",
      :title => "My custom widget",
      :type => :blubb,
      :width => 100

<br />


### Mounting as Rails app

I currently don't know how this can be done, since FnordMetric
uses rack-websockets, but it *should* be possible. If somebody
has done this, please send a Pull Request for this page on
github :)
