

1. Design Overview

 +-------------------------------------------------------------+

     Transaction           PartitionMap          Partitions


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
                               ...
                                                     ...

 +-------------------------------------------------------------+

 - There is one global transaction, it contains a single pointer to each live
   metric's partition map.

 - A metric partitions map contains a number of split points (time values) and
   a list of partition pointers. The split points implicitly denote the min and
   max time value for each partition.


2. Considered Limits

  1_000_000 timeseries x 5_000_000 datapoints


3. In-Memory Datastructures

  - Both the global transaction and the partition maps are always kept in main
    memory

  - Concurrent access to the transaction is supported through MVCC. Every
    modification of the transaction (i.e. addition or removal of a metric)
    creates a new copy of the transaction. There is a writer lock that prevents
    more than one thread from trying to create a new transaction at the same
    time. However, readers do not need to grab the lock. The transaction is
    refcounted so pointers to old transactions stay valid after a new
    transaction was commited until the last reader has dropped their refernece.

  - The partition map is also refcounted and modified using copy-on write.

  - The partitions are refcounted, but readers and writers have to grab a lock
    to read or modify a page's data.


4. On-Disk Datastructure

 - The Transaction and PartitionMap may (later) themselves be stored as trees
   on disk to decrease write amplification. This will be fairly simple since
   all entries are constant-size. So we can simply derive the height of the
   tree from the number of stored entries
