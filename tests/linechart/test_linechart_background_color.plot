width: 1200px;
height: 600px;

background-color: #000;
foreground-color: #fff;

plot {
  series {
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
    color: #06c;
  }
}
