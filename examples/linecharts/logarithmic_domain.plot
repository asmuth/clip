width: 1200px;
height: 480px;

plot {
  axis-top: off;
  axis-right: off;

  xdomain: log;
  xmin: 0;
  xmax: 5;

  ydomain: log;
  ymin: 0;
  ymax: 10000;

  series {
    title: "To the Moon";
    xs: csv('examples/data/log_example.csv', 0);
    ys: csv('examples/data/log_example.csv', 1);
    point-size: 3pt;
  }
}
