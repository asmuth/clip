width: 1200px;
height: 600px;

background-color: #000;
foreground-color: #fff;

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
