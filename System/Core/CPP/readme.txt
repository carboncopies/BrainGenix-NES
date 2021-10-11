There are 2 ways to build this example:
1. Using cmake

  cd build
  cmake ..
  make

2. With shell script

  sh scripts/build.sh

Note: the cmake way should be considered the primary one, and the shell script is added for illustration of what's going on behind the scenes.

After any of the 2 ways the result is 2 files in artifacts directory: artifacts/bin/_pycpptest.so is the wrapped library and artifacts/py/pycpptest.py contains the python-side bindings.

The usage example is in the python/pytest.py.

## NOTE: REMOVED SCRIPT TO BUILD WITHOUT CMAKE