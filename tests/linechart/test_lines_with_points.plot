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

  ymin: -3;
  ymax: 30;
  xdomain: categorical;
  xdomain-format: string;

  series {
    xs: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec;
    ys: -0.2, 0.8, 5.7, 11.3, 17.0, 22.0, 24.8, 24.1, 20.1, 14.1, 8.6, 2.5;
    point-size: 3.3pt;
  }

  series {
    xs: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec;
    ys: 4.8, 6.6, 10.3, 14.2, 16.6, 17.0, 15.2, 11.9, 8.5, 5.7, 4.2, 3.9;
    point-size: 3.3pt;
  }
}
