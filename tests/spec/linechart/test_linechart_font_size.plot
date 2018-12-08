width: 1200px;
height: 480px;

font-size: 16pt;

plot {
  ymin: -10;
  ymax: 32;
  xdomain: categorical;
  xdomain-format: string;

  legend: inside top left;

  series {
    title: "New York";
    xs: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec;
    ys: -0.2, 0.8, 5.7, 11.3, 17.0, 22.0, 24.8, 24.1, 20.1, 14.1, 8.6, 2.5;
    point-size: 3.3pt;
  }
}
