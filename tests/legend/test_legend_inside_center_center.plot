width: 1200px;
height: 480px;

plot {
  legend: inside center center;
  legend-title: "The quick brown fox";

  series {
    title: "Series 1";
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 2);
  }

  series {
    title: "Series 2";
    xs: csv('tests/testdata/measurement.csv', 0);
    ys: csv('tests/testdata/measurement.csv', 4);
  }
}

