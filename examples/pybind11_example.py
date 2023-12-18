#!/usr/bin/env python

import argparse
from pyheatmap import PyHeatmap, PyHeatmapStamp, PyColorScheme
from random import gauss
import numpy as np
import sys
import time
import collections 

def render_heatmap(rawimg: list[bytes], output: str, background_image: str):
    from PIL import Image
    heatmap_img = Image.frombuffer('RGBA', (w, h), rawimg, 'raw', 'RGBA', 0, 1)
    if background_image:
        heatmap_img.putalpha(127)
        background = Image.open(background_image)
        background.paste(heatmap_img, (0, 0), heatmap_img)
        background.show()
        background.save(output, compress_level=1) 
    else:
        heatmap_img.save(output, compress_level=1) 


def create_heatmaps_with_individual_points(w: int, h: int, npoints: int, stamp_size: int, pointsx : np.array, pointsy : np.array):
    libhm = PyHeatmap(w, h)
    libm_stamp = PyHeatmapStamp(stamp_size)

    for i in range(npoints):
        int_x = int(pointsx[i])
        int_y = int(pointsy[i])
        libhm.add_point_with_stamp(int_x, int_y, libm_stamp)

    rawimg = libhm.render_with_color_scheme(PyColorScheme.Spectral_mixed)
    return rawimg

def create_heatmaps_with_multiple_points(w: int, h: int, npoints: int, stamp_size: int, pointsx : np.array, pointsy : np.array):
    libhm = PyHeatmap(w, h)
    libm_stamp = PyHeatmapStamp(stamp_size)
    pair_points = []
    for i in range(npoints):
        int_x = int(pointsx[i])
        int_y = int(pointsy[i])
        pair_points.append((int_x, int_y))
    libhm.add_points_with_stamp(pair_points, libm_stamp)

if __name__ == "__main__":
    w, h, npoints = 2560, 5120, 200000

    parser = argparse.ArgumentParser(description='Generate Heatmap from file.')
    parser.add_argument('-f', '--file', required=True, help='file of pairs of points separated by new lines.')
    parser.add_argument('-b', '--background_image', help='background_image')
    parser.add_argument('-o', '--output', help='output file')
    args = parser.parse_args()

    stamp_size = 10

    pointsx = None
    pointsy = None
    if args.file:
        try:
            with np.load(args.file) as fp:
                pointsx = fp['ptsx']
                pointsy = fp['ptsy']
                width = fp.get('w')
                if width and width.size > 0:
                    w = width.astype(int) #[1]
                else:
                    print("input file missing 'w' as key for width.")
                height = fp.get('h')
                if height and height.size > 0:
                    h = height.astype(int) #[1]
                else:
                    print("input file missing 'h' as key for height.")
                npoints = len(pointsx)            
                print("loading new indices from " + args.file)
        except FileNotFoundError:
                pointsx = np.random.normal(w*0.5, w/6.0, npoints)
                pointsy = np.random.normal(h*0.5, h/6.0, npoints)
                np.savez(args.file, ptsx=pointsx, ptsy=pointsy, h=h, w=w)
                print("saving new indices to " + args.file)
        except:
                print("file missing some 'ptsx' or 'ptsy'. in file " + args.file)
    else:
        # Add a bunch of random points to the heatmap now.
        # numpy saves me about a 1s (8.61s vs 9.62s for 1000000)
        pointsx = np.random.normal(w*0.5, w/6.0, npoints)
        pointsy = np.random.normal(h*0.5, h/6.0, npoints)

    create_heatmaps_with_individual_points(w, h, npoints, stamp_size, pointsx, pointsy)
    rawimg = create_heatmaps_with_multiple_points(w, h, npoints, stamp_size, pointsx, pointsy)
    if args.output:
        render_heatmap(rawimg, args.output, args.background_image)
    else:
        print("Heatmap generated, but not output as image.")
