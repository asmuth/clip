size: 2048px 512px;
dpi: 240;
limit-x: 0 7200;
limit-y: -5 5;

axes {
  position: bottom left;
  label-format-y: scientific();
  label-format-x: datetime("%H:%M:%S");
  label-placement-x: linear-interval(900 900 7200);
}

lines {
  data-x: csv("test/testdata/measurement2.csv" time);
  data-y: csv("test/testdata/measurement2.csv" value3);
  color: #06c;
}

legend {
  position: top right;

  item {
    label: "Random Data";
    color: #06c;
  }
}
