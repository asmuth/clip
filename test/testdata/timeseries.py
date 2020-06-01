#!/usr/bin/env python
import math
import random

n = 400
d = 0
print("time,value")
for i in range(0, n*2):
  x = i / float(n)
  x1 = (0.7 - x) * 80

  y0 = -.3
  y1 = (math.sin(x1) / x1 if x1 != 0 else 1) * 0.8

  x2 = (0.3 - x) * 90
  y2 = (math.sin(x2) / x2 if x2 != 0 else 1) * 0.4

  y3 = random.gauss(0, 0.03)

  y4 = x * 0.5
  y5 = min(0, 1.8 + ((1.0 - x) * -2))

  x6 = (1.4 - x) * 50
  y6 = (math.sin(x6) / x6 if x6 != 0 else 1) * 0.9

  y = y0 + y1 + y2 + y3 + y4 + y5 + y6

  time = x * 3600
  value = (y * 32) + 32
  print("%f,%f" % (time, value))
