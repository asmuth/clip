width: 1200px;
height: 480px;

plot {
  axis-x-format: datetime("%H:%M:%S");

  layer {
    type: lines;
    x: csv('tests/testdata/measurement.csv', time);
    y: csv('tests/testdata/measurement.csv', value2);
  }
}
