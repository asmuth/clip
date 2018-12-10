width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/measurement.csv');
  x: 0;
  y: 2;

  xdomain-format: datetime("%H:%M:%S");

  series {
  }

}
