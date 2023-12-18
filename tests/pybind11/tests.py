#!/usr/bin/env python
# heatmap - High performance heatmap creation in python
#
# The MIT License (MIT)
#
# Copyright (c) 2013 Joel Petersen
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

from typing import List
from pyheatmap import PyHeatmap, PyHeatmapStamp, PyColorScheme
from random import gauss
import os


def render_heatmap(rawimg: List[bytes], output: str, background_image: str):
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


def create_3x3_stamp():
    stamp_data = [
        0.0, 0.5, 0.0,
        0.5, 1.0, 0.5,
        0.0, 0.5, 0.0
    ]

    return PyHeatmapStamp(3, 3, stamp_data)


def test_render_random_README_example():
    from PIL import Image

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
    file_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), "heatmap.png")

    img.save(file_name, compress_level=1)
    assert os.path.exists(file_name), "This should have made an image."
    os.remove(file_name)

def test_render_to_saturating():

    expected1: [bytes] = [
                0, 0, 0, 0,   255, 255, 255, 255,   0, 0, 0, 0,
        255, 255, 255, 255,   255, 255, 255, 255,   255, 255, 255, 255,
                0, 0, 0, 0,   255, 255, 255, 255,   0, 0, 0, 0,
    ]

    expected2: [bytes] = [
                0, 0, 0, 0,   191, 191, 191, 255,   0, 0, 0, 0,
        191, 191, 191, 255,   255, 255, 255, 255,   191, 191, 191, 255,
                0, 0, 0, 0,   191, 191, 191, 255,   0, 0, 0, 0,
    ]

    hm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()

    hm.add_point_with_stamp(1, 1, stamp)
    hm.add_point_with_stamp(1, 1, stamp)
    hm.add_point_with_stamp(1, 1, stamp)

    rawimg = hm.render_saturated(PyColorScheme.b2w, 1.0)
    for idx in range(0, len(rawimg)):
        assert rawimg[idx] == expected1[idx], "saturated (1) rendered 3x3 heatmap is correct"

    rawimg = hm.render_saturated(PyColorScheme.b2w, 2.0)
    for idx in range(0, len(rawimg)):
        assert rawimg[idx] == expected2[idx], "saturated (2) rendered 3x3 heatmap is correct"

    # TODO: (Also try negative and non-one-max stamps?)


def test_render_to_normalizing():

    expected: [bytes] = [
                0, 0, 0, 0,   127, 127, 127, 255,   0, 0, 0, 0,
        127, 127, 127, 255,   255, 255, 255, 255,   127, 127, 127, 255,
                0, 0, 0, 0,   127, 127, 127, 255,   0, 0, 0, 0,
    ]

    hm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()
    hm.add_point_with_stamp(1, 1, stamp)

    rawimg = hm.render_with_color_scheme(PyColorScheme.b2w)

    for idx in range(0, len(rawimg)):
        assert rawimg[idx] == expected[idx], "simple rendered 3x3 heatmap is correct"

    hm.add_point_with_stamp(1, 1, stamp)

    rawimg = hm.render_with_color_scheme(PyColorScheme.b2w)

    for idx in range(0, len(rawimg)):
        assert rawimg[idx] == expected[idx], "two points on one spot don't change the outcome, due to normalization."

    # TODO: (Also try negative and non-one-max stamps?)


def test_render_to_creation():
    expected: [bytes] = [
                0, 0, 0, 0,   127, 127, 127, 255,   0, 0, 0, 0,
        127, 127, 127, 255,   255, 255, 255, 255,   127, 127, 127, 255,
                0, 0, 0, 0,   127, 127, 127, 255,   0, 0, 0, 0,
    ]

    libhm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()
    libhm.add_point_with_stamp(1, 1, stamp)

    rawimg = libhm.render_with_color_scheme(PyColorScheme.b2w)
    for idx in range(0, len(rawimg)):
        assert rawimg[idx] == expected[idx], "simple rendered 3x3 heatmap is correct"

