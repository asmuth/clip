plot {
  axis-top: off;
  axis-right: off;
  margin-right: 2.4em;

  ymin: -10;
  ymax: 32;
  xdomain: categorical;
  xdomain-format: string;

  series {
    title: "London";
    xs: csv(examples/data/city_temperatures_london.csv, 0);
    ys: csv(examples/data/city_temperatures_london.csv, 1);
    labels: csv(examples/data/city_temperatures_london.csv, 2);
    point-size: 3.3pt;
  }

}
