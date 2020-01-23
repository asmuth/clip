#!/usr/bin/env python
import math

n = 46
print("x,y1,y2,y3,y4")
for i in range(0, n):
  x = (i / float(n - 1)) * 2.0
  y1 = (3 + math.sin(x*1.5)  + math.sin(x*1.5 - 2)  * 4) * 2 + 4
  y2 = y1 + (3 + math.sin(x*1.2)  + math.sin(x*2 - 1.8)  * 3) * 2
  y3 = y2 + (3 + math.sin(x*1.5)  + math.sin(x*1.5 - 2)  * 4) * 2 + 4
  y4 = y3 + (3 + math.sin(x*1.5)  + math.sin(x*1.5 - 2)  * 4) * 2 + 4
  print("%f,%f,%f,%f,%f" % (x, y1, y2, y3, y4))
