FnordMetric.widgets.htmlWidget = function(){
  function render(opts){

    opts.elem.append(
      $('<div class="headbar small"></div>').html(opts.title)
    ).css({
      'marginBottom': 20,
      'overflow': 'hidden'
    });

    var container = $('<div></div>')
      .addClass('html_container')
      .html(opts.html);

    opts.elem.append(container);
  }

  return {
    render: render
  };
}
