size: 1600px 800px;
dpi: 240;
scale-x-padding: 0.6;
limit-x: 0.5 12.5;
limit-y: 0 25;

axes {
  position: bottom;
  label-placement-x: linear-alignat(1 1);
  label-format-x: custom("Jan" "Feb" "Mar" "Apr" "May" "Jun" "Jul" "Aug" "Sep" "Oct" "Nov" "Dec");
  label-placement-y: linear-alignat(5 -10);
}

lines {
  data-x: csv("test/testdata/city_temperatures_london.csv" month);
  data-y: csv("test/testdata/city_temperatures_london.csv" temperature);
  labels: csv("test/testdata/city_temperatures_london.csv" temperature_str);
  label-font-size: 10pt;
  color: 1;
  marker-size: 5pt;
}

legend {

  item {
    label: "London - Average Temperature";
    color: 1;
  }
  item-flow: on;
  border: none;
}
