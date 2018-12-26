plot {
  data: csv('tests/testdata/city_temperatures.csv');
  x: var(month);
  y: var(temperature);
  group: var(city);

  axis-y: inverted;
  axis-y-min: -10;
  axis-y-max: 32;
  axis-top: off;
  axis-right: off;

  layer {
    type: lines;
  }

  layer {
    type: points;
    point-size: 3.3pt;
  }
}
