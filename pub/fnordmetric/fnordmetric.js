FnordMetric = {
  d: document,
  p: '/fnordmetric/',
  id: function(id){return FnordMetric.d.getElementById(id)},
  tag: function(element){return FnordMetric.d.getElementsByTagName(element)},
  ce: function(element){return FnordMetric.d.createElement(element)},

  js: function(url, callback){
    var s = FnordMetric.ce('script');
    s.type = "text/javascript";
    s.onload = callback;
    s.src = FnordMetric.p+url;
    FnordMetric.tag('head')[0].appendChild(s);  
  },

  css: function(url, callback){
    var s = FnordMetric.ce('link');
    s.type = "text/css";
    s.rel = 'stylesheet';
    s.href = FnordMetric.p+url;
    s.onload = callback;         
    FnordMetric.tag('head')[0].appendChild(s); 
  },

  render: function(elem, widget_config){
    var f = FnordMetric.ce('iframe');    
    f.style.width = '100%'; f.style.height = widget_config.widget_height+'px';
    f.frameBorder = 'none'; f.scrolling = 'no';
    FnordMetric.id(elem).appendChild(f); 
    var s = f.contentDocument.createElement('script')
    s.type = "text/javascript";
    s.src = FnordMetric.p+'fnordmetric.js';    
    s.onload = function(){ f.contentWindow.FnordMetric.js(widget_config.widget_url); }
    f.contentWindow.widget_config = widget_config;        
    f.contentDocument.getElementsByTagName('head')[0].appendChild(s);   	
  }

};