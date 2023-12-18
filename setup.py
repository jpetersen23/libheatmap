# Available at setup time due to pyproject.toml
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"

cpp_args = ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.7', '-fvisibility=hidden']

# The main interface is through Pybind11Extension.
# * You can add cxx_std=11/14/17, and then build_ext can be removed.
# * You can set include_pybind11=false to add the include directory yourself,
#   say from a submodule.
#
# Note:
#   Sort input source files if you glob sources to ensure bit-for-bit
#   reproducible builds (https://github.com/pybind/python_example/pull/53)

ext_modules = [
    Pybind11Extension(
        "pyheatmap",
        ['heatmap.cpp',
         'colorschemes/gray.cpp',
        'colorschemes/Blues.cpp',
        'colorschemes/BrBG.cpp',
        'colorschemes/BuGn.cpp',
        'colorschemes/BuPu.cpp',
        'colorschemes/GnBu.cpp',
        'colorschemes/Greens.cpp',
        'colorschemes/Greys.cpp',
        'colorschemes/Oranges.cpp',
        'colorschemes/OrRd.cpp',
        'colorschemes/PiYG.cpp',
        'colorschemes/PRGn.cpp',
        'colorschemes/PuBuGn.cpp',
        'colorschemes/PuBu.cpp',
        'colorschemes/PuOr.cpp',
        'colorschemes/PuRd.cpp',
        'colorschemes/Purples.cpp',
        'colorschemes/RdBu.cpp',
        'colorschemes/RdGy.cpp',
        'colorschemes/RdPu.cpp',
        'colorschemes/RdYlBu.cpp',
        'colorschemes/RdYlGn.cpp',
        'colorschemes/Reds.cpp',
        'colorschemes/Spectral.cpp',
        'colorschemes/YlGnBu.cpp',
        'colorschemes/YlGn.cpp',
        'colorschemes/YlOrBr.cpp',
        'colorschemes/YlOrRd.cpp',
        'pyheatmap.cpp', 
        'pycolorschemes.cpp'],
        include_dirs=['.', 'extern/pybind11/include'],
        language='c++',
        extra_compile_args = cpp_args,

        # Example: passing in the version to the compiled code
        define_macros=[("VERSION_INFO", __version__)],
    ),
]

setup(
    name='pyheatmap',
    author='Lucas Beyer',
    author_email='',
    version=__version__,
    url="https://github.com/lucasb-eyer/libheatmap",
    description="libheatmap python integration through pybind11",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
