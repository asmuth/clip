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
      <th>property</th>
      <th>description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>from</strong></code></td>
      <td>Begin of the query time window (inclusive). Valid values are: ISO-8601-style dates, unix timestamps, `now` and relative references of the form `-NN[s(((ec)ond)s)|m(((in)ute)s)|h((our)s)|d((ay)s)|w((eek)s)|y((ear)s)]`. For example: `2017-01-29 14:00:00`, `now`, `-30min` or `-6h`</td>
    </tr>
    <tr>
      <td><code><strong>until</strong></code></td>
      <td>End of the time window (exclusive). See "from" (above) for list of valid values.</td>
    </tr>
    <tr>
      <td><code><strong>queries</strong></code></td>
      <td>List/array of query objects (see below)</td>
    </tr>
    <tr>
      <td><code><strong>axis_y_position</strong></code></td>
      <td>...</td>
    </tr>
    <tr>
      <td><code><strong>border_top</strong></code></td>
      <td>...</td>
    </tr>
    <tr>
      <td><code><strong>border_right</strong></code></td>
      <td>...</td>
    </tr>
    <tr>
      <td><code><strong>border_bottom</strong></code></td>
      <td>...</td>
    </tr>
    <tr>
      <td><code><strong>border_left</strong></code></td>
      <td>...</td>
    </tr>
  </tbody>
</table>

### Properties (Query Specification)

<table>
  <thead>
    <tr>
      <th>property</th>
      <th>description</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code><strong>query</strong></code></td>
      <td>JSON object containing a <a href="/documentation/charts/json_queries">JSON Query</a></td>
    </tr>
  </tbody>
</table>


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
