Scales
======

Scale settings control how input data is mapped to plot coordinates.

## Scale Types

You can change the scale type using the `scale-x` and `scale-y` properties on
the `plot` element:

    scale-x: <scale-options>;
    scale-y: <scale-options>;

The following scale options are currently supported:

  - `linear` - A simple linear scale from min to max
  - `logarithmic` - A logarithmic scale
  - `discrete` - A discrete (categorical) scale
  - `inverted` - A modifier that will invert any of the scales listed above

## Limits

The scale limits are the smallest (min) and largest (max) values that will be
displayed on the plot. Configure using the `axis-x-min`, `axis-x-max`, `axis-y-min` 
and `axis-y-max` properties.

    scale-x-min: <value>;
    scale-x-max: <value>;
    scale-y-min: <value>;
    scale-y-max: <value>;


## Categorical Values

## Logarithmic, Inverted

## Multiple Scales

