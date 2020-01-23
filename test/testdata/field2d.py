#!/usr/bin/env python
import math
import numpy

d=0.1
print("x,y,a")
for x in numpy.arange(-5, 5, d):
  for y in numpy.arange(-2, 2.0, d):
    a = min(abs(1 - ((x * 0.7)**2 + (y*0.9) ** 2)), 1.0)
    a -= (((x * 0.2)**2 + (y*0.2) ** 2)) * 0.8
    print("%f,%f,%f" % (x * 100 + 5, y * 100 + 5, a))

