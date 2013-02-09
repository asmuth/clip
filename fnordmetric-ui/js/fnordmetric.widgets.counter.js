if (typeof FnordMetric == 'undefined')
  FnordMetric = {};

if (typeof FnordMetric.widgets == 'undefined')
  FnordMetric.widgets = {};


FnordMetric.widgets.counter = function(elem){

  function destroy() {
    console.log("destroy called");
  }

  return {
    destroy: destroy
  };

};
