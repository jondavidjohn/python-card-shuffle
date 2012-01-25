from distutils.core import setup, Extension

module = Extension('shuffle', sources=['shufflemodule.c'])
setup(name='Card Shuffling for Python Lists', version='1.0', ext_modules=[module])
