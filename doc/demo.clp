class: plot;

limit-x: 0 7200;
limit-y: 0 100;

axes {
  font: "Latin Modern Roman";
  font-size: 12pt;
  label-format-x: datetime("%H:%M:%S");
  label-placement-x: linear-interval(900 900 7000);
}

lines {
  data-x: csv(test/testdata/timeseries.csv time);
  data-y: csv(test/testdata/timeseries.csv value);
  stroke-width: 0.8pt;
}
