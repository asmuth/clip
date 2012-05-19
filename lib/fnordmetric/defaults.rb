FnordMetric::COLORS = ["#406ea5", "#aa4643", "#83a145"].reverse

FnordMetric::DEFAULT_PROC = lambda{ |arg|

  unless flags[:hide_active_users]

    eventfeed_gauge :active_users,
      :title => "Active Users",
      :group => "Overview"

  end

}

