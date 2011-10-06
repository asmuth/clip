window.js_loader = {
  d:document,
  id:function(id){return js_loader.d.getElementById(id)},
  tag:function(element){return js_loader.d.getElementsByTagName(element)},
  ce:function(element){return js_loader.d.createElement(element)},
  js:function(url, callback){
    s = js_loader.ce('script');
    s.type = "text/javascript";
    s.onload = callback;
    s.src = url;
    js_loader.tag('head')[0].appendChild(s);  
  }
};

window.css_loader = {
  d:document,
  id:function(id){return css_loader.d.getElementById(id)},
  tag:function(element){return css_loader.d.getElementsByTagName(element)},
  ce:function(element){return css_loader.d.createElement(element)},        
  css:function(url, callback){
   s = css_loader.ce('link');
   s.type = "text/css";
   s.rel = 'stylesheet';
   s.href = url;
   s.onload = callback;         
   css_loader.tag('head')[0].appendChild(s); 
  }
};

window.widget_loader = function(conf){           
  widget_config = conf;        
  js_loader.js(widget_config.js_file, function(){});        
};