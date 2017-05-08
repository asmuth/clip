Units of Measurement
====================

The `unit` stanza allows you configure a unit of measurement. The unit configuration
will be used at display time to create humnan-readable output from the raw values.
The general config syntax to declare units is:


    unit <unit_id> {
      unit_name <unit_name_id> <factor> <singular> <plural> <symbol>
      ...
    }

A unit must have one or more `unit_name`s. The appropiate unit name will be
chosen based on the factor and value to be formatted. Consider the following
example:

    unit byte {
      unit_name bytes      1           "Byte"      "Bytes"      "B"
      unit_name kilobytes  1000        "Kilobyte"  "Kilobytes"  "KB"
      unit_name megabytes  1000000     "Megabyte"  "Megabytes"  "MB"
      unit_name gigabytes  1000000000  "Gigabyte"  "Gigabytes"  "GB"
    }

    metric disk_space_used {
      unit byte
    }

