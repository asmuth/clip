size: 2048px 512px;
dpi: 240;
font: "Comic Sans MS";
limit-x: 1404278100 1404299700;
limit-y: 0 120;

grid {
  color: #fff;
}

axes {
  position: bottom left;
  label-format-y: fixed(2);
  label-format-x: datetime("%H:%M:%S");
  label-placement-x: linear(2400);
  label-placement-y: linear(20);
}

lines {
  data-x: csv("test/testdata/measurement.csv" time);
  data-y: csv("test/testdata/measurement.csv" value2);
}

legend {
  position: top right;
  border: none;
  margin: 0.5em;
  item-flow: on;

  item {
    label: "A very serious measurement";
  }
}
