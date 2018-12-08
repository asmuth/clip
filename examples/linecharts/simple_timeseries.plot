width: 1200px;
height: 480px;

plot {
  axis-top: off;
  axis-right: off;

  margin: 0px;
  margin-top: 1.1rem;
  margin-bottom: 2.6rem;
  margin-left: 5rem;
  margin-right: 3.4rem;

  xdomain-format: datetime("%H:%M:%S");
  ydomain-format: fixed(1);

  series {
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
  }
}
