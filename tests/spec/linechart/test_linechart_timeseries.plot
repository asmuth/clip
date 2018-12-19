width: 1200px;
height: 480px;

plot {
  axis-x-format: datetime("%H:%M:%S");

  layer {
    type: lines;
    x: csv('tests/testdata/measurement.csv', 0);
    y: csv('tests/testdata/measurement.csv', 2);
  }
}
