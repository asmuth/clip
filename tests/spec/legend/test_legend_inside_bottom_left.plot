width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/city_temperatures.csv');
  x: $1;
  y: $2;
  group: $0;

  axis-x-format: string;
  axis-y-min: -10;
  axis-y-max: 32;

  legend {
    position: bottom left inside;
  }

  layer {
    type: lines;
    line-color: $0;
  }
}
