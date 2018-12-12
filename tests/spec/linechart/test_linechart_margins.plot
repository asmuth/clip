width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/measurement.csv');
  x: $0;
  y: $2;

  margin-left: 100px;
  margin-right: 100px;
  margin-top: 30px;
  margin-bottom: 30px;

  axis-x-format: datetime("%H:%M:%S");

  layer {
    type: lines;
    line-color: $0;
  }
}
