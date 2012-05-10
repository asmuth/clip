

GAUGE OVERLAY PROTOCOL

    gauge discovery:

      { "_class":"discover_request"}
      { "_class":"discover_response", "gauge_key":"gauge123" }


    render a gauge (request):

      { "_class":"render_request", "_channel":"$GAUGE_NAME" }
      { "_class":"render_response", "_channel":"$GAUGE_NAME", "payload": "$PAYLOAD" }




LOGGER

    options
       :file, :channels




ACCEPTOR

    options
       :protocol, :host, :port




TIMELINEWIDGET

    options

        :title, :tab, :ticks, :series, :series_titles, :include_current, :height


    handlers

      values_at(series, ticks, tick)


    api json:

      request: {"_class"=>"widget_request", "_channel"=>"XXX", "cmd"=>"values_at", "tick"=>86400, "ticks"=>[1336435200, ...], "widget"=>"ZZZ" }

      response: {"_class"=>"widget_response", "_channel"=>"XXX", "cmd"=>"values_at", "tick"=>86400, "values"=> { "series1": { 123: 0, 124: 0 } }, "widget"=>"ZZZ" }

     
