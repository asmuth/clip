Chart Options
=============

This page describes the chart specification JSON. The chart specification allows
you to define custom charts and embed them using the IFrame widget or by rendering
them to a vector or raster image.

Here is a simple example:

    {
      title: "Users online"
      from: "-2h",
      until: "now",
      queries: [
        {
          query: {
            op: fetch_summary,
            metric_id: users_online
          }
        }
      ]
    }


### Properties

<table>
  <thead>
    <tr>
      <th>Property</th>
      <th>Description</th>
      <th>Default</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><b>title</b></code></td>
      <td>Here be dragons</td>
      <td>false</td>
    </tr>
    <tr>
      <td>show_plot</td>
      <td>Here be dragons</td>
      <td>false</td>
    </tr>
    <tr>
      <td>show_summary</td>
      <td>Here be dragons</td>
      <td>false</td>
    </tr>
    <tr>
      <td>show_series_list</td>
      <td>Here be dragons</td>
      <td>false</td>
    </tr>
    <tr>
      <td>show_summary</td>
      <td>Here be dragons</td>
      <td>false</td>
    </tr>
  </tbody>
</table>

### Properties (Query)



### Example:

    {
      title: "My title"
      show_summary: title,
      show_summary: true,
      show_plot: true,
      show_series_list: true,
      queries: [
        {
          draw_type "line",
          draw_color: "#0f0"
          query: {
            op: fetch
            metric_id: mymetric
          }
        }
      ]
    }