def test_render_to_nothing():
    expected = b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
    
    libhm = PyHeatmap(3, 3)

    rawimg = libhm.render_with_color_scheme(PyColorScheme.b2w)
    assert expected == rawimg, "empty rendered 3x3 heatmap is correct"

def test_stamp_gen_nonlinear():
    ones = [
        1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
    ]

    halves = [
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
    ]

    zeroes = [
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
    ]

    s1 = PyHeatmapStamp(1, lambda d: 0.5)
    s2 = PyHeatmapStamp(1, lambda d: -1.0)
    s3 = PyHeatmapStamp(1, lambda d: 10.0)

    halves_stamp = PyHeatmapStamp(3, 3, halves)
    ones_stamp = PyHeatmapStamp(3, 3, ones)
    zeros_stamp = PyHeatmapStamp(3, 3, zeroes)

    assert s1.width() == 3 and s1.height() == 3, "the 1-radius nonlinear .5-stamp size is correct"
    assert s2.width() == 3 and s2.height() == 3, "the 1-radius nonlinear -1-stamp size is correct"
    assert s3.width() == 3 and s3.height() == 3, "the 1-radius nonlinear 10-stamp size is correct"
    assert s1 == halves_stamp, "the 1-radius nonlinear .5-stamp data is correct"
    assert s2 == ones_stamp, "the 1-radius nonlinear -1-stamp data is correct"
    assert s3 == zeros_stamp, "the 1-radius nonlinear 10-stamp data is correct"


def test_stamp_gen():

    expected = [
        0.2928932, 0.5, 0.2928932,
        0.5, 1.0, 0.5,
        0.2928932, 0.5, 0.2928932,
    ]

    s = PyHeatmapStamp(1)
    expected_stamp = PyHeatmapStamp(3, 3, expected)

    assert s.width() == 3 and s.height() == 3, "the 1-radius stamp size is correct"
    assert s.almost_eq(expected_stamp), "the 1-radius stamp data is correct"

def test_add_point_with_stamp_outside():
    expected = [
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
    ]
    expected_stamp = PyHeatmapStamp(3, 3, expected)

    stamp = create_3x3_stamp()

    hm = PyHeatmap(3, 3)
    hm.add_point_with_stamp(3, 2, stamp)
    hm.add_point_with_stamp(2, 3, stamp)
    hm.add_point_with_stamp(3, 3, stamp)

    assert hm.is_stamp_equal(expected_stamp), "no point outside the map got added"
    assert hm.max() == 0.0, "the max of the heatmap is zero"

    hm.add_weighted_point_with_stamp(3, 2, 1.5, stamp)
    hm.add_weighted_point_with_stamp(2, 3, 1.5, stamp)
    hm.add_weighted_point_with_stamp(3, 3, 1.5, stamp)

    assert hm.is_stamp_equal(expected_stamp), "no weighted point outside the map got added"
    assert hm.max() == 0.0, "the max of the heatmap is zero"

def test_add_points_with_stamp_outside():
    expected = [
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
    ]
    expected_stamp = PyHeatmapStamp(3, 3, expected)

    stamp = create_3x3_stamp()

    hm = PyHeatmap(3, 3)

    pair_points = [(3, 2), (2, 3), (3, 3)]
    hm.add_points_with_stamp(pair_points, stamp)

    assert hm.is_stamp_equal(expected_stamp), "no point outside the map got added"
    assert hm.max() == 0.0, "the max of the heatmap is zero"

    pair_points = [(3, 2), (2, 3), (3, 3)]

    hm.add_weighted_points_with_stamp(pair_points, 1.5, stamp)

    assert hm.is_stamp_equal(expected_stamp), "no weighted point outside the map got added"
    assert hm.max() == 0.0, "the max of the heatmap is zero"


