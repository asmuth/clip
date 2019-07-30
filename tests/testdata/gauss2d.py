#!/usr/bin/env python
import random

print("x,y,z")
for i in range(0, 1000):
  x = random.gauss(200.0, 50.0)
  y = random.gauss(100.0, 20.0)
  z = min(1.0, max(0, (y - 60) / 60.0))
  print("%f,%f,%f" % (x, y,z ))

