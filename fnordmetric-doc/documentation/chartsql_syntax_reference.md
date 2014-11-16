ChartSQL Syntax Reference
=========================

### The DRAW statement

    common_chart_options ::=
        [ TITLE 'title' ]
        [ SUBTITLE 'subtitle' ]
        [ XDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ]
        [ YDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ]
        [ ZDOMAIN [ min, max ] [ LOGARITHMIC ] [ INVERT ] ]
        [ GRID [ HORIZONTAL ] [ VERTICAL ] ]
        [ axis_definition... ]
        [ legend_definition ]

    axis_definition ::=
        AXIS { TOP | RIGHT | BOTTOM | LEFT }
            [ TITLE 'title' ]
            [ TICKS [ { INSIDE | OUTSIDE | OFF } ] [ ROTATE deg ] ]

    legend_definition ::=
        LEGEND { TOP | BOTTOM } { LEFT | RIGHT} { INSIDE | OUTSIDE }
            [ TITLE 'title' ]

    DRAW AREACHART
        [ [ WITH ]
              common_chart_options
              [ STACKED ] ]

    DRAW BARCHART
        [ [ WITH ]
              common_chart_options
              [ ORIENTATION { HORIZONTAL | VERTICAL } ]
              [ STACKED ]
              [ LABELS ] ]

    DRAW HEATMAP
        [ [ WITH ]
              common_chart_options ]

    DRAW HISTOGRAM
        [ [ WITH ]
              common_chart_options ]

    DRAW LINECHART
        [ [ WITH ]
              common_chart_options ]

    DRAW POINTCHART
        [ [ WITH ]
              common_chart_options ]

### The IMPORT statement

    IMPORT TABLE tablename [, tablename]...
        FROM source_url;

Examples:

    IMPORT TABLE measurements
        FROM "csv://my_measurements.csv?headers=true";

    IMPORT TABLE products, users
        FROM "mysql://localhost:3306/mydatabase?user=hans&pass=geheim"


### The SELECT statement

    SELECT
        [ALL | DISTINCT | DISTINCTROW ]
        select_expr [, select_expr ...]
        [FROM table_references
        [WHERE where_condition]
        [GROUP [OVER TIMEWINDOW(time_column, window_expr [, step_expr]) ]
            [ BY {col_name | expr | position} ]
            [ASC | DESC], ...]
        [HAVING where_condition]
        [ORDER BY {col_name | expr | position}
            [ASC | DESC], ...]
        [LIMIT {[offset,] row_count | row_count OFFSET offset}]


