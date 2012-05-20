FnordMetric::COLORS = ["#4572a7", "#aa4643", "#89a54e", "#80699b", "#3d96ae", "#db843d"].reverse
FnordMetric::TICKS =  [60, 300, 1200, 3600, 86400]

FnordMetric::DEFAULT_PROC = lambda{ |arg|

  unless flags[:hide_active_users]

    eventfeed_gauge :active_users,
      :title => "Active Users",
      :group => "Overview"

  end

}

