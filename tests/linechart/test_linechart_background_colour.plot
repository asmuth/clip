width: 1200px;
height: 600px;

background-colour: #000;
foreground-colour: #fff;

plot {
  series {
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
    colour: #06c;
  }
}
