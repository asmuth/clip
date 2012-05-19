FnordMetric::DEFAULT_PROC = lambda{ |arg|

  unless flags[:hide_active_users]

    eventfeed_gauge :active_users,
      :title => "Active Users",
      :group => "Overview"

  end

}