/* pycolorschemes - High performance heatmap creation in C++ and python
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
#ifndef PY_COLOR_SCHEMES_H
#define PY_COLOR_SCHEMES_H

#include "heatmap.h"

#define COLOR_SCHEME_TABLE \
        COLOR_SCHEME_ITEM(b2w) \
        COLOR_SCHEME_ITEM(b2w_opaque) \
        COLOR_SCHEME_ITEM(w2b) \
        COLOR_SCHEME_ITEM(w2b_opaque) \
        COLOR_SCHEME_ITEM(Blues_discrete) \
        COLOR_SCHEME_ITEM(Blues_soft) \
        COLOR_SCHEME_ITEM(Blues_mixed) \
        COLOR_SCHEME_ITEM(Blues_mixed_exp) \
        COLOR_SCHEME_ITEM(BrBG_discrete) \
        COLOR_SCHEME_ITEM(BrBG_soft) \
        COLOR_SCHEME_ITEM(BrBG_mixed) \
        COLOR_SCHEME_ITEM(BrBG_mixed_exp) \
        COLOR_SCHEME_ITEM(BuGn_discrete) \
        COLOR_SCHEME_ITEM(BuGn_soft) \
        COLOR_SCHEME_ITEM(BuGn_mixed) \
        COLOR_SCHEME_ITEM(BuGn_mixed_exp) \
        COLOR_SCHEME_ITEM(BuPu_discrete) \
        COLOR_SCHEME_ITEM(BuPu_soft) \
        COLOR_SCHEME_ITEM(BuPu_mixed) \
        COLOR_SCHEME_ITEM(BuPu_mixed_exp) \
        COLOR_SCHEME_ITEM(GnBu_discrete) \
        COLOR_SCHEME_ITEM(GnBu_soft) \
        COLOR_SCHEME_ITEM(GnBu_mixed) \
        COLOR_SCHEME_ITEM(GnBu_mixed_exp) \
        COLOR_SCHEME_ITEM(Greens_discrete) \
        COLOR_SCHEME_ITEM(Greens_soft) \
        COLOR_SCHEME_ITEM(Greens_mixed) \
        COLOR_SCHEME_ITEM(Greens_mixed_exp) \
        COLOR_SCHEME_ITEM(Greys_discrete) \
        COLOR_SCHEME_ITEM(Greys_soft) \
        COLOR_SCHEME_ITEM(Greys_mixed) \
        COLOR_SCHEME_ITEM(Greys_mixed_exp) \
        COLOR_SCHEME_ITEM(Oranges_discrete) \
        COLOR_SCHEME_ITEM(Oranges_soft) \
        COLOR_SCHEME_ITEM(Oranges_mixed) \
        COLOR_SCHEME_ITEM(Oranges_mixed_exp) \
        COLOR_SCHEME_ITEM(OrRd_discrete) \
        COLOR_SCHEME_ITEM(OrRd_soft) \
        COLOR_SCHEME_ITEM(OrRd_mixed) \
        COLOR_SCHEME_ITEM(OrRd_mixed_exp) \
        COLOR_SCHEME_ITEM(PiYG_discrete) \
        COLOR_SCHEME_ITEM(PiYG_soft) \
        COLOR_SCHEME_ITEM(PiYG_mixed) \
        COLOR_SCHEME_ITEM(PiYG_mixed_exp) \
        COLOR_SCHEME_ITEM(PRGn_discrete) \
        COLOR_SCHEME_ITEM(PRGn_soft) \
        COLOR_SCHEME_ITEM(PRGn_mixed) \
        COLOR_SCHEME_ITEM(PRGn_mixed_exp) \
        COLOR_SCHEME_ITEM(PuBuGn_discrete) \
        COLOR_SCHEME_ITEM(PuBuGn_soft) \
        COLOR_SCHEME_ITEM(PuBuGn_mixed) \
        COLOR_SCHEME_ITEM(PuBuGn_mixed_exp) \
        COLOR_SCHEME_ITEM(PuBu_discrete) \
        COLOR_SCHEME_ITEM(PuBu_soft) \
        COLOR_SCHEME_ITEM(PuBu_mixed) \
        COLOR_SCHEME_ITEM(PuBu_mixed_exp) \
        COLOR_SCHEME_ITEM(PuOr_discrete) \
        COLOR_SCHEME_ITEM(PuOr_soft) \
        COLOR_SCHEME_ITEM(PuOr_mixed) \
        COLOR_SCHEME_ITEM(PuOr_mixed_exp) \
        COLOR_SCHEME_ITEM(PuRd_discrete) \
        COLOR_SCHEME_ITEM(PuRd_soft) \
        COLOR_SCHEME_ITEM(PuRd_mixed) \
        COLOR_SCHEME_ITEM(PuRd_mixed_exp) \
        COLOR_SCHEME_ITEM(Purples_discrete) \
        COLOR_SCHEME_ITEM(Purples_soft) \
        COLOR_SCHEME_ITEM(Purples_mixed) \
        COLOR_SCHEME_ITEM(Purples_mixed_exp) \
        COLOR_SCHEME_ITEM(RdBu_discrete) \
        COLOR_SCHEME_ITEM(RdBu_soft) \
        COLOR_SCHEME_ITEM(RdBu_mixed) \
        COLOR_SCHEME_ITEM(RdBu_mixed_exp) \
        COLOR_SCHEME_ITEM(RdGy_discrete) \
        COLOR_SCHEME_ITEM(RdGy_soft) \
        COLOR_SCHEME_ITEM(RdGy_mixed) \
        COLOR_SCHEME_ITEM(RdGy_mixed_exp) \
        COLOR_SCHEME_ITEM(RdPu_discrete) \
        COLOR_SCHEME_ITEM(RdPu_soft) \
        COLOR_SCHEME_ITEM(RdPu_mixed) \
        COLOR_SCHEME_ITEM(RdPu_mixed_exp) \
        COLOR_SCHEME_ITEM(RdYlBu_discrete) \
        COLOR_SCHEME_ITEM(RdYlBu_soft) \
        COLOR_SCHEME_ITEM(RdYlBu_mixed) \
        COLOR_SCHEME_ITEM(RdYlBu_mixed_exp) \
        COLOR_SCHEME_ITEM(RdYlGn_discrete) \
        COLOR_SCHEME_ITEM(RdYlGn_soft) \
        COLOR_SCHEME_ITEM(RdYlGn_mixed) \
        COLOR_SCHEME_ITEM(RdYlGn_mixed_exp) \
        COLOR_SCHEME_ITEM(Reds_discrete) \
        COLOR_SCHEME_ITEM(Reds_soft) \
        COLOR_SCHEME_ITEM(Reds_mixed) \
        COLOR_SCHEME_ITEM(Reds_mixed_exp) \
        COLOR_SCHEME_ITEM(Spectral_discrete) \
        COLOR_SCHEME_ITEM(Spectral_soft) \
        COLOR_SCHEME_ITEM(Spectral_mixed) \
        COLOR_SCHEME_ITEM(Spectral_mixed_exp) \
        COLOR_SCHEME_ITEM(YlGnBu_discrete) \
        COLOR_SCHEME_ITEM(YlGnBu_soft) \
        COLOR_SCHEME_ITEM(YlGnBu_mixed) \
        COLOR_SCHEME_ITEM(YlGnBu_mixed_exp) \
        COLOR_SCHEME_ITEM(YlGn_discrete) \
        COLOR_SCHEME_ITEM(YlGn_soft) \
        COLOR_SCHEME_ITEM(YlGn_mixed) \
        COLOR_SCHEME_ITEM(YlGn_mixed_exp) \
        COLOR_SCHEME_ITEM(YlOrBr_discrete) \
        COLOR_SCHEME_ITEM(YlOrBr_soft) \
        COLOR_SCHEME_ITEM(YlOrBr_mixed) \
        COLOR_SCHEME_ITEM(YlOrBr_mixed_exp) \
        COLOR_SCHEME_ITEM(YlOrRd_discrete) \
        COLOR_SCHEME_ITEM(YlOrRd_soft) \
        COLOR_SCHEME_ITEM(YlOrRd_mixed) \
        COLOR_SCHEME_ITEM(YlOrRd_mixed_exp)

#define COLOR_SCHEME_ITEM(a) e##a,
enum PyColorScheme {
  COLOR_SCHEME_TABLE
  eMax
};
#undef COLOR_SCHEME_ITEM

const heatmap_colorscheme_t* GetColorScheme(PyColorScheme scheme);

#endif