width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/city_temperatures.csv');
  x: var(month);
  y: var(temperature);
  group: var(city);

  axis-y-min: -30;
  axis-y-max: 50;

  layer {
    type: lines;
  }
}
