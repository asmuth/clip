width: 900px;
height: 360px;

plot {
  axis-top: off;
  axis-right: off;

  xdomain-format: datetime("%H:%M:%S");
  ydomain-format: fixed(1);

  series {
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
  }
}
