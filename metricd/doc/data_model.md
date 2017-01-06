Data Model
==========

The data model of metricd is very simple: The server stores a list of `metrics`.
Each metric contains zero or more `series`. The metric is mostly a configuration object (it 


      Server                   Metrics              Series


                                      +--------> +-----------+
                +--------> +------------+        | xxxxxxxxx |
                |          | =R1======+ |        | xxxxxxxxx |
                |          | =R2======= |        +-----------+
                |          | =R3======= |
                |          | =R4======+--------> +-----------+
    +-------------+        | =R4======= |        | xxxxxxxxx |
    | =M1=======+ |        |            |        | xxxxxxxxx |
    | =M2======== |        +------------+        +-----------+
    | =M3======== |
    | =M4=======+--------> +------------+------> +-----------+
    | =M5======== |        | =R1======+ |        | xxxxxxxxx |
    | =M6======== |        | =R1======= |        | xxxxxxxxx |
    +-------------+        | =R2======+ |        +-----------+
                           |          | |
                           |          +--------> +-----------+
                           |            |        | xxxxxxxxx |
                           +------------+        | xxxxxxxxx |
                                                 +-----------+




#### Example metric names:

    sys.cpu_util
    myapp.response_time
    /http/request_latency
    /apps/myapps/stats.daily_active_users
