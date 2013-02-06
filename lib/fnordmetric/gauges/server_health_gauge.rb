# ServerHealthGauge
# here be dragons....
#
# This gauge listens for JSON events that look like this:
#
# {
#   "hostname": "my-hostname",
#   "uptime": 1231523,
#   "load_avg": [0.32, 0.16, 0.13],
#   "mem": { "total": 4069, "free": 1096 },
#   "swap": { "total": 0, "free": 0 },
#   "disk_usage": "43%"
# }
