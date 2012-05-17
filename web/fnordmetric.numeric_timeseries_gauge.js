FnordMetric.widgets.timeseriesWidget().render({
  title: "Fnord!!!!",
  elem: $('.mywidget'),
  default_style: 'flow',
  series: [{
    data: [{x:0, y:1},{x:2, y:5}],
    name: "fnord",
    color: "0f0"
  }]
  //series: #{@series_render.to_json}
});  
