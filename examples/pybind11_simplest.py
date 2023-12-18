#!/usr/bin/env python

from pyheatmap import PyHeatmap, PyHeatmapStamp, PyColorScheme
from PIL import Image
from random import gauss

w, h, npoints = 256, 512, 1000

# Create the heatmap object with the given dimensions (in pixel).
hm = PyHeatmap(w, h)

# Add a bunch of random points to the heatmap now.
for x, y in ((int(gauss(w*0.5, w/6.0)), int(gauss(h*0.5, h/6.0))) for _ in range(0, npoints)):
    hm.add_point(x, y)

# This creates an image out of the heatmap.
# `rawimg` now contains the image data in 32-bit RGBA.
rawimg = hm.render_default()

# Use the PIL (for example) to make a png file out of that.
img = Image.frombuffer('RGBA', (w, h), rawimg, 'raw', 'RGBA', 0, 1)

img.save("heatmap.png", compress_level=1)
