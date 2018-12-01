width: 1200px;
height: 600px;

plot {
  axis-top: off;
  axis-right: off;

  ydomain-format: fixed(1);

  series {
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
  }
}
