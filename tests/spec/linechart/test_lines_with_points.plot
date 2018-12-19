plot {
  group: csv('tests/testdata/city_temperatures.csv', 0);
  x: csv('tests/testdata/city_temperatures.csv', 1);
  y: csv('tests/testdata/city_temperatures.csv', 2);

  axis-x-format: string;
  axis-y-min: -10;
  axis-y-max: 32;
  axis-top: off;
  axis-right: off;
  margin-right: 2.4em;

  legend {
    position: top left inside;
  }

  layer {
    type: lines;
    color: csv('tests/testdata/city_temperatures.csv', 0);
  }

  layer {
    type: points;
    color: csv('tests/testdata/city_temperatures.csv', 0);
    point-size: 3.3pt;
  }
}
