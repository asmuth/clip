size: 960px 280px;
dpi: 96;
limit-x: 0 13;
limit-y: 0 23;

axes {
  position: bottom;
  label-placement-x: linear-interval(1 1 12);
  label-placement-x: linear-interval(1 1 12);
  label-format-x: custom("Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec");
}

bars {
  data-x: csv("test/testdata/city_temperatures_london.csv" month);
  data-y: csv("test/testdata/city_temperatures_london.csv" temperature);
  labels: csv("test/testdata/city_temperatures_london.csv" temperature_str);
  width: 2em;
  color: 2;
}

legend {
  position: top left;
  border: none;

  item {
    label: "London - Average Monthly Temperature";
    color: 2;
  }
}
