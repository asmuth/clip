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

  ymin: -10;
  ymax: 30;
  xdomain: categorical;
  xdomain-format: string;

  series {
    title: "New York";
    xs: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec;
    ys: -0.2, 0.8, 5.7, 11.3, 17.0, 22.0, 24.8, 24.1, 20.1, 14.1, 8.6, 2.5;
    point-size: 3.3pt;
  }

  series {
    title: "Beijing";
    xs: Jan, Feb, Mar, Apr,  May,  Jun,  Jul,  Aug,  Sep,  Oct,  Nov, Dec;
    ys: -4, -1.4, 5.4, 13.5, 19.9, 24.3, 26.3, 24.9, 19.9, 13.1, 4.7, -1.8;
    point-size: 3.3pt;
  }

  series {
    title: "Москва";
    xs: Jan,  Feb,  Mar,  Apr, May,  Jun,  Jul,  Aug,  Sep,  Oct, Nov, Dec;
    ys: -7.4, -5.8, -0.1, 7.1, 14.7, 17.7, 20.6, 18.8, 12.6, 6.1, 1.1, -3.6;
    point-size: 3.3pt;
  }

  series {
    title: "Berlin";
    xs: Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec;
    ys: -0.9, 0.6, 3.5, 8.4, 13.5, 17.0, 18.6, 17.9, 14.3, 9.0, 3.9, 1.0;
    point-size: 3.3pt;
  }

}
