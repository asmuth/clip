FnordMetric.ui.navbar = function(elem, opts){
  var head = $('<h1></h1>');
  for(ind in opts.breadcrumb){
  	var cur = $('<a>').html(opts.breadcrumb[ind][0]);
  	if (ind == (opts.breadcrumb.length - 1)){ 
	  cur.addClass('current'); 
    } else {
      cur.append(' / ');
    }
    head.append(cur);
  };
  elem.append(head);
}