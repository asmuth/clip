width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/city_temperatures.csv');
  x: var(month);
  y: var(temperature);
  group: var(city);

  axis-y-min: -10;
  axis-y-max: 32;

  layer {
    type: lines;
  }

  legend {
    position: bottom center inside;
  }
}
