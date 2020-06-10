class: plot;
size: 1750px 500px;

limit-x: 0 7200;
limit-y: 0 100;

axes {
  font: "Roboto";
  position: left bottom;
  label-placement-x: linear-interval(900 900 7000);
  label-format-x: datetime("%H:%M:%S");
}

lines {
  data: "test/testdata/timeseries.csv";
  stroke-width: 0.8pt;
  color: #06c;
}
