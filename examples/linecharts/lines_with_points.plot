plot {
  data: csv('tests/testdata/city_temperatures.csv');
  x: $1;
  y: $2;
  group: $0;

  axis-x-format: string;
  axis-y-min: -10;
  axis-y-max: 32;
  axis-top: off;
  axis-right: off;
  margin-right: 2.4em;

  layer {
    type: lines;
    line-color: $0;
  }

  layer {
    type: points;
    point-size: 3.3pt;
    point-color: $0;
  }
}
