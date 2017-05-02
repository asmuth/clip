var elem = document.querySelector(".chart_elem");

describe("FnordMetricMetricSeriesListChart", function() {
  it("should render a summary elem with a sum value and unit", function() {
    var now = Date.now();
    FnordMetricMetricSeriesListChart(elem, {
      summary: {
        series_id: "cur_value"
      },
      time: [now, now - 1000, now - 2000],
      series: [
        {
          series_id: "cur_value",
          title: "Current Value",
          unit: "KB/s",
          values: [1, 4, 6],
          summaries: [
            {
              summary: "sum",
              value: 76067
            }
          ]
        }
      ]
    });

    expect(elem.querySelector(".summary")).not.toBe(null);
    expect(elem.querySelector(".summary .total").innerHTML).toBe("76067 KB/s");
  });

  it("should render a legend", function() {
    expect(elem.querySelector(".legend")).not.toBe(null);
  });
});

