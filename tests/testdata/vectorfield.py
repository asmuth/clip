#!/usr/bin/env python
import math
import numpy

print("x,y,dx,dy")
for x in numpy.arange(-3.8, 4.0, 0.2):
  for y in numpy.arange(-1.6, 2.0, 0.2):
    a = math.atan(y**2 - x);
    dx = math.sin(a)
    dy = math.cos(a)
    print("%f,%f,%f,%f" % (x , y - 0.14, max(dx, 0.0) * 0.1, dy * 0.08))

