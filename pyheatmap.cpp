/* pyheatmap - High performance heatmap creation in C++ and python
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Joel Petersen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software") \ to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/pytypes.h>
#include <pybind11/functional.h>
#include "heatmap.h"
#include "pycolorschemes.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

using namespace pybind11::literals;

namespace py = pybind11;

static bool almost_eq_float_array(float* a, float* b, size_t n)
{
    for(size_t i = 0 ; i < n ; ++i) {
        if(std::abs(a[i] - b[i]) > 1e-6)
            return false;
    }

    return true;
}

class PyHeatmapStamp
{
public:
    ~PyHeatmapStamp()
    {
        if (stamp != nullptr)
        {
            heatmap_stamp_free(stamp);
        }
    }

    PyHeatmapStamp(int radius)
    {
        //stamp = heatmap_stamp_gen_nonlinear(radius, [](float d){return sqrtf(d);});
        stamp = heatmap_stamp_gen(radius);
    }

    PyHeatmapStamp(unsigned int radius, const std::function<float(float)> &f)
    {
        // I dont like this, but I cant seem to get a capture working.
        // even trying to work off of https://adroit-things.com/programming/c-cpp/how-to-bind-lambda-to-function-pointer/
        static auto captured_func = f;
        captured_func = f;
        stamp = heatmap_stamp_gen_nonlinear(radius, [](float d) { return captured_func(d); });
    }

    PyHeatmapStamp(unsigned w, unsigned h, const std::vector<float>& data)
    {
        if (data.size() != w*h)
        {
            throw std::runtime_error("data size doesnt match w & h passed in.");
            return;
        }
        const float* data_ptr = data.data();
        stamp = heatmap_stamp_load(w, h, data_ptr);
    }

    int get_width() const {
        return stamp != nullptr ? stamp->w : -1;
    }

    int get_height() const {
        return stamp != nullptr ? stamp->h : -1;
    }

    bool almost_eq(const PyHeatmapStamp& b) const
    {
        const heatmap_stamp_t* self = get_stamp();
        const heatmap_stamp_t* stamp = b.get_stamp();
        bool is_null = stamp == nullptr;
        bool b_is_null = stamp == nullptr;
        if (is_null != b_is_null)
        {
            return false;
        }
        else if (self == nullptr || stamp == nullptr)
        {
            return true;
        }

        return stamp->w == self->w
            && stamp->h == self->h
            && almost_eq_float_array(stamp->buf, self->buf, self->w*self->h);
    }

    bool operator==(const PyHeatmapStamp& b) const
    {
        const heatmap_stamp_t* b_stamp = b.get_stamp();
        bool is_null = stamp == nullptr;
        bool b_is_null = b_stamp == nullptr;
        if (is_null != b_is_null)
        {
            return false;
        }
        else if (stamp == nullptr || b_stamp == nullptr)
        {
            return true;
        }

        return stamp->w == b_stamp->w
            && stamp->h == b_stamp->h
            && 0 == memcmp(stamp->buf, b_stamp->buf, sizeof(float)*stamp->w*stamp->h);
    }

    void debug_print()
    {
        if (stamp == nullptr)
        {
            py::print("no stamp");
            return;
        }
        for (unsigned int i = 0; i < stamp->w*stamp->h; i++)
        {
            py::print(stamp->buf[i]);
        }
    }

    const heatmap_stamp_t* get_stamp() const { return stamp; }

private:
    heatmap_stamp_t* stamp = nullptr;
};

class PyHeatmap
{
public:
    PyHeatmap(unsigned w, unsigned h)
    {
        heatmap = heatmap_new(w, h);
    }

    ~PyHeatmap()
    {
        if (heatmap != nullptr)
        {
            heatmap_free(heatmap);
        }
    }
    
    float get_max()
    {
        return heatmap != nullptr ? heatmap->max: -1.0f;
    }

    bool operator==(const PyHeatmap& b) const
    {
        bool is_null = heatmap == nullptr;
        bool b_is_null = b.heatmap == nullptr;
        if (is_null != b_is_null)
        {
            return false;
        }
        else if (heatmap == nullptr || b.heatmap == nullptr)
        {
            return true;
        }

        return heatmap->w == b.heatmap->w
            && heatmap->h == b.heatmap->h
            && 0 == memcmp(b.heatmap->buf, heatmap->buf, sizeof(float)*heatmap->w*heatmap->h);
    }

    bool is_stamp_equal(const PyHeatmapStamp& py_stamp) const
    {
        const heatmap_stamp_t* stamp = py_stamp.get_stamp();
        bool is_null = heatmap == nullptr;
        bool b_is_null = stamp == nullptr;
        if (is_null != b_is_null)
        {
            return false;
        }
        else if (heatmap == nullptr || stamp == nullptr)
        {
            return true;
        }
        
        return heatmap->w == stamp->w
            && heatmap->h == stamp->h
            && 0 == memcmp(stamp->buf, heatmap->buf, sizeof(float)*heatmap->w*heatmap->h);
    }

    void add_point(int x, int y)
    {
        heatmap_add_point(heatmap, x, y);
    }

    void add_points(const std::vector<std::pair<int, int>>& points)
    {
        for(const auto& pair : points)
        {
            // handling to allow ctrl-c to interrupt.
            if (PyErr_CheckSignals() != 0)
            {
                throw py::error_already_set();
            }
            heatmap_add_point(heatmap, pair.first, pair.second);
        }
    }

    void add_point_with_stamp(int x, int y, const PyHeatmapStamp& stamp)
    {
        heatmap_add_point_with_stamp(heatmap, x, y, stamp.get_stamp());
    }

    void add_weighted_point_with_stamp(int x, int y, float weight, const PyHeatmapStamp& stamp)
    {
        heatmap_add_weighted_point_with_stamp(heatmap, x, y, weight, stamp.get_stamp());
    }

    void add_weighted_points_with_stamp(const std::vector<std::pair<int, int>>& points, float weight, const PyHeatmapStamp& stamp)
    {
        for(const auto& pair : points)
        {
            // handling to allow ctrl-c to interrupt.
            if (PyErr_CheckSignals() != 0)
            {
                throw py::error_already_set();
            }
            heatmap_add_weighted_point_with_stamp(heatmap, pair.first, pair.second, weight, stamp.get_stamp());
        }
    }
    
    void add_points_with_stamp(const std::vector<std::pair<int, int>>& points, const PyHeatmapStamp& stamp)
    {
        const heatmap_stamp_t* stamp_ptr = stamp.get_stamp();
        for(const auto& pair : points)
        {
            // handling to allow ctrl-c to interrupt.
            if (PyErr_CheckSignals() != 0)
            {
                throw py::error_already_set();
            }
            heatmap_add_point_with_stamp(heatmap, pair.first, pair.second, stamp_ptr);
        }
    }

    py::bytes render_default()
    {
        if (heatmap == nullptr)
        {
            return py::bytes();
        }
        size_t buf_size = heatmap->w*heatmap->h*4;
        char* retbuf = reinterpret_cast<char*>(malloc(buf_size));
        if (retbuf == nullptr)
        {
            return py::bytes();
        }
        unsigned char* pixels = reinterpret_cast<unsigned char*>(retbuf);
        heatmap_render_default_to(heatmap, pixels);
        return py::bytes(retbuf, buf_size);
    }

    py::bytes render_with_color_scheme(PyColorScheme colorScheme)
    {
        if (heatmap == nullptr)
        {
            return py::bytes();
        }
        size_t buf_size = heatmap->w*heatmap->h*4;
        char* retbuf = reinterpret_cast<char*>(malloc(buf_size));
        if (retbuf == nullptr)
        {
            return py::bytes();
        }
        unsigned char* pixels = reinterpret_cast<unsigned char*>(retbuf);
        const heatmap_colorscheme_t* colorscheme = GetColorScheme(colorScheme);        
        heatmap_render_to(heatmap, colorscheme, pixels);
        return py::bytes(retbuf, buf_size);
    }

    py::bytes render_saturated(PyColorScheme colorScheme, float saturation)
    {
        if (heatmap == nullptr)
        {
            return py::bytes();
        }
        size_t buf_size = heatmap->w*heatmap->h*4;
        char* retbuf = reinterpret_cast<char*>(malloc(buf_size));
        if (retbuf == nullptr)
        {
            return py::bytes();
        }
        unsigned char* pixels = reinterpret_cast<unsigned char*>(retbuf);
        const heatmap_colorscheme_t* colorscheme = GetColorScheme(colorScheme);
        heatmap_render_saturated_to(heatmap, colorscheme, saturation, pixels);
        return py::bytes(retbuf, buf_size);
    }

private:

    heatmap_t* heatmap = nullptr;
};

PYBIND11_MODULE(pyheatmap, m) {
    m.doc() = R"pbdoc(
        Pybind11 heatmap plugin
        -----------------------
        .. currentmodule:: pyheatmap
        .. autosummary::
           :toctree: _generate

    )pbdoc";

    py::class_<PyHeatmapStamp>(m, "PyHeatmapStamp")
        .def(py::init([](unsigned radius) { return new PyHeatmapStamp(radius); }))
        .def(py::init([](unsigned width, unsigned height, const std::vector<float>& data) { return new PyHeatmapStamp(width, height, data); }))
        .def(py::init([](unsigned radius, const std::function<float(float)> &f) { return new PyHeatmapStamp(radius, f); }))
        .def("__eq__", [](const PyHeatmapStamp &a, const PyHeatmapStamp& b) {
            return a == b;
        }, py::is_operator())
        .def("width", &PyHeatmapStamp::get_width,
        R"pbdoc(
            returns stamp width
        )pbdoc")
        .def("height", &PyHeatmapStamp::get_height,
        R"pbdoc(
            returns stamp height
        )pbdoc")
        .def("debug_print", &PyHeatmapStamp::debug_print,
        R"pbdoc(
            prints stamps floats.
        )pbdoc")
        .def("almost_eq", &PyHeatmapStamp::almost_eq,
        R"pbdoc(
            is stamp almost equal.
        )pbdoc");

    auto colorScheme = py::enum_<PyColorScheme>(m, "PyColorScheme");

#define COLOR_SCHEME_ITEM(a) #a,
const char *color_scheme_name[] = {
  COLOR_SCHEME_TABLE
};
#undef COLOR_SCHEME_ITEM    

    for(int i = 0; i < PyColorScheme::eMax; i++)
    {
        colorScheme = colorScheme.value(color_scheme_name[i], static_cast<PyColorScheme>(i));        
    }

    colorScheme.export_values();

    py::class_<PyHeatmap>(m, "PyHeatmap")
        .def(py::init([](unsigned w, unsigned h) { return new PyHeatmap(w, h); }))
        .def("__eq__", [](const PyHeatmap &a, const PyHeatmap& b) {
            return a == b;
        }, py::is_operator())
        .def("max", &PyHeatmap::get_max,
        R"pbdoc(
            The highest heat in the whole map. Used for normalization.
        )pbdoc")
        .def("add_point", &PyHeatmap::add_point,
        R"pbdoc(
            Adds a single point to the heatmap using the default stamp.
        )pbdoc")
        .def("add_point_with_stamp", &PyHeatmap::add_point_with_stamp,
        R"pbdoc(
            Adds a single point to the heatmap using a custom stamp.
        )pbdoc")
        .def("add_points", &PyHeatmap::add_points,
        R"pbdoc(
            Adds multiple points to the heatmap using the default stamp.
        )pbdoc")
        .def("add_points_with_stamp", &PyHeatmap::add_points_with_stamp,
        R"pbdoc(
            Adds multiple points to the heatmap using a custom stamp.
        )pbdoc")
        .def("add_weighted_point_with_stamp", &PyHeatmap::add_weighted_point_with_stamp,
        R"pbdoc(
            Adds a single weighted point to the heatmap using a given stamp.
        )pbdoc")
        .def("add_weighted_points_with_stamp", &PyHeatmap::add_weighted_points_with_stamp,
        R"pbdoc(
            Adds multiple weighted points to the heatmap using a given stamp.
        )pbdoc")
        .def("render_default", &PyHeatmap::render_default,
        R"pbdoc(
            Render the heatmap with the default color scheme to a byte array which can be converted to an image.
        )pbdoc")
        .def("render_with_color_scheme", &PyHeatmap::render_with_color_scheme,
        R"pbdoc(
            Render the heatmap with a color scheme to a byte array which can be converted to an image.
        )pbdoc")
        .def("render_saturated", &PyHeatmap::render_saturated,
        R"pbdoc(
            Renders an RGB image of the heatmap into the given colorbuf,
            using a given colorscheme.
            saturation: The heatmap will be truncated at the given heat value, meaning
                         all spots hotter than `saturation` will be assigned the same
                         color as the hottest color on the scale.
        )pbdoc")
        .def("is_stamp_equal", &PyHeatmap::is_stamp_equal,
        R"pbdoc(
            Check to see if the heatmap is equal to the stamp. used for testing
        )pbdoc")
; 
                       

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
