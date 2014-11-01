

SELECT value, time FROM num_http_requests;

-- number of requests per http status code over a moving 60 second window in
-- the last hour
SELECT
  status_code as series,
  delta(value) as y,
  time as x,
  "Number of HTTP " + status_code + " codes: " + y as label
FROM
  http_status_codes
WHERE
  time > -60minutes
GROUP BY
  TIMEWINDOW(time, 60, 10),
  status_code;

-- error rate computed from joining two metrics over a moving 60 second window
-- in the last hour
SELECT
  status_code as series,
  delta(error_metric.value) / delta(successful_requests_metric.value) as y,
  time as x,
  "error rate " + y as label
FROM
  successful_requests_metric,
  error_metric
WHERE
  time > -60minutes
GROUP BY
  TIMEWINDOW(time, 60, 10),
  status_code;


SELECT
  "http error rate" as series,
  num_200.time as x,
  num_500.delta / num_200.delta as y
FROM (
    SELECT
      time,
      delta(value) OVER as delta,
    WHERE
      time > -60minutes
      status_code = 200
    GROUP BY
      TIMEWINDOW(time, 60, 10);
  ) AS num_200
JOIN (
    SELECT
      time,
      delta(value) as delta,
    WHERE
      time > -60minutes
      status_code = 500
    GROUP BY
      TIMEWINDOW(time, 60, 10) AS window;
  ) AS num_500
) ON num_200.time = num_500.time;



