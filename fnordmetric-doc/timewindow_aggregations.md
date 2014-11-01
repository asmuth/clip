

-- display the last hour of measurements
SELECT "mymetric" as series, time as x, value as y FROM mymetric,

-- display the first derivative of our measurement over a moving 60s window
SELECT time as x, delta(value) as FROM mymetric GROUP BY TIMEWINDOW(60, 10);

-- display the first derivative of our measurement over a moving 60s window in the last hour
SELECT time as x, delta(value) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10)
  WHERE time > -60mins;

-- ....equivalent to
SELECT time as x, delta(value) as y
  FROM mymetric
  GROUP BY TIMEWINDOW(time, 60, 10)
  SINCE -60mins UNTIL now;

-- number of requests per http status code over a moving 60 second window in
-- the last hour
SELECT status_code as series, time as x, delta(value) as y
  FROM http_status_codes
  GROUP BY TIMEWINDOW(time, 60, 10), status_code;

-- error rate computed from joining two metrics over a moving 60 second window
-- in the last hour
SELECT
  hostname as series,
  time as x,
  delta(error_metric.value) / delta(success_metric.value) as y
FROM
  success_metric,
  error_metric
WHERE
  time > -60minutes
GROUP BY
  TIMEWINDOW(time, 60, 10),
  hostname;

-- error rate computed from joining the first derivates of two metrics over a
-- moving 60 second window in the last hour
SELECT
  "http error rate" as series,
  num_200.time as x,
  num_500.val / num_200.val as y
FROM (
    SELECT time, delta(value) AS val
      FROM http_status_codes
      SINCE -60minutes UNTIL now
      WHERE status_code = 200
      GROUP BY TIMEWINDOW(time, 60, 10);
  ) AS num_200
JOIN (
    SELECT time, delta(value) AS val
      FROM http_status_codes
      SINCE -60minutes UNTIL now
      WHERE status_code = 5200
      GROUP BY TIMEWINDOW(time, 60, 10);
  ) AS num_500
) ON num_200.time = num_500.time;

-- error rate computed from joining the first derivates of two metrics over a
-- moving 60 second window in the last hour
SELECT
  "http error rate" as series,
  num_200.time as x,
  num_500.val / num_200.val as y
FROM (
    SELECT time, delta(value) AS val
      FROM http_status_codes
      SINCE -60minutes UNTIL now
      WHERE status_code = 200
      GROUP BY TIMEWINDOW(time, 60, 10);
  ) AS num_200, (
    SELECT time, delta(value) AS val
      FROM http_status_codes
      SINCE -60minutes UNTIL now
      WHERE status_code = 5200
      GROUP BY TIMEWINDOW(time, 60, 10);
  ) AS num_500
WHERE num_200.time = num_500.time;




