FnordMetric = {
  d: document,
  id: function(id){return FnordMetric.d.getElementById(id)},
  tag: function(element){return FnordMetric.d.getElementsByTagName(element)},
  ce: function(element){return FnordMetric.d.createElement(element)},

  js: function(url, callback){
    s = FnordMetric.ce('script');
    s.type = "text/javascript";
    s.onload = callback;
    s.src = url;
    FnordMetric.tag('head')[0].appendChild(s);  
  },

  css: function(url, callback){
   s = FnordMetric.ce('link');
   s.type = "text/css";
   s.rel = 'stylesheet';
   s.href = url;
   s.onload = callback;         
   FnordMetric.tag('head')[0].appendChild(s); 
  },

  render: function(_widget_config){
  	widget_config = _widget_config;        
  	console.log(widget_config);
    FnordMetric.js(widget_config.widget_url, function(){}); 
  }

};