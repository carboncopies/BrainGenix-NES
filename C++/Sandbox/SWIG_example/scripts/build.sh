SOURCE_ROOT_DIR=$(dirname $(dirname $0))

CPP_INCLUDE_DIR=${SOURCE_ROOT_DIR}/cpp/include

# The directories below are made to match ones that result from cmake build

CPPTEST_OBJ_DIR=${SOURCE_ROOT_DIR}/build/CMakeFiles/cpptest.dir/cpp/src
SWIG_GENERATED_DIR=${SOURCE_ROOT_DIR}/build/swig_generated
SWIG_OBJ_DIR=${SOURCE_ROOT_DIR}/build/CMakeFiles/pycpptest.dir/swig_generated
LIBRARY_OUTPUT_DIR=${SOURCE_ROOT_DIR}/artifacts/bin
WRAPPER_OUTPUT_DIR=${SOURCE_ROOT_DIR}/artifacts/py

mkdir -p ${CPPTEST_OBJ_DIR}
mkdir -p ${SWIG_GENERATED_DIR}
mkdir -p ${SWIG_OBJ_DIR}
mkdir -p ${LIBRARY_OUTPUT_DIR}
mkdir -p ${WRAPPER_OUTPUT_DIR}

CXX_WRAPPER_FILENAME=pycpptestPYTHON_wrap.cxx

# compile the c++ library into object
g++ -c -fPIC -o ${CPPTEST_OBJ_DIR}/cpptest.cpp.o -I${CPP_INCLUDE_DIR} ${SOURCE_ROOT_DIR}/cpp/src/cpptest.cpp

# generate wrapper, which consists of c++ source part (-o ...) and python part (-outdir ... -module ...)
swig -c++ -python -I${CPP_INCLUDE_DIR} -o ${SWIG_GENERATED_DIR}/${CXX_WRAPPER_FILENAME} -outdir ${WRAPPER_OUTPUT_DIR} -module pycpptest ${SOURCE_ROOT_DIR}/wrap/pycpptest.i

# compile the c++ part of generated wrapper into object
g++ -fPIC -c $(pkg-config --cflags python3) -o ${SWIG_OBJ_DIR}/${CXX_WRAPPER_FILENAME}.o -I${CPP_INCLUDE_DIR} ${SWIG_GENERATED_DIR}/${CXX_WRAPPER_FILENAME}

# link library and wrapper objects into .so
g++ -shared -o ${LIBRARY_OUTPUT_DIR}/_pycpptest.so ${SWIG_OBJ_DIR}/${CXX_WRAPPER_FILENAME}.o ${CPPTEST_OBJ_DIR}/cpptest.cpp.o