def test_add_point_with_stamp_botright():

    expected = [
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.5,
        0.0, 0.5, 1.0
    ]

    hm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()

    hm.add_point_with_stamp(2, 2, stamp)

    expected_stamp = PyHeatmapStamp(3, 3, expected)
    assert hm.is_stamp_equal(expected_stamp), "bot-right point is correct"
    assert hm.max() == 1.0, "the max of the heatmap is one"

    hm2 = PyHeatmap(3, 3)
    hm.add_point_with_stamp(2, 2, stamp)
    hm2.add_weighted_point_with_stamp(2, 2, 2.0, stamp)

    assert hm2 == hm, "a point with weight 2.0 generates the same heatmap as two regular ones."


def test_add_point_with_stamp_topleft():
    expected = [
        1.0, 0.5, 0.0,
        0.5, 0.0, 0.0,
        0.0, 0.0, 0.0,
    ]

    hm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()

    hm.add_point_with_stamp(0, 0, stamp)

    expected_stamp = PyHeatmapStamp(3, 3, expected)
    assert hm.is_stamp_equal(expected_stamp), "top-left point is correct"

    assert hm.max() == 1.0, "the max of the heatmap is one"

    hm2 = PyHeatmap(3, 3)
    # add 2 more to compare to 3x weighted stamp
    hm.add_point_with_stamp(0, 0, stamp)
    hm.add_point_with_stamp(0, 0, stamp)
    hm2.add_weighted_point_with_stamp(0, 0, 3.0, stamp)

    assert hm2 == hm, "a point with weight 3.0 generates the same heatmap as three regular ones."


def test_add_points_with_stamp_topleft():
    expected = [
        1.0, 0.5, 0.0,
        0.5, 0.0, 0.0,
        0.0, 0.0, 0.0,
    ]

    hm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()

    points = [(0,0), (0,0), (0,0)]
    hm.add_points_with_stamp(points, stamp)

    hm2 = PyHeatmap(3, 3)
    # add 2 more to compare to 3x weighted stamp
    hm2.add_weighted_point_with_stamp(0, 0, 3.0, stamp)

    assert hm2 == hm, "a point with weight 3.0 generates the same heatmap as three regular ones."

def test_add_point_with_stamp_center():
    libhm = PyHeatmap(3, 3)
    stamp = create_3x3_stamp()
    libhm.add_point_with_stamp(1, 1, stamp)

    assert libhm.is_stamp_equal(stamp), "the heatmap not equal to the same size stamp"

    assert libhm.max() == 1.0, "the max of the heatmap is one"

    libhm.add_point_with_stamp(1, 1, stamp)

    expected = [
        0.0, 1.0, 0.0,
        1.0, 2.0, 1.0,
        0.0, 1.0, 0.0
    ]

    double_strong_stamp = PyHeatmapStamp(3, 3, expected)
    assert libhm.is_stamp_equal(double_strong_stamp), "the heatmap equals double the stamp"

    assert libhm.max() == 2.0, "the max of the heatmap is two"

    libhm2 = PyHeatmap(3, 3)
    libhm2.add_weighted_point_with_stamp(1, 1, 2.0, stamp)

    assert libhm == libhm2, "a point with weight 2.0 generates the same heatmap."

def test_always_passes():
    assert True

def test_not_equality():
    libhm = PyHeatmap(3, 3)
    libhm2 = PyHeatmap(2, 2)
    assert libhm != libhm2

def test_equality():
    libhm = PyHeatmap(3, 3)
    libhm2 = PyHeatmap(3, 3)
    assert libhm == libhm2

def test_add_nothing():
    libhm = PyHeatmap(3, 3)
    rawimg : bytearray = libhm.render_default()
    for idx in range(0, len(rawimg)):
        assert (rawimg[idx] == 0)
    assert (libhm.max() == 0)

if __name__ == "__main__":
    test_add_nothing()
    test_equality()
    test_not_equality()
    test_add_point_with_stamp_center()
    test_add_point_with_stamp_topleft()
    test_add_points_with_stamp_topleft()
    test_add_point_with_stamp_botright()
    test_add_point_with_stamp_outside()

    test_add_points_with_stamp_outside()

    test_stamp_gen()
    test_stamp_gen_nonlinear()

    test_render_to_nothing()
    test_render_to_creation()
    test_render_to_normalizing()
    test_render_to_saturating()

    exit(0)
