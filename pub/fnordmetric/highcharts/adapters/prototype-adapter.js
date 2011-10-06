/*
 Highcharts JS v2.1.6 (2011-07-08)
 Prototype adapter

 @author Michael Nelson, Torstein H?nsi.

 Feel free to use and modify this script.
 Highcharts license: www.highcharts.com/license.
*/
var HighchartsAdapter=function(){var f=typeof Effect!="undefined";return{init:function(){if(f)Effect.HighchartsTransition=Class.create(Effect.Base,{initialize:function(b,a,c,d){var e;this.element=b;e=b.attr(a);if(a=="d"){this.paths=Highcharts.pathAnim.init(b,b.d,c);this.toD=c;e=0;c=1}this.start(Object.extend(d||{},{from:e,to:c,attribute:a}))},setup:function(){HighchartsAdapter._extend(this.element);this.element._highchart_animation=this},update:function(b){var a=this.paths;if(a)b=Highcharts.pathAnim.step(a[0],
a[1],b,this.toD);this.element.attr(this.options.attribute,b)},finish:function(){this.element._highchart_animation=null}})},addEvent:function(b,a,c){if(b.addEventListener||b.attachEvent)Event.observe($(b),a,c);else{HighchartsAdapter._extend(b);b._highcharts_observe(a,c)}},animate:function(b,a,c){var d;c=c||{};c.delay=0;c.duration=(c.duration||500)/1E3;if(f)for(d in a)new Effect.HighchartsTransition($(b),d,a[d],c);else for(d in a)b.attr(d,a[d]);if(!b.attr)throw"Todo: implement animate DOM objects";
},stop:function(b){b._highcharts_extended&&b._highchart_animation&&b._highchart_animation.cancel()},each:function(b,a){$A(b).each(a)},fireEvent:function(b,a,c,d){if(a.preventDefault)d=null;if(b.fire)b.fire(a,c);else b._highcharts_extended&&b._highcharts_fire(a,c);d&&d(c)},removeEvent:function(b,a,c){if($(b).stopObserving)$(b).stopObserving(a,c);else{HighchartsAdapter._extend(b);b._highcharts_stop_observing(a,c)}},grep:function(b,a){return b.findAll(a)},map:function(b,a){return b.map(a)},merge:function(){function b(a,
c){var d,e;for(e in c){d=c[e];a[e]=d&&typeof d=="object"&&d.constructor!=Array&&typeof d.nodeType!=="number"?b(a[e]||{},d):c[e]}return a}return function(){for(var a=arguments,c={},d=0;d<a.length;d++)c=b(c,a[d]);return c}.apply(this,arguments)},_extend:function(b){b._highcharts_extended||Object.extend(b,{_highchart_events:{},_highchart_animation:null,_highcharts_extended:true,_highcharts_observe:function(a,c){this._highchart_events[a]=[this._highchart_events[a],c].compact().flatten()},_highcharts_stop_observing:function(a,
c){if(a)if(c)this._highchart_events[a]=[this._highchart_events[a]].compact().flatten().without(c);else delete this._highchart_events[a];else this._highchart_events={}},_highcharts_fire:function(a,c){(this._highchart_events[a]||[]).each(function(d){c&&c.stopped||d.bind(this)(c)}.bind(this))}})}}}();
