/*
 Highcharts JS v2.1.6 (2011-07-08)
 MooTools adapter

 (c) 2010-2011 Torstein H?nsi

 License: www.highcharts.com/license
*/
(function(){var g=window,j=!!g.$merge,h=g.$extend||function(){return Object.append.apply(Object,arguments)};g.HighchartsAdapter={init:function(){var a=Fx.prototype,b=a.start,c=Fx.Morph.prototype,d=c.compute;a.start=function(f){var e=this.element;if(f.d)this.paths=Highcharts.pathAnim.init(e,e.d,this.toD);b.apply(this,arguments);return this};c.compute=function(f,e,k){var i=this.paths;if(i)this.element.attr("d",Highcharts.pathAnim.step(i[0],i[1],k,this.toD));else return d.apply(this,arguments)}},animate:function(a,
b,c){var d=a.attr,f=c&&c.complete;if(d&&!a.setStyle){a.getStyle=a.attr;a.setStyle=function(){var e=arguments;a.attr.call(a,e[0],e[1][0])};a.$family=a.uid=true}HighchartsAdapter.stop(a);c=new Fx.Morph(d?a:$(a),h({transition:Fx.Transitions.Quad.easeInOut},c));if(b.d)c.toD=b.d;f&&c.addEvent("complete",f);c.start(b);a.fx=c},each:function(a,b){return j?$each(a,b):a.each(b)},map:function(a,b){return a.map(b)},grep:function(a,b){return a.filter(b)},merge:function(){var a=arguments,b=[{}],c=a.length;if(j)a=
$merge.apply(null,a);else{for(;c--;)b[c+1]=a[c];a=Object.merge.apply(Object,b)}return a},addEvent:function(a,b,c){if(typeof b=="string"){if(b=="unload")b="beforeunload";if(!a.addEvent)if(a.nodeName)a=$(a);else h(a,new Events);a.addEvent(b,c)}},removeEvent:function(a,b,c){if(b){if(b=="unload")b="beforeunload";defined(c)?a.removeEvent(b,c):a.removeEvents(b)}else a.removeEvents()},fireEvent:function(a,b,c,d){b=new Event({type:b,target:a});b=h(b,c);b.preventDefault=function(){d=null};a.fireEvent&&a.fireEvent(b.type,
b);d&&d(b)},stop:function(a){a.fx&&a.fx.cancel()}}})();
