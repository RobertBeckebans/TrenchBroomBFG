rm -rf build
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="cmake/packages" -DCMAKE_BUILD_TYPE=Release ..
