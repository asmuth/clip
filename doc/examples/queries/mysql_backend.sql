IMPORT TABLE test_table
   FROM 'mysql://localhost/test_database?user=root';


SELECT series, x, y FROM test_table;

