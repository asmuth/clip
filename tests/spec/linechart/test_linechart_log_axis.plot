width: 1200px;
height: 480px;

plot {
  title: "To the Moon";

  data: csv('examples/data/log_example.csv', 0);
  x: $0;
  y: $1;

  axis-x-type: log;
  axis-x-min: 0;
  axis-x-max: 5;

  axis-y-type: log;
  axis-y-min: 0;
  axis-y-max: 10000;

  layer {
    type: lines;
    line-color: $0;
  }

  layer {
    type: points;
    line-color: $0;
  }
}
