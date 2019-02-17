#!/usr/bin/env python
import random

print("x,y")
for i in range(0, 1000):
  x = random.gauss(200.0, 50.0)
  y = random.gauss(100.0, 20.0)
  print("%f,%f" % (x, y))

