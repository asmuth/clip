width: 1200px;
height: 480px;

plot {
  series {
    xs: csv('examples/data/irregular_data2.csv', 0);
    ys: csv('examples/data/irregular_data2.csv', 1);
    point-size: 3pt;
    title: "series 1";
  }
  series {
    xs: csv('examples/data/irregular_data1.csv', 0);
    ys: csv('examples/data/irregular_data1.csv', 1);
    point-size: 3pt;
    title: "series 1";
  }
}
