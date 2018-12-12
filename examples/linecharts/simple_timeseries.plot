width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/measurement.csv');
  x: $0;
  y: $2;

  axis-x-format: datetime("%H:%M:%S");

  layer {
    type: lines;
    line-color: $0;
  }
}
