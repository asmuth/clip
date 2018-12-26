plot {
  x: csv('tests/testdata/city_temperatures_london.csv', month);
  y: csv('tests/testdata/city_temperatures_london.csv', temperature);

  axis-top: off;
  axis-right: off;

  axis-y-min: -10;
  axis-y-max: 32;

  layer {
    type: lines;
  }

  layer {
    type: points;
  }

  layer {
    type: labels;
    label: csv('tests/testdata/city_temperatures_london.csv', temperature_str);
  }
}
