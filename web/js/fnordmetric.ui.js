FnordMetric.ui.navbar = function(elem, opts){

  elem.css('width', $('#viewport').width());

  var back = $('<a>')
    .addClass('button back')
    .html('&laquo;');

  elem.prepend(back);

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

  for(ind in opts.buttons){
    var btn = $('<a>')
      .addClass('button navbutton')
      .click(opts.buttons[ind][1])
      .html(opts.buttons[ind][0]);
    elem.prepend(btn);
  }

  if(opts.datepicker){
    elem.append(
      $('<a class="button datepicker">')
        .html('<div class="date">&nbsp;</div><i class="icon-calendar"></i>'));  
  }

  elem.append(head);
}

FnordMetric.ui.modal = function(opts){
  FnordMetric.ui.close_modal('body');
  width = $("#viewport").width() * 0.8;

  if (opts.max_width)
    width = Math.min(opts.max_width, width);

  var dialog_elem = $('<div class="modal">')
    .append($('<div class="modal_inner">'))
    .css({
      "width": width,
      "height": opts.height,
      "marginLeft": (($("#viewport").width() - width) / 2),
      "display": "none"
    });


  if (opts.max_width)
    dialog_elem.css('maxWidth', opts.max_width);

  if (opts.content)
    dialog_elem.html(opts.content);

  $("#viewport").append(dialog_elem);
  dialog_elem.show().addClass('visible');
  $('.modal_backdrop').show().addClass('visible');
}

FnordMetric.ui.close_modal = function(elem){
  var m = $('.modal', $(elem));
  m.push($(elem).closest('.modal'));

  m.removeClass('visible');

  window.setTimeout(function(){
    m.remove();
  }, 500);

  $('.modal_backdrop').removeClass('visible').fadeOut();
}

FnordMetric.ui.resizable = function(elem){
  $('.resizable', $(elem)).each(function(){
    var wwperc = parseInt($(this).attr('data-width'));
    if (!wwperc || (wwperc < 1)){ wwperc = 100; }
    var wwidth = $('.viewport_inner').width() * (wwperc/100.0);
    if(wwperc==100){ $(this).addClass('full_width'); } 
    else if($(this).hasClass('right')) { $(this).css('float', 'right'); }
    else { wwidth -= 1; $(this).css('float', 'left'); }
    $(this).css('overflow', 'hidden');
    $(this).width(wwidth);
  });
}
