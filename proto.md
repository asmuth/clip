


render a gauge (request):

  { "_class":"render_request", "_channel":"$GAUGE_NAME" }


render a response (request):

  { "_class":"render_response", "_channel":"$GAUGE_NAME", "payload": "$PAYLOAD" }



  {"_class"=>"request", "_channel"=>"number_of_searches", "cmd"=>"get", "tick"=>86400, "ticks"=>[1336435200, ...], "widget"=>"total_timeline" }
