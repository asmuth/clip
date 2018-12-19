plot {
  axis-x-format: string;
  axis-y-min: -10;
  axis-y-max: 32;
  axis-top: off;
  axis-right: off;

  legend {
    position: top left inside;
  }

  layer {
    type: lines;
    group: csv('tests/testdata/city_temperatures_london.csv', 0);
    color: csv('tests/testdata/city_temperatures_london.csv', 0);
    x: csv('tests/testdata/city_temperatures_london.csv', 1);
    y: csv('tests/testdata/city_temperatures_london.csv', 2);
  }

  layer {
    type: labels;
    x: csv('tests/testdata/city_temperatures_london.csv', 1);
    y: csv('tests/testdata/city_temperatures_london.csv', 2);
    label: csv('tests/testdata/city_temperatures_london.csv', 3);
  }
}
