The IMPORT Statement
==================

The IMPORT statement allows you to import tables from external data sources, like
CSV files or a MySQL database. The general syntax of the IMPORT statement is:

    IMPORT TABLE <table> [, <table>]... FROM <source_url>;

The source url specifies the external source from which the tables should be
imported. For example, to import the table `mytable` from a remote MySQL
database:

    IMPORT TABLE mytable FROM 'mysql://localhost:3306/mydatabase';
    SELECT * FROM mytable;

Or to import a CSV file:

    IMPORT TABLE measurements FROM 'csv://measurements.csv?headers=true';
    SELECT * FROM measurements;


CSV Files
---------

Use the `csv://` source url prefix to import from csv files. The url format
for csv files is:

    csv://path/to/csv_file.csv?param=value...

You can import exactly one table per IMPORT statement when retrieving data from CSV.
The valid parameters for csv files urls are:

<table style="width:100%;">
  <tr>
    <th>headers</th>
    <td>
      does the csv file start with a header line? (valid values are `true` or `false`)
    </td>
  </tr>
  <tr>
    <th>quote_char</th>
    <td>
      the CSV row quote char (default is `"`)
    </td>
  </tr>
  <tr>
    <th>col_sep</th>
    <td>
      the CSV column separator char (default is `,`)
    </td>
  </tr>
  <tr>
    <th>row_sep</th>
    <td>
      the CSV row separator char (default is `\n`)
    </td>
  </tr>
  <tr>
    <th>escape_char</th>
    <td>
      the CSV row scape char (default is `\`)
    </td>
  </tr>
</table>
<br />

Some examples:

    csv://path/to/csv_file.csv?headers=true
    csv://path/to/csv_file.csv?headers=true&col_sep=,
    csv:///absolute_path/to/csv_file.csv



MySQL
-----

Use the `mysql://` source url prefix to import tables from a mysql database.
The url format for mysql databases is:

    mysql://host:port/database?param=value...

You can import one or more tables per IMPORT statement when importing from a
MySQL database. The valid parameters for mysql database urls are:

<table style="width:100%;">
  <tr>
    <th>username</th>
    <td>
      the mysql username
    </td>
  </tr>
  <tr>
    <th>password</th>
    <td>
      the mysql password
    </td>
  </tr>
</table>
<br />

Some examples:

    mysql://localhost:3306/mydb?username=root
    mysql://127.0.0.1:3306/fnordmetric?username=hans&password=geheim


