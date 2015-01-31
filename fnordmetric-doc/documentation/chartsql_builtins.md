ChartSQL Built-in Functions
===========================


Aggregate Functions
-------------------

#### sum(expr)
Returns the sum of all values in the result set.

#### count(expr)
Returns the number of values in the result set

#### mean(expr)
Returns the mean/average of values in the result set

#### min(expr)
Returns the min of values in the result set

#### max(expr)
Returns the max of values in the result set


Boolean Functions
-----------------

#### neg(expr) / !expr
Returns false if the `expr` is true and false if the `expr` is true.

#### eq(a, b) / a == b
Returns true if the values `a` and `b` are equal.

#### eq(a, b) / a != b
Returns true if the values `a` and `b` are not equal.

#### lt(a, b) / a < b
Returns true if the values `a` is strictly less than `b`

#### lte(a, b) / a <= b
Returns true if the values `a` is strictly less than or equal to `b`

#### gt(a, b) / a > b
Returns true if the values `a` is strictly greater than `b`

#### gte(a, b) / a >= b
Returns true if the values `a` is strictly greater than or equal to `b`

#### and(a, b) / a && b
Returns true if the values `a` and `b` are both true.

#### or(a, b) / a || b
Returns true if one or both of the values `a` and `b` are true.


DateTime Functions
------------------

#### FROM_TIMESTAMP(timestamp)
Convert a numeric unix timestamp into a DateTime value.


Math Functions
--------------

#### add(a, b) / a + b
Returns the sum of the values `a` and `b`.

#### sum(a, b) / a - b
Returns the difference of the values `a` and `b`.

#### mul(a, b) / a * b
Returns the product of the values `a` and `b`.

#### div(a, b) / a / b
Returns the quotient of the values `a` and `b`.

#### mod(a, b) / a % b
Returns the modulo of the values `a` and `b`.

#### pow(a, b)
Returns `a` to the power of `b`.
